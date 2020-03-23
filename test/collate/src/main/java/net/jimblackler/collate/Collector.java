package net.jimblackler.collate;

import com.google.api.client.googleapis.auth.oauth2.GoogleCredential;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.jackson2.JacksonFactory;
import com.google.api.services.toolresults.ToolResults;
import com.google.api.services.toolresults.model.Execution;
import com.google.api.services.toolresults.model.FileReference;
import com.google.api.services.toolresults.model.History;
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
import java.util.List;
import java.util.function.Consumer;
import java.util.regex.Pattern;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

class Collector {

  private static final Pattern BAD_CHARS = Pattern.compile("[^a-zA-Z0-9-_.]");
  private static final String FIELDS =
      "nextPageToken,"
          + "steps(stepId,dimensionValue,testExecutionStep(toolExecution(toolOutputs,toolLogs)))";

  static void deviceCollect(Consumer<? super JSONArray> emitter, int scenario)
      throws IOException, InterruptedException {
    // Requires adb root
    Path outputFile = Files.createTempFile("data-", ".json");
    //noinspection HardcodedFileSeparator
    Runtime.getRuntime()
        .exec("adb pull /data/media/0/results" + scenario + ".json " + outputFile)
        .waitFor();
    collectResult(emitter, Files.readString(outputFile), null);
  }

  static void cloudCollect(Consumer<? super JSONArray> emitter) throws IOException {

    String projectId = "istresser";

    String accessToken = Auth.getAccessToken();
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

    ListHistoriesResponse historiesResponse =
        toolResults.projects().histories().list(projectId).setPageSize(1).execute();
    History history = historiesResponse.getHistories().get(0);

    String historyId = history.getHistoryId();
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
            TestExecutionStep testExecutionStep = step.getTestExecutionStep();
            if (testExecutionStep == null) {
              continue;
            }
            ToolExecution toolExecution = testExecutionStep.getToolExecution();
            if (toolExecution == null) {
              continue;
            }
            JSONObject extra = new JSONObject();
            //noinspection HardcodedFileSeparator
            extra.put(
                "resultsPage",
                "https://firebase.corp.google.com/project/istresser/testlab/histories/"
                    + historyId
                    + "/matrices/"
                    + executionId
                    + "/executions/"
                    + step.getStepId());
            List<FileReference> toolLogs = toolExecution.getToolLogs();
            if (toolLogs == null) {
              continue;
            }
            String logsUrl =
                toolLogs.get(0).getFileUri().replace("gs://", "https://storage.cloud.google.com/");
            extra.put("logs", logsUrl);

            JSONObject dimensions = new JSONObject();
            for (StepDimensionValueEntry pair : step.getDimensionValue()) {
              dimensions.put(pair.getKey(), pair.getValue());
            }
            extra.put("dimensions", dimensions);
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
      return Files.readString(file);
    } catch (MalformedInputException e) {
      throw new IOException(e);
    }
  }
}
