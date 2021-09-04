package net.jimblackler.collate;

import java.nio.file.FileSystems;
import java.nio.file.Path;

public class Config {
  private static final Path HOME =
      FileSystems.getDefault().getPath(System.getProperty("user.home"));
  private static final Path SDK_BASE = HOME.resolve("code/android-games-sdk");
  static final Path GRABBER_BASE = SDK_BASE.resolve("gamesdk/test/grabber");
  private static final Path GCLOUD_LOCATION = HOME.resolve("Library")
                                                  .resolve("Application Support")
                                                  .resolve("google-cloud-tools-java")
                                                  .resolve("managed-cloud-sdk")
                                                  .resolve("LATEST")
                                                  .resolve("google-cloud-sdk")
                                                  .resolve("bin");
  static final Path GCLOUD_EXECUTABLE = GCLOUD_LOCATION.resolve("gcloud");
}
