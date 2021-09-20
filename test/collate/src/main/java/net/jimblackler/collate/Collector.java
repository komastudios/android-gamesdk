package net.jimblackler.collate;

import static com.google.api.client.googleapis.javanet.GoogleNetHttpTransport.newTrustedTransport;
import static net.jimblackler.collate.JsonUtils.getSchema;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;
import com.fasterxml.jackson.dataformat.smile.databind.SmileMapper;
import com.google.api.client.http.HttpRequestInitializer;
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
import com.google.auth.http.HttpCredentialsAdapter;
import com.google.auth.oauth2.GoogleCredentials;
import com.google.cloud.datastore.DatastoreOptions;
import com.google.cloud.datastore.Query;
import com.google.cloud.datastore.StructuredQuery;
import com.google.cloud.storage.Blob;
import com.google.cloud.storage.BlobId;
import com.google.cloud.storage.Storage;
import com.google.cloud.storage.StorageOptions;
import com.google.common.collect.ImmutableList;
import java.io.IOException;
import java.net.URI;
import java.nio.charset.MalformedInputException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.GeneralSecurityException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;
import java.util.regex.Pattern;
import org.everit.json.schema.Schema;
import org.everit.json.schema.ValidationException;
import org.json.JSONObject;

class Collector {
  private static final Pattern BAD_CHARS = Pattern.compile("[^a-zA-Z0-9-_.]");
  private static final Pattern LINE_SPLIT = Pattern.compile("\\r?\\n");

  static void deviceCollect(String appName, Consumer<List<Map<String, Object>>> emitter)
      throws IOException {
    Schema resultsRowSchema = getSchema("resultsRow.schema.json");
    // Requires adb root
    Path outputFile = Files.createTempFile("data-", ".json");
    // noinspection HardcodedFileSeparator
    String files = Utils.execute("adb", "shell", "find",
        "/storage/emulated/0/Android/data/" + appName + "/files", "-type", "f");
    for (String file : files.split(System.lineSeparator())) {
      Utils.execute("adb", "pull", file, outputFile.toString());
      collectResult(emitter, FileUtils.readFile(outputFile), null, resultsRowSchema);
    }
  }

  static void cloudCollect(String historyId, Consumer<List<Map<String, Object>>> emitter)
      throws IOException {
    int sleepFor = 0;
    int ioSleepFor = 0;

    String projectId = Utils.getProjectId();

    Schema resultsRowSchema = getSchema("resultsRow.schema.json");
    Map<String, Map<String, Object>> launcherDevices = new HashMap<>();
    DeviceFetcher.fetch(device -> launcherDevices.put((String) device.get("id"), device));

    int inProgress = 0;
    Collection<String> reported = new HashSet<>();
    NetHttpTransport httpTransport = null;
    try {
      httpTransport = newTrustedTransport();
    } catch (GeneralSecurityException e) {
      throw new IOException(e);
    }
    JacksonFactory jsonFactory = new JacksonFactory();
    GoogleCredentials googleCredentials = AuthUserFlow.runAuthUserFlow(
        Collector.class.getResourceAsStream("/clientSecrets.json"), jsonFactory, httpTransport,
        ImmutableList.of("https://www.googleapis.com/auth/devstorage.read_only",
            "https://www.googleapis.com/auth/cloud-platform"));
    HttpRequestInitializer requestInitializer = new HttpCredentialsAdapter(googleCredentials);
    do {
      try {
        Storage storage =
            StorageOptions.newBuilder().setCredentials(googleCredentials).build().getService();
        ToolResults toolResults =
            new ToolResults.Builder(new NetHttpTransport(), jsonFactory, requestInitializer)
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
                  toolResults.projects().histories().executions().steps().list(
                      projectId, historyId, executionId);
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
                  Map<String, Object> extra = new LinkedHashMap<>();
                  extra.put("historyId", historyId);
                  extra.put("step", step);
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
                  collectResult(emitter, contents, extra, resultsRowSchema);
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

  private static void collectResult(Consumer<List<Map<String, Object>>> emitter, String text,
      Map<String, Object> extra, Schema resultsRowSchema) {
    ObjectMapper objectMapper = new ObjectMapper();
    boolean validate = false;
    List<Map<String, Object>> data = new ArrayList<>();
    Arrays.asList(LINE_SPLIT.split(text)).forEach(line -> {
      line = line.trim();
      try {
        Map<String, Object> value = objectMapper.readValue(line, Map.class);
        if (validate) {
          resultsRowSchema.validate(new JSONObject(line));
        }
        data.add(value);
      } catch (JsonProcessingException e) {
        System.out.println("Not JSON: " + line);
      } catch (ValidationException e) {
        System.out.println("Did not validate: " + e.getMessage());
      }
    });
    if (data.isEmpty()) {
      data.add(new LinkedHashMap<>());
    }
    if (extra != null) {
      data.get(0).put("extra", extra);
    }
    emitter.accept(data);
  }

  private static String getContents(Storage storage, String bucketName, String path)
      throws IOException {
    Path file = Paths.get("cache", bucketName, BAD_CHARS.matcher(path).replaceAll("_"));

    if (!Files.exists(file)) {
      System.out.println("Fetching " + path);
      file.toFile().getParentFile().mkdirs();
      BlobId blobId = BlobId.of(bucketName, path);
      Blob blob = storage.get(blobId);
      if (blob == null) {
        throw new IOException("Could not get blob " + blobId);
      } else {
        blob.downloadTo(file);
      }
    }
    try {
      String s = FileUtils.readFile(file);
      if (s.isEmpty()) {
        Files.delete(file);  // Don't cache an empty file.
      }
      return s;
    } catch (MalformedInputException e) {
      throw new IOException(e);
    }
  }

  static void collectDataStoreResult(DataStoreResultConsumer consumer, String version)
      throws IOException {
    ObjectMapper objectMapper = new ObjectMapper();
    ObjectReader objectReader = objectMapper.reader();
    ObjectReader smileReader = new SmileMapper().reader();
    Collection<Number> runTimes = new HashSet<>();
    NetHttpTransport httpTransport;
    try {
      httpTransport = newTrustedTransport();
    } catch (GeneralSecurityException e) {
      throw new IOException(e);
    }
    JacksonFactory jsonFactory = new JacksonFactory();
    String secretsName = "/clientSecrets_gatherBenchmarks.json";
    GoogleCredentials googleCredentials =
        AuthUserFlow.runAuthUserFlow(Collector.class.getResourceAsStream(secretsName), jsonFactory,
            httpTransport, ImmutableList.of("https://www.googleapis.com/auth/datastore"));
    Map<String, Object> secrets =
        jsonFactory.fromInputStream(Collector.class.getResourceAsStream(secretsName), Map.class);
    Map<String, Object> installed = (Map<String, Object>) secrets.get("installed");
    String projectId = (String) installed.get("project_id");
    DatastoreOptions.newBuilder()
        .setCredentials(googleCredentials)
        .setProjectId(projectId)
        .build()
        .getService()
        .run(Query.newEntityQueryBuilder()
                 .setKind("Benchmark")
                 .setFilter(StructuredQuery.CompositeFilter.and(
                     StructuredQuery.PropertyFilter.eq("versionName", version),
                     StructuredQuery.PropertyFilter.eq("testType", "management")))
                 .build())
        .forEachRemaining(entity -> {
          Map<String, Object> results1;
          try {
            if (entity.contains("resultsSmile")) {
              com.google.cloud.datastore.Blob resultsSmile = entity.getBlob("resultsSmile");
              results1 = smileReader.readValue(resultsSmile.toByteArray(), Map.class);
            } else {
              results1 = objectReader.readValue(entity.getString("results"), Map.class);
            }
          } catch (IOException e) {
            throw new IllegalStateException(e);
          }

          Number runTime = (Number) results1.get("runTime");
          if (!runTimes.add(runTime)) {
            System.out.println("Duplicate run seen");
            return;
          }

          System.out.println("Registering run number " + runTimes.size());

          List<List<Map<String, Object>>> tests1 =
              (List<List<Map<String, Object>>>) results1.get("tests");

          Map<String, Object> buildConfig = (Map<String, Object>) results1.get("buildConfig");

          String versionName = (String) buildConfig.get("VERSION_NAME");

          List<List<Map<String, Object>>> results =
              (List<List<Map<String, Object>>>) results1.get("results");
          for (int testNumber = 0; testNumber != results.size(); testNumber++) {
            List<Map<String, Object>> result = results.get(testNumber);
            Map<String, Object> paramsIn = new LinkedHashMap<>();
            paramsIn.put("tests", tests1);
            paramsIn.put("coordinates", getCoordinates(testNumber, tests1));
            paramsIn.put("run", versionName);
            if (!result.isEmpty()) {
              Map<String, Object> first = result.get(0);
              if (first != null) {
                first.put("params", paramsIn);
              }
            }
            consumer.accept(results1, result);
          }
        });
  }

  static Collection<Integer> getCoordinates(int testNumber, List<List<Map<String, Object>>> tests) {
    List<Integer> coordinates = new ArrayList<>();
    int calc = testNumber;
    for (int idx = tests.size() - 1; idx >= 0; idx--) {
      int dimensionSize = tests.get(idx).size();
      coordinates.add(0, calc % dimensionSize);
      calc /= dimensionSize;
    }
    return coordinates;
  }

  interface DataStoreResultConsumer {
    void accept(Map<String, Object> results1, List<Map<String, Object>> result);
  }
}
