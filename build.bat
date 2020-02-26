set ANDROID_HOME=%CD%\..\prebuilts\sdk
set ANDROID_NDK_HOME=%CD%\..\prebuilts\ndk\r20

if "%1"=="full" (set TARGET=fullSdkZip) else (set TARGET=gamesdkZip)
gradlew.bat %TARGET%
