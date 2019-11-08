# AndroidCertTest

---

## Installation
Build the gamesdk `archiveZip` target:
```
$ cd ../../gamesdk
$ ./gradlew archiveZip
```

Vulkan requires that `Shaderc` (an NDK dependency) is built:
```
$ cd ${ANDROID_NDK}/sources/third_party/shaderc
$ ../../../ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk APP_STL:=c++_static APP_ABI=armeabi-v7a libshaderc_combined -j16
$ ../../../ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk APP_STL:=c++_static APP_ABI=arm64-v8a libshaderc_combined -j16
```

Run the `install_deps.sh` script to install `glm` and `nlohman::json`
```
$ cd <GAMESDK>/cert/test
$ ./install_deps.sh
```
