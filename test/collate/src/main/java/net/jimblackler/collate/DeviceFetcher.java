package net.jimblackler.collate;

import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;

/**
 * A service for fetching information about Firebase Test lab devices.
 */
public class DeviceFetcher {
  /**
   * Fetch information about Firebase Test lab devices.
   *
   * @param consumer The consumer for the device information.
   * @throws IOException There was an error fetching the information.
   */
  static void fetch(Consumer<Map<String, Object>> consumer) throws IOException {
    String[] args = {Config.GCLOUD_EXECUTABLE.toString(), "beta", "firebase", "test", "android",
        "models", "list", "--format", "json"};

    String text = Utils.executeSilent(args);
    List<Object> devices = new ObjectMapper().readValue(text, List.class);
    for (Object device : devices) {
      consumer.accept((Map<String, Object>) device);
    }
  }
}
