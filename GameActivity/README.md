# GameActivity

GameActivity is an Android `Activity` modeled on the API of `NativeActivity`, but with a few changes to make it a better starting point for game developers.

* It inherits from `androidx.appcompat.app.AppCompatActivity` - allowing you to use Jetpack components architecture (and still use some of the newer platform features on older Android devices).
* It renders into a `SurfaceView` that allows you to interface with any other Android UI element.
* It handles *events* like a Java activity would do, allowing any Android UI element (like a text edit, a webview, an ad or a form) to work as usual, but still exposes the events to your game using a C interface that makes them easy to consume in your game loop.
* It offers a C API similar to `NativeActivity`, and to the *android_native_app_glue* library.

## Build the GameActivity AAR from sources

Use the script to build the GameActivity AAR and inject the C++ source files.

* `cd GameActivity && ./build.sh`

> Note that this is a temporary workaround (in the future, running `./gradlew :GameActivity:assembleRelease` should be enough).

The AAR is outputed in `out/outputs/aar/GameActivity.aar` (`out` directory being where you cloned the Android Game SDK). It contains GameActivity, both the Java class and its C++ implementation, and the "android_native_app_glue" library.

## Integrate GameActivity AAR to your build

Refer to the integration guide for now. Link to it and other documentation will be added here later.
