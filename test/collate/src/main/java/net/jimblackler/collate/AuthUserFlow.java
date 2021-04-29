package net.jimblackler.collate;

import com.google.api.client.extensions.java6.auth.oauth2.AuthorizationCodeInstalledApp;
import com.google.api.client.extensions.jetty.auth.oauth2.LocalServerReceiver;
import com.google.api.client.googleapis.auth.oauth2.GoogleAuthorizationCodeFlow;
import com.google.api.client.googleapis.auth.oauth2.GoogleClientSecrets;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.JsonFactory;
import com.google.api.client.util.store.FileDataStoreFactory;
import com.google.auth.oauth2.GoogleCredentials;
import com.google.auth.oauth2.UserCredentials;
import com.google.common.collect.ImmutableList;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

public class AuthUserFlow {
  public static GoogleCredentials runAuthUserFlow(InputStream clientSecretsStream,
      JsonFactory jsonFactory, NetHttpTransport transport, ImmutableList<String> scopes)
      throws IOException {
    try (InputStreamReader reader =
             new InputStreamReader(clientSecretsStream, StandardCharsets.UTF_8)) {
      GoogleClientSecrets clientSecrets = GoogleClientSecrets.load(jsonFactory, reader);
      String clientId = clientSecrets.getDetails().getClientId();
      GoogleClientSecrets.Details details = clientSecrets.getDetails();
      return UserCredentials.newBuilder()
          .setClientId(details.getClientId())
          .setClientSecret(details.getClientSecret())
          .setRefreshToken(new AuthorizationCodeInstalledApp(
              new GoogleAuthorizationCodeFlow.Builder(transport, jsonFactory, clientSecrets, scopes)
                  .setDataStoreFactory(new FileDataStoreFactory(
                      new File(AuthUserFlow.class.getResource("/").getPath(), clientId)))
                  .setAccessType("offline")
                  .setApprovalPrompt("auto")
                  .build(),
              new LocalServerReceiver.Builder().setPort(61984).build())
                               .authorize("user")
                               .getRefreshToken())
          .build();
    }
  }
}