package com.google.androidgamesdk;

/** JNI api for getting device information */
public class GameSdkDeviceInfoJni {
  private static Exception initializationException;

  static {
    try {
      System.loadLibrary("game_sdk_device_info_jni");
    } catch(Exception exception) {
      initializationException = exception;
    }
  }

  /**
   * Returns a byte array, which is a serialized proto containing device information, or
   * null if the native library "game_sdk_device_info_jni" could not be loaded.
   *
   * @return Optional with the serialized byte array, representing game sdk device info with errors,
   * or null.
   */
  public static byte[] tryGetProtoSerialized() {
    if (initializationException != null) {
      return null;
    }

    return getProtoSerialized();
  }


  /**
   * Returns the exception that was caught when trying to load the library, if any.
   * Otherwise, returns null.
   *
   * @return The caught Exception or null.
   */
  public static Exception getInitializationException() {
    return initializationException;
  }

  /**
   * Returns a byte array, which is a serialized proto.
   *
   * @return serialized byte array, representing game sdk device info with errors.
   */
  private static native byte[] getProtoSerialized();

  private GameSdkDeviceInfoJni() {}
}
