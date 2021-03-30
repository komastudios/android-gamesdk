package com.google.android.apps.internal.games.memoryadvice;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

class ByteArrayUtils {
  /**
   * Copies a stream to a direct byte buffer.
   * @param inputStream An input stream.
   * @return The contents of the stream in a direct byte buffer.
   */
  static ByteBuffer streamToDirectByteBuffer(InputStream inputStream) throws IOException {
    byte[] bytes = streamToByteArray(inputStream);
    ByteBuffer byteBuffer = ByteBuffer.allocateDirect(bytes.length);
    byteBuffer.put(bytes);
    return byteBuffer;
  }

  /**
   * Copies a stream to a byte array.
   * @param input An input stream.
   * @return The contents of the stream as a byte array.
   */
  static byte[] streamToByteArray(InputStream input) throws IOException {
    try (ByteArrayOutputStream output = new ByteArrayOutputStream()) {
      byte[] buffer = new byte[1024];
      int bytesRead;
      while ((bytesRead = input.read(buffer)) != -1) {
        output.write(buffer, 0, bytesRead);
      }
      return output.toByteArray();
    }
  }
}
