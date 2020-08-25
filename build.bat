set ANDROID_HOME=C:\win_build_extras\sdk

gradlew packageLocalZip -Plibraries=swappy,tuningfork -PincludeSampleArtifacts -Pndk=21.3.6528147
