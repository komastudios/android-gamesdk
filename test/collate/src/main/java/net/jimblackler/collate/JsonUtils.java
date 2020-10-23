package net.jimblackler.collate;

import com.google.android.apps.internal.games.memoryadvice_common.Schemas;
import com.google.android.apps.internal.games.memoryadvice_common.StreamUtils;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.URL;
import org.everit.json.schema.Schema;
import org.everit.json.schema.loader.SchemaLoader;
import org.json.JSONObject;

public class JsonUtils {
  /**
   * Convert a schema from text form in the Memory Advice common library into a org.everit schema
   * for validation.
   * @param name The name of the Schema to fectch.
   * @return The built org.everit Schema object.
   */
  static Schema getSchema(String name) {
    try {
      URL schema = Schemas.getSchema(name);
      return SchemaLoader.load(new JSONObject(StreamUtils.readStream(schema.openStream())), url -> {
        try {
          if (url.startsWith("http://") || url.startsWith("https://")) {
            // Web resources are streamed.
            return new URL(url).openStream();
          }
          // Raw URLs are obtained from the Memory Advice common library.
          return Schemas.getSchema(url).openStream();
        } catch (IOException e1) {
          System.out.println("Couldn't load " + url);  // Warn about the loading error.
          // Don't abort building the whole tree because of this one missing part. Transplant a
          // permissive schema.
          return new ByteArrayInputStream("{}".getBytes());
        }
      });
    } catch (IOException ex) {
      throw new UncheckedIOException(ex);
    }
  }
}
