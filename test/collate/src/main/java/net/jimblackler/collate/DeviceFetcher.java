package net.jimblackler.collate;

import java.io.IOException;
import java.util.function.Consumer;
import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;

/**
 * A service for fetching information about Firebase Test lab devices.
 */
public class DeviceFetcher {
  /**
   * Fetch information about Firebase Test lab devices.
   * @param consumer The consumer for the device information.
   * @throws IOException There was an error fetching the information.
   */
  static void fetch(Consumer<JSONObject> consumer) throws IOException {
    String[] args = {Config.GCLOUD_EXECUTABLE.toString(), "beta", "firebase", "test", "android",
        "models", "list", "--format", "json"};

    String text = Utils.executeSilent(args);
    JSONTokener jsonTokener = new JSONTokener(text);
    JSONArray devices = new JSONArray(jsonTokener);

    for (int idx = 0; idx != devices.length(); idx++) {
      consumer.accept(devices.getJSONObject(idx));
    }
  }
}
