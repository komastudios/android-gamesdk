package com.google.android.apps.internal.games.memoryadvice_common;

import java.net.URL;

public class Schemas {
  /**
   * Supply a schema from the library by name.
   * @param schema The schema filename to obtain.
   * @return The schema in text form.
   */
  public static URL getSchema(String schema) {
    return Schemas.class.getResource("/schemas/" + schema);
  }
}
