package net.jimblackler.collate;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.api.client.googleapis.auth.oauth2.GoogleAuthorizationCodeFlow;
import com.google.api.client.googleapis.auth.oauth2.GoogleAuthorizationCodeRequestUrl;
import com.google.api.client.googleapis.auth.oauth2.GoogleClientSecrets;
import com.google.api.client.googleapis.auth.oauth2.GoogleRefreshTokenRequest;
import com.google.api.client.googleapis.auth.oauth2.GoogleTokenResponse;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.jackson2.JacksonFactory;
import com.google.common.collect.ImmutableList;
import java.awt.Desktop;
import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.nio.file.NoSuchFileException;
import java.nio.file.Path;
import java.util.LinkedHashMap;
import java.util.Map;

class Auth {
  static String getAccessToken() throws IOException {
    while (true) {
      try {
        Map<String, Object> obj =
            new ObjectMapper().readValue(Path.of("creds.json").toFile(), Map.class);
        GoogleClientSecrets googleClientSecrets = getGoogleClientSecrets();
        GoogleTokenResponse response =
            new GoogleRefreshTokenRequest(new NetHttpTransport(), new JacksonFactory(),
                (String) obj.get("refreshToken"), googleClientSecrets.getInstalled().getClientId(),
                googleClientSecrets.getInstalled().getClientSecret())
                .execute();
        return response.getAccessToken();
      } catch (NoSuchFileException e) {
        // Expected.
      }
      getAccessTokenFromBrowser();
    }
  }

  private static void getAccessTokenFromBrowser() throws IOException {
    GoogleClientSecrets googleClientSecrets = getGoogleClientSecrets();

    GoogleAuthorizationCodeFlow flow =
        new GoogleAuthorizationCodeFlow
            .Builder(new NetHttpTransport(), new JacksonFactory(), googleClientSecrets,
                ImmutableList.of("https://www.googleapis.com/auth/devstorage.read_only",
                    "https://www.googleapis.com/auth/cloud-platform"))
            .build();

    String redirectUri = "urn:ietf:wg:oauth:2.0:oob";
    GoogleAuthorizationCodeRequestUrl authorizeUrl =
        flow.newAuthorizationUrl().setRedirectUri(redirectUri).setAccessType("offline");

    Desktop.getDesktop().browse(authorizeUrl.toURI());

    System.out.println("Enter the code:");
    String authorizationCode;

    try (BufferedReader bufferedReader =
             new BufferedReader(new InputStreamReader(System.in, StandardCharsets.UTF_8))) {
      authorizationCode = bufferedReader.readLine();
    }
    // Exchange for an access and refresh token.
    GoogleTokenResponse response =
        flow.newTokenRequest(authorizationCode).setRedirectUri(redirectUri).execute();

    if (response.getRefreshToken() == null) {
      System.out.println("To receive a new refresh token please revoke the previous refresh "
          + "token first: https://security.google.com/settings/u/0/security/permissions");
    }

    try (FileWriter file = new FileWriter("creds.json", StandardCharsets.UTF_8)) {
      Map<String, Object> obj = new LinkedHashMap<>();
      obj.put("accessToken", response.getAccessToken());
      obj.put("refreshToken", response.getRefreshToken());
      file.write(new ObjectMapper().writerWithDefaultPrettyPrinter().writeValueAsString(obj));
    }
  }

  private static GoogleClientSecrets getGoogleClientSecrets() throws IOException {
    return GoogleClientSecrets.load(JacksonFactory.getDefaultInstance(),
        new InputStreamReader(
            Main.class.getResourceAsStream("/clientSecrets.json"), StandardCharsets.UTF_8));
  }
}
