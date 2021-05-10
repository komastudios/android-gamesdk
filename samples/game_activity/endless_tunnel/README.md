# Endless Tunnel with GameActivity sample

This is the [Endless Tunnel project from the NDK samples](https://github.com/android/ndk-samples/tree/master/endless-tunnel/) adapted to use GameActivity.

Note: this sample must be used with the GameActivity library built from sources.
We'll publish later prebuilt AARs of GameActivity.

## Run the sample after building Game Activity from sources

* First, build the GameActivity AAR: `cd GameActivity && ./build.sh`
* Open and run the sample project in Android Studio. The app is "Endless Tunnel" NDK sample with a few changes to use GameActivity and the events coming from it.

Note: the latest version of Prefab must be used to fix a problem in previous versions with headers only libraries. The version is specified in `gradle.properties` so you should not have any issue. Make sure to use a recent version of Android Studio (4.1+).