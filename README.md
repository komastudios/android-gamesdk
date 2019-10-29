# Android Game SDK

## Build the Game SDK

In order to build the Game SDK, this project must be initialized from a custom repo.

```bash
mkdir android-games-sdk
cd android-games-sdk
repo init -u https://android.googlesource.com/platform/manifest -b android-games-sdk
```
### Build with locally installed SDK/NDK

If Android SDK is already installed locally, then download only the Game SDK source and build tools (<500Mb).

```bash
repo sync -c -j8 gamesdk 
repo sync -c -j8 prebuilts/cmake/linux-x86 external/modp_b64 external/googletest external/nanopb-c external/protobuf
```

Point environment variable `ANDROID_HOME` to your local Android SDK (and `ANDROID_NDK`, if the ndk isn't in `ANDROID_HOME/ndk-bundle`).
Use the following gradle tasks to build the Game SDK with or without Tuning Fork (default target is `archiveZip`).

```bash
./gradlew archiveZip # Without Tuning Fork
./gradlew archiveTfZip # With Tuning Fork
```

### Build with specific prebuilt SDKs

Download the project along with specific versions of prebuilt Android SDK and NDK (<4GB)

```bash
repo sync -c -j8 gamesdk 
repo sync -c -j8 prebuilts/cmake/linux-x86 external/modp_b64 external/googletest external/nanopb-c external/protobuf
repo sync -c -j8 prebuilts/ndk/r17 prebuilts/sdk/build-tools/28.0.3 prebuilts/sdk/platforms/android-28 
```
(This downloads NDK 17 and SDK 28, change the command as necessary.)

Use the following gradle tasks to build the Game SDK with or without Tuning Fork (default target is `archiveZip`).

```bash
cd gamesdk
ANDROID_HOME=../prebuilts/sdk ANDROID_NDK=../prebuilts/ndk/r17 ./gradlew archiveZip # Without Tuning Fork
ANDROID_HOME=../prebuilts/sdk ANDROID_NDK=../prebuilts/ndk/r17 ./gradlew archiveTfZip # With Tuning Fork
```

### Build with all prebuilt SDKs

Download the whole repository with prebuilt Android SDKs and NDKs (<23GB)

```bash
repo sync -c -j8  
```

Build static and dynamic libraries for several SDK/NDK pairs.

```bash
cd gamesdk
ANDROID_HOME=../prebuilts/sdk ./gradlew gamesdkZip
```

## Tests

```bash
./gradlew localUnitTests # Requires a connected ARM64 device to run
./gradlew localDeviceInfoUnitTests # No device required, tests are running on host
```

## Samples

Samples are classic Android projects, using CMake to build the native code. They are also all triggering the build of the Game SDK.

### Using Grade command line:

```bash
cd samples/bouncyball && ./gradlew assemble
cd samples/cube && ./gradlew assemble
cd samples/tuningfork/tftestapp && ./gradlew assemble
```

The Android SDK/NDK exposed using environment variables (`ANDROID_HOME`) will be used for building both the sample project and the Game SDK.

### Using Android Studio

Open projects using Android Studio:

* `samples/bouncyball`
* `samples/cube`
* `samples/tuningfork/tftestapp`

and run them directly (`Shift + F10` on Linux, `Control + R` on macOS). The local Android SDK/NDK (configured in Android Studio) will be used for building both the sample project and the Game SDK.

#### Development and debugging

After opening a sample project using Android Studio, uncomment the line containing `add_gamesdk_sources()`.
This will add the Swappy/Tuning Fork sources as part of the project. You can then inspect the source code (with working auto completions) and run the app in debug mode (with working breakpoints and inspectors).

