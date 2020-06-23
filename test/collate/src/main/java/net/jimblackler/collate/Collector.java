package net.jimblackler.collate;

import com.google.api.client.googleapis.auth.oauth2.GoogleCredential;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.jackson2.JacksonFactory;
import com.google.api.services.toolresults.ToolResults;
import com.google.api.services.toolresults.model.Execution;
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

class Collector {
  private static final Pattern BAD_CHARS = Pattern.compile("[^a-zA-Z0-9-_.]");

  static void deviceCollect(String appName, Consumer<? super JSONArray> emitter)
      throws IOException {
    // Requires adb root
    Path outputFile = Files.createTempFile("data-", ".json");
    // noinspection HardcodedFileSeparator
    String files = Utils.execute("adb", "shell", "find",
        "/storage/emulated/0/Android/data/" + appName + "/files", "-type", "f");
    for (String file : files.split(System.lineSeparator())) {
      Utils.execute("adb", "pull", file, outputFile.toString());
      collectResult(emitter, Files.readString(outputFile), null);
    }
  }

  static void cloudCollect(String historyId, Consumer<? super JSONArray> emitter)
      throws IOException {
    int sleepFor = 0;
    int ioSleepFor = 0;

    String projectId = Utils.getProjectId();

    Map<String, JSONObject> launcherDevices = new HashMap<>();
    DeviceFetcher.fetch(device -> { launcherDevices.put(device.getString("id"), device); });

    int inProgress = 0;
    Collection<String> reported = new HashSet<>();
    do {
      String accessToken = Auth.getAccessToken();
      try {
        GoogleCredentials credentials1 =
            GoogleCredentials.create(new AccessToken(accessToken, null));
        Storage storage =
            StorageOptions.newBuilder().setCredentials(credentials1).build().getService();

        GoogleCredential credentials =
            new GoogleCredential.Builder().build().setAccessToken(accessToken);
        ToolResults toolResults =
            new ToolResults
                .Builder(new NetHttpTransport(), new JacksonFactory(),
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

        while (true) {
          int fetched = 0;
          inProgress = 0;
          ToolResults.Projects.Histories.Executions.List list =
              toolResults.projects().histories().executions().list(projectId, historyId);
          while (true) {
            ListExecutionsResponse response = list.execute();
            if (response == null) {
              break;
            }
            for (Execution execution : response.getExecutions()) {
              String executionId = execution.getExecutionId();
              ToolResults.Projects.Histories.Executions.Steps.List list1 =
                  toolResults.projects()
                      .histories()
                      .executions()
                      .steps()
                      .list(projectId, historyId, executionId);
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
                  fetched++;
                  JSONObject extra = new JSONObject();
                  extra.put("historyId", historyId);
                  extra.put("step", new JSONObject(step.toString()));
                  // noinspection HardcodedFileSeparator
                  extra.put("resultsPage",
                      "https://console.firebase.google.com/project/" + projectId
                          + "/testlab/histories/" + historyId + "/matrices/" + executionId
                          + "/executions/" + stepId);
                  for (StepDimensionValueEntry entry : step.getDimensionValue()) {
                    if ("Model".equals(entry.getKey())) {
                      String id = entry.getValue();
                      if (launcherDevices.containsKey(id)) {
                        extra.put("fromLauncher", launcherDevices.get(id));
                      }
                      break;
                    }
                  }

                  List<ToolOutputReference> toolOutputs = toolExecution.getToolOutputs();
                  String contents = "";
                  if (toolOutputs != null) {
                    for (ToolOutputReference toolOutputReference : toolOutputs) {
                      URI uri = URI.create(toolOutputReference.getOutput().getFileUri());
                      if (!uri.getPath().endsWith(".json")) {
                        continue;
                      }
                      try {
                        contents = getContents(storage, uri.getHost(), uri.getPath().substring(1));
                        break;
                      } catch (IOException e) {
                        e.printStackTrace();
                      }
                    }
                  }
                  collectResult(emitter, contents, extra);
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
          if (fetched == 0) {
            sleepFor += 10;
            System.out.print("Nothing fetched so sleeping for " + sleepFor + " seconds... ");
            try {
              Thread.sleep(sleepFor * 1000L);
            } catch (InterruptedException ignored) {
              // Ignored by design.
            }
            System.out.println("done");
          } else {
            sleepFor = 0;
          }
        }
        ioSleepFor = 0;
      } catch (IOException e) {
        e.printStackTrace();
        ioSleepFor += 10;
        System.out.print("IO error so sleeping for " + ioSleepFor + " seconds... ");
        try {
          Thread.sleep(ioSleepFor * 1000L);
        } catch (InterruptedException ignored) {
          // Ignored by design.
        }
        System.out.println("done");
      }
    } while (inProgress != 0 || reported.isEmpty());
  }

  private static void collectResult(
      Consumer<? super JSONArray> emitter, String text, JSONObject extra) {
    JSONArray data = new JSONArray();
    text.lines().forEach(line -> {
      line = line.trim();
      try {
        JSONObject value = new JSONObject(line);
        data.put(value);
      } catch (JSONException e) {
        System.out.println("Not JSON: " + line);
      }
    });
    if (data.isEmpty()) {
      data.put(new JSONObject());
    }
    data.getJSONObject(0).put("extra", extra);
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
