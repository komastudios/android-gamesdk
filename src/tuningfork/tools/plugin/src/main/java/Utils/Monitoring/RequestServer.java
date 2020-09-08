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

import Utils.Monitoring.PerformanceParameters.UploadTelemetryRequest;
import Utils.Monitoring.PerformanceParameters.UploadTelemetryRequest.Builder;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class RequestServer {
  public static void listen() throws IOException {
    ServerSocket serverSocket = new ServerSocket(9000);
    new Thread("Device Listener") {
      public void run() {
        try {
          Socket socket = null;
          while ((socket = serverSocket.accept()) != null) {
            String stringResponse = new String(socket.getInputStream().readAllBytes(),
                StandardCharsets.UTF_8);
            System.out.println(stringResponse);
            if (stringResponse.contains("uploadTelemetry HTTP/1.1")) {
              String jsonFromResponse = "{" + stringResponse.split("\\{", 2)[1];
              parseJson(jsonFromResponse);
            }
          }
        } catch (Exception e) {
          e.printStackTrace();
        }
      };
    }.start();
  }

  private static void parseJson(String jsonString) throws InvalidProtocolBufferException {
    Builder telemetryBuilder = UploadTelemetryRequest.newBuilder();
    JsonFormat.parser().ignoringUnknownFields().merge(jsonString, telemetryBuilder);
    UploadTelemetryRequest telemetryRequest = telemetryBuilder.build();
    //TODO @targintaru: pass data to UI and represent it graphically
  }
}
