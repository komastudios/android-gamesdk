/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package Utils.Monitoring;

import com.google.android.performanceparameters.v1.PerformanceParameters.UploadTelemetryRequest;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;
import java.io.IOException;
import java.math.BigDecimal;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.Optional;
import java.util.function.Consumer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class RequestServer {

  private static Thread serverThread;
  private static ServerSocket serverSocket;
  private static boolean isBound = false;

  public static void listen(Consumer<UploadTelemetryRequest> consumer)
      throws IOException {
    serverSocket = new ServerSocket(9000);
    if (serverSocket.isBound()) {
      isBound = true;
    }
    serverThread = new Thread("Device Listener") {
      public void run() {
        try {
          Socket socket = null;
          while ((socket = serverSocket.accept()) != null) {
            String stringResponse = new String(socket.getInputStream().readAllBytes(),
                StandardCharsets.UTF_8);
            if (stringResponse.contains("uploadTelemetry HTTP/1.1")) {
              String jsonFromResponse = "{" + stringResponse.split("\\{", 2)[1];
              consumer.accept(parseJsonTelemetry(jsonFromResponse));
            }
          }
          serverSocket.close();
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
    };
    serverThread.start();
  }

  public static void stopListening() throws IOException {
    if (isBound) {
      serverSocket.close();
      serverThread.interrupt();
      isBound = false;
    }
  }

  private static Optional<String> replaceExponentSubstrings(String jsonString) {
    String exponentPattern = "\"duration\": \"[0-9]+\\.[0-9]+[eE][+-][0-9]+s\"";
    Pattern pattern = Pattern.compile(exponentPattern);
    StringBuilder stringBuilder = new StringBuilder();
    Matcher matcher = pattern.matcher(jsonString);

    boolean found = false;
    while (matcher.find()) {
      found = true;
      String stringToReplace = matcher.group(0);
      stringToReplace = stringToReplace.substring(13, stringToReplace.length() - 2);
      System.out.println(stringToReplace);
      BigDecimal bigDecimal = new BigDecimal(stringToReplace);
      matcher.appendReplacement(stringBuilder, "\"duration\": " +
          Long.toString(bigDecimal.longValue()) + "s");
    }
    matcher.appendTail(stringBuilder);

    if (found) {
      return Optional.of(stringBuilder.toString());
    }
    return Optional.empty();
  }

  private static UploadTelemetryRequest parseJsonTelemetry(String jsonString)
      throws InvalidProtocolBufferException {
    UploadTelemetryRequest.Builder telemetryBuilder = UploadTelemetryRequest.newBuilder();
    Optional<String> replacedSubstrings = replaceExponentSubstrings(jsonString);
    jsonString = replacedSubstrings.isPresent() ? replacedSubstrings.get() : jsonString;
    JsonFormat.parser().ignoringUnknownFields().merge(jsonString, telemetryBuilder);
    return telemetryBuilder.build();
  }
}
