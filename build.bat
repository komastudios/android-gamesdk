set ANDROID_HOME=C:\win_build_extras\sdk
set ANDROID_NDK_REVISION=21.3.6528147
set ANDROID_NDK=%ANDROID_HOME%\ndk\%ANDROID_NDK_REVISION%

gradlew packageLocalZip -Plibraries=swappy,tuningfork -PincludeSampleArtifacts

