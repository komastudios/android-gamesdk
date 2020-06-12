package net.jimblackler.collate;

import com.google.api.client.googleapis.auth.oauth2.GoogleCredential;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.jackson2.JacksonFactory;
import com.google.api.services.toolresults.ToolResults;
import com.google.api.services.toolresults.model.Execution;
import com.google.api.services.toolresults.model.FileReference;
import com.google.api.services.toolresults.model.ListExecutionsResponse;
import com.google.api.services.toolresults.model.ListHistoriesResponse;
import com.google.api.services.toolresults.model.ListStepsResponse;
import com.google.api.services.toolresults.model.Step;
import com.google.api.services.toolresults.model.StepDimensionValueEntry;
import com.google.api.services.toolresults.model.TestExecutionStep;
import com.google.api.services.toolresults.model.ToolExecution;
import com.google.api.services.toolresults.model.ToolOutputReference;
import com.google.auth.oauth2.AccessToken;
import com.google.auth.oauth2.GoogleCredentials;
import com.google.cloud.storage.BlobId;
import com.google.cloud.storage.Storage;
import com.google.cloud.storage.StorageOptions;
import java.io.IOException;
import java.net.URI;
import java.nio.charset.MalformedInputException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;
import java.util.regex.Pattern;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

class Collector {

  private static final Pattern BAD_CHARS = Pattern.compile("[^a-zA-Z0-9-_.]");
  private static final String FIELDS = "nextPageToken," +
      "steps(state,stepId,dimensionValue,testExecutionStep(toolExecution(toolOutputs,toolLogs)))";

  static void deviceCollect(String appName, Consumer<? super JSONArray> emitter)
      throws IOException {
    // Requires adb root
    Path outputFile = Files.createTempFile("data-", ".json");
    //noinspection HardcodedFileSeparator
    String files = Utils.execute(
        "adb", "shell", "find", "/storage/emulated/0/Android/data/" +
        appName + "/files", "-type", "f");
    for (String file : files.split(System.lineSeparator())) {
      Utils.execute("adb", "pull", file, outputFile.toString());
      collectResult(emitter, Files.readString(outputFile), null);
    }
  }

  static void cloudCollect(String historyId, Consumer<? super JSONArray> emitter)
      throws IOException {
    String projectId = Utils.getProjectId();
    String accessToken = Auth.getAccessToken();

    Map<String, JSONObject> launcherDevices = new HashMap<>();
    DeviceFetcher.fetch(device -> {
      launcherDevices.put(device.getString("id"), device);
    });

    GoogleCredentials credentials1 = GoogleCredentials.create(new AccessToken(accessToken, null));
    Storage storage = StorageOptions.newBuilder().setCredentials(credentials1).build().getService();

    GoogleCredential credentials =
        new GoogleCredential.Builder().build().setAccessToken(accessToken);
    ToolResults toolResults =
        new ToolResults.Builder(
                new NetHttpTransport(),
                new JacksonFactory(),
                new NetHttpTransport().createRequestFactory(credentials).getInitializer())
            .setServicePath(ToolResults.DEFAULT_SERVICE_PATH)
            .setApplicationName(projectId)
            .build();

    if (historyId == null) {
      // If no historyId supplied, get the latest run.
      ListHistoriesResponse historiesResponse =
          toolResults.projects().histories().list(projectId).setPageSize(1).execute();
      historyId = historiesResponse.getHistories().get(0).getHistoryId();
    }

    Collection<String> reported = new HashSet<>();

    do {
      while (true) {
        int inProgress = 0;
        ToolResults.Projects.Histories.Executions.List list =
            toolResults.projects().histories().executions().list(projectId, historyId);
        while (true) {
          ListExecutionsResponse response = list.execute();
          for (Execution execution : response.getExecutions()) {
            String executionId = execution.getExecutionId();
            ToolResults.Projects.Histories.Executions.Steps.List list1 =
                toolResults
                    .projects()
                    .histories()
                    .executions()
                    .steps()
                    .list(projectId, historyId, executionId)
                    .setFields(FIELDS);
            while (true) {
              ListStepsResponse response1 = list1.execute();
              if (response1.isEmpty()) {
                break;
              }
              for (Step step : response1.getSteps()) {
                if ("inProgress".equals(step.getState())) {
                  inProgress++;
                }
                TestExecutionStep testExecutionStep = step.getTestExecutionStep();
                if (testExecutionStep == null) {
                  continue;
                }
                ToolExecution toolExecution = testExecutionStep.getToolExecution();
                if (toolExecution == null) {
                  continue;
                }
                String stepId = step.getStepId();
                if (!reported.add(stepId)) {
                  continue;
                }
                JSONObject extra = new JSONObject();
                //noinspection HardcodedFileSeparator
                extra.put(
                    "resultsPage",
                    "https://console.firebase.google.com/project/" + projectId +
                        "/testlab/histories/"
                        + historyId
                        + "/matrices/"
                        + executionId
                        + "/executions/"
                        + stepId);
                for (StepDimensionValueEntry entry : step.getDimensionValue()) {
                  if ("Model".equals(entry.getKey())) {
                    String id = entry.getValue();
                    if (launcherDevices.containsKey(id)) {
                      extra.put("fromLauncher", launcherDevices.get(id));
                    }
                    break;
                  }
                }
                List<FileReference> toolLogs = toolExecution.getToolLogs();
                if (toolLogs == null) {
                  continue;
                }
                String logsUrl = toolLogs.get(0).getFileUri()
                    .replace("gs://", "https://storage.cloud.google.com/");
                extra.put("logs", logsUrl);
                List<ToolOutputReference> toolOutputs = toolExecution.getToolOutputs();
                if (toolOutputs == null || toolOutputs.isEmpty()) {
                  continue;
                }
                ToolOutputReference toolOutputReference = toolOutputs.get(0);
                URI uri = URI.create(toolOutputReference.getOutput().getFileUri());
                try {
                  String contents = getContents(storage, uri.getHost(), uri.getPath().substring(1));
                  collectResult(emitter, contents, extra);
                } catch (IOException e) {
                  e.printStackTrace();
                }
              }
              String nextPageToken1 = response1.getNextPageToken();
              if (nextPageToken1 == null) {
                break;
              }
              list1.setPageToken(nextPageToken1);
            }
          }
          String nextPageToken = response.getNextPageToken();
          if (nextPageToken == null) {
            break;
          }
          list.setPageToken(nextPageToken);
        }
        if (inProgress == 0) {
          break;
        }
        System.out.println(inProgress + " runs in progress");
      }
    } while (reported.isEmpty());
  }

  private static void collectResult(
      Consumer<? super JSONArray> emitter, String text, JSONObject extra) {
    JSONArray data = new JSONArray();
    JSONTokener jsonTokener = new JSONTokener(text);
    try {
      while (true) {
        JSONObject value = new JSONObject(jsonTokener);
        if (extra != null) {
          value.put("extra", extra);
          extra = null;
        }
        data.put(value);
      }
    } catch (JSONException e) {
      // Silent by design.
    }
    emitter.accept(data);
  }

  private static String getContents(Storage storage, String bucketName, String path)
      throws IOException {
    Path file = Paths.get("cache", bucketName, BAD_CHARS.matcher(path).replaceAll("_"));

    if (!Files.exists(file)) {
      System.out.println("Fetching " + path);
      file.toFile().getParentFile().mkdirs();
      storage.get(BlobId.of(bucketName, path)).downloadTo(file);
    }
    try {
      String s = Files.readString(file);
      if (s.isEmpty()) {
        Files.delete(file);  // Don't cache an empty file.
      }
      return s;
    } catch (MalformedInputException e) {
      throw new IOException(e);
    }
  }
}
