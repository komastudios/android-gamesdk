Android Game SDK

## Integrating the Game SDK in your game

Unless you need to compile the Game SDK from sources, it's recommended that you use the package with the pre-compiled library. You can download it on https://developer.android.com/games/sdk.

## Build the Game SDK

In order to build the Game SDK, this project must be initialized using the [*repo* tool](https://gerrit.googlesource.com/git-repo/). On [Windows](https://gerrit.googlesource.com/git-repo/+/HEAD/docs/windows.md), we recommend running all commands involving *repo* with Git Bash, to avoid issues with symlinks.

```bash
mkdir android-games-sdk
cd android-games-sdk
repo init -u https://android.googlesource.com/platform/manifest -b android-games-sdk
```

### Build with locally installed SDK/NDK

If the Android SDK is already installed locally, then download only the Game SDK source and build tools (~500Mb).

```bash
repo sync -c -j8 gamesdk
repo sync -c -j8 external/modp_b64 external/googletest external/nanopb-c external/protobuf
repo sync -c -j8 prebuilts/cmake/linux-x86 prebuilts/cmake/windows-x86 prebuilts/cmake/darwin-x86
```

Point the environment variable `ANDROID_HOME` to your local Android SDK (and `ANDROID_NDK`, if the ndk isn't in `ANDROID_HOME/ndk-bundle`).
Use the following gradle tasks to build the Game SDK with or without Tuning Fork:

```bash
cd gamesdk
# Build Swappy:
./gradlew packageLocalZip -Plibraries=swappy -PpackageName=local
# Build Swappy and Tuning Fork:
./gradlew packageLocalZip -Plibraries=swappy,tuningfork -PpackageName=localtf
```

### Build with specific prebuilt SDKs

Download the project along with specific versions of prebuilt Android SDK and NDK (~4GB).
First, download the core project and tools:

```bash
repo sync -c -j8 gamesdk
repo sync -c -j8 external/modp_b64 external/googletest external/nanopb-c external/protobuf
repo sync -c -j8 prebuilts/cmake/linux-x86 prebuilts/cmake/windows-x86 prebuilts/cmake/mac-x86
```

Next, use the download script to get prebuilt SDKs and/or NDKs.

```bash
cd gamesdk
./download.sh
```

Finally, build the Game SDK using downloaded prebuilts.

```bash
cd gamesdk
# Build and package Swappy in a ZIP file:
ANDROID_HOME=`pwd`/../prebuilts/sdk ANDROID_NDK=`pwd`/../prebuilts/ndk/r20 ./gradlew packageLocalZip -Plibraries=swappy -PpackageName=local
# Build and package Swappy and Tuning Fork in a ZIP file:
ANDROID_HOME=`pwd`/../prebuilts/sdk ANDROID_NDK=`pwd`/../prebuilts/ndk/r20 ./gradlew packageLocalZip -Plibraries=swappy,tuningfork -PpackageName=localtf
```

### Build with all prebuilt SDKs

Download the whole repository with all available prebuilt Android SDKs and NDKs (~23GB).

```bash
repo sync -c -j8
```

Build static and dynamic libraries for several SDK/NDK pairs.

```bash
cd gamesdk
ANDROID_HOME=`pwd`/../prebuilts/sdk ./gradlew packageZip -Plibraries=swappy,tuningfork
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
cd samples/tuningfork/insightsdemo && ./gradlew assemble
cd samples/tuningfork/experimentsdemo && ./gradlew assemble
```

The Android SDK/NDK exposed using environment variables (`ANDROID_HOME`) will be used for building both the sample project and the Game SDK.

### Using Android Studio

Open projects using Android Studio:

* `samples/bouncyball`
* `samples/cube`
* `samples/tuningfork/insightsdemo`
* `samples/tuningfork/experimentsdemo`

and run them directly (`Shift + F10` on Linux, `Control + R` on macOS). The local Android SDK/NDK (configured in Android Studio) will be used for building both the sample project and the Game SDK.

#### Development and debugging

After opening a sample project using Android Studio, uncomment the line containing `add_gamesdk_sources()`.
This will add the Swappy/Tuning Fork sources as part of the project. You can then inspect the source code (with working auto completions) and run the app in debug mode (with working breakpoints and inspectors).

