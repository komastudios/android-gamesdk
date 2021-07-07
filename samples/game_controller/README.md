# Game Controller library sample

This is a basic sample app that demonstrates use of the Game Controller
library. The sample:

* Displays graphical representation of control inputs from the active controller
* Displays information about connected controllers
* Provides an interface to test optional extended controller features,
such as vibration

There are two versions of the sample. One uses NativeActivity, the other
uses GameActivity.

## Prerequisites

Before building in Android Studio the following prerequisites must be
performed:

### AGDK library archives

Copy the relevant AGDK library .aar files to the libs/ folder of
the sample you wish to run.

For `nativeactivity/app/libs` you only need to copy the `games-controller` AAR file.
For `gameactivity/app/libs` you need to copy both the `games-controller` and the
`games-activity` AAR files.

**Note:** Verify that the filename of the .aar(s) in the
`nativeactivity/app/build.gradle` or `gameactivity/app/build.gradle`
dependencies section matches the filename of the .aar(s) you copied to `libs/`.
Fix the name in relevant `build.gradle` if they do not match.

### ImGui

1. Open a terminal and set the working directory to `samples/game_controller/`
2. `cd third-party`
3. `git clone https://github.com/ocornut/imgui`
4. `cd imgui`
5. `git checkout v1.80`

### libPNG

1. Download the source code to libPNG 1.6.37 from the [project site](http://www.libpng.org/pub/png/libpng.html)
2. Extract the `libpng-1.6.37` directory and copy it into the
`third-party/libpng` directory
3. Open a terminal and set the working directory to `samples/game_controller/`
4. `cd third-party/libpng`
5. `./build_all_android.sh libpng-1.6.37`

Note: The build script expects the `$ANDROID_NDK` environment variable to be
set to a path of a valid Android NDK install.

## Building

Once the prerequisites are complete, open the `nativeactivity` or
`gameactivity` directories in Android Studio 4.2 or higher. You can
then build and run the samples from Android Studio.

## License

Copyright 2021 The Android Open Source Project

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
