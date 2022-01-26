# AGDKTunnel

A sample project demonstrating use of the Android Game Development Kit libraries.
AGDKTunnel is derived from the NDK sample Endless Tunnel.

AGDKTunnel uses the following AGDK libraries:

* Android Performance Tuner
* Frame Pacing
* GameActivity
* GameController
* GameTextInput
* Oboe

## Prerequisites

### Python

Python is expected to be available in your `PATH`. The `protobuf` package is
expected to have been installed via `pip`.

### GLM

The GLM library must be downloaded before building:

1. Open a shell and set the working directory to `samples/agdktunnel`
2. `cd third-party/glm`
3. `git clone https://github.com/g-truc/glm.git`

### Google Play Games for PC

We use build variants to control the platform where the the app will run.
If you are building this application to run in Google Play Games for PC you
need to follow the next steps.

1. Go to **Build > Select Build Variant** and select the `playGamesPC` [build variant](https://developer.android.com/studio/build/build-variants).
2. Create the `app/libs` directory and locate the AAR file corresponding to the Input SDK.

## Building

Open the `samples/agdktunnel' directory in Android Studio 4.2 or higher.

## Android Performance Tuner API Key

The APT functionality requires a valid API key to operate. This is not
necessary to run the sample. For information on configure an API key
for APT, see the **Get an API key for the Android Performance Tuner**
section of the Codelab [Integrating Android Performance Tuner into your native Android game](https://developer.android.com/codelabs/android-performance-tuner-native#1).