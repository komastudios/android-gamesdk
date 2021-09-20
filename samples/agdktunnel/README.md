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

## Building

Open the `samples/agdktunnel' directory in Android Studio 4.2 or higher.

## Android Performance Tuner API Key

The APT functionality requires a valid API key to operate. This is not
necessary to run the sample. For information on configure an API key
for APT, see the **Get an API key for the Android Performance Tuner**
section of the Codelab [Integrating Android Performance Tuner into your native Android game](https://developer.android.com/codelabs/android-performance-tuner-native#1).