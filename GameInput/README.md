# Game Input library

Game Input is providing a simple API to show and hide the soft keyboard, set and get the currently editing text and receive notifications when the text is changed. It is not meant for fully-fledged text editor applications but still provides selection and composing region support for typical game use-case.

## Build the GameInput AAR from sources

Use the script to build the GameInput AAR and inject the C++ source files.

* `cd GameInput && ./build.sh`

> Note that this is a temporary workaround (in the future, running `./gradlew :GameInput:assembleRelease` should be enough).

The AAR is outputed in `out/outputs/aar/GameInput.aar` (`out` directory being where you cloned the Android Game SDK).

## Integrate GameInput AAR to your build

Refer to the integration guide for now. Link to it and other documentation will be added here later.
