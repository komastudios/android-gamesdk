package net.jimblackler.collate;

import com.google.common.collect.ImmutableSet;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.Set;
import java.util.TreeSet;
import java.util.stream.Collectors;
import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;

public class GetDevices {
  private static final boolean USE_WHITELIST = false;
  private static final int MIN_VERSION = 26;

  public static void main(String[] args) throws IOException {

    ImmutableSet<String> whitelist =
        ImmutableSet.of(
            "star2qltesq",
            "greatlteks",
            "crownlteks",
            "dreamlte",
            "crownqltesq",
            "greatqlte",
            "starqltesq",
            "dreamqltesq",
            "dream2qltesq",
            "star2lte",
            "starlte",
            "dream2lte",
            "herolte",
            "star2lteks",
            "crownlte",
            "greatlte",
            "hero2lte",
            "SO-04J",
            "dream2lteks",
            "beyond2",
            "taimen",
            "beyond2q",
            "dreamlteks",
            "gtaxlwifi",
            "SO-01K",
            "SO-01J",
            "SOV36",
            "beyondx",
            "heroqltevzw",
            "HWANE",
            "starlteks",
            "a5y17lte",
            "SC-02H");
    String text = execute("gcloud firebase test android models list --format=json".split(" "));
    JSONTokener jsonTokener = new JSONTokener(text);
    JSONArray devices = new JSONArray(jsonTokener);

    Set<String> d2 = new TreeSet<>();
    for (int idx = 0; idx != devices.length(); idx++) {
      JSONObject device = devices.getJSONObject(idx);
      if (!device.getString("formFactor").equals("PHONE")) {
        continue;
      }
      if (!device.getString("form").equals("PHYSICAL")) {
        continue;
      }
      String id = device.getString("id");
      assert id.equals(device.getString("codename"));
      if (USE_WHITELIST && !whitelist.contains(id)) {
        continue;
      }
      JSONArray supportedVersionIds = device.getJSONArray("supportedVersionIds");
      for (int idx2 = 0; idx2 != supportedVersionIds.length(); idx2++) {

        int versionId = supportedVersionIds.getInt(idx2);
        if (versionId < MIN_VERSION) {
          continue;
        }
        //noinspection HardcodedFileSeparator
        System.out.println(String.format("--device model=%s,version=%d \\", id, versionId));
        d2.add(id);
      }
    }

    for (String device : d2) {
      System.out.println(device);
    }
  }

  private static String readStream(InputStream inputStream) throws IOException {
    try (BufferedReader bufferedReader =
        new BufferedReader(new InputStreamReader(inputStream, StandardCharsets.UTF_8))) {
      return bufferedReader.lines().collect(Collectors.joining(System.lineSeparator()));
    }
  }

  private static String execute(String... args) throws IOException {
    return readStream(new ProcessBuilder(args).start().getInputStream());
  }
}
