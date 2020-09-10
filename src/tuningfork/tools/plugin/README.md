# Android Studio Plugin for Android Performance Tuner

This plugin helps game developers integrate APT in their Android Studio project.

# Setup
Setting up the project requires changing the plugin local android studio build.
* Open up local.properties (If it doesn't exist create one)
* Add android.studio.dir
* Set android.studio.dir to local Android Studio Path

This is needed because the plugin needs C++ Idea Api which only exists in Android Studio
/ CLion platform

**Example**

android.studio.dir=/Applications/Android Studio.app/Contents

##### DON'T add local.properties to version control
