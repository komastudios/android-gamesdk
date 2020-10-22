# Android Performance Tuner Plugin For Android Studio

This plugin helps game developers integrate APT in their Android Studio projects.
 The plugin also provide debugging features for TuningFork by allowing the user to
 intercept all the outgoing HTTP requests and monitor them on the plugin such as.

 * Monitoring Renderer Time Histograms
 * Debug Information sent from APK
 * Downloading Fidelity From Server.

## Setup
Setting up the project requires changing the plugin local android studio build.
* Clone the repo.
* Open up the project using intelliJ(Community or Ultimate).
* Open up local.properties (If it doesn't exist create one).
* Add android.studio.dir.
* Set android.studio.dir to local Android Studio Path.

This is needed because the plugin needs C++ Idea Api which only exists in Android Studio
/ CLion platform

**Example**

android.studio.dir=/Applications/Android Studio.app/Contents

##### DON'T add local.properties to version control

## Launching
* Complete all the steps in the 'Setup' section.
* Open the list of gradle tasks.
* Execute 'runIde' task which is responsible for running the plugin in testing environment.
* The plugin will be run using the local android studio version specified in the 'Setup' procedure.

## Supported Features

* Allows adding/removing/editing Enums and their options.
* Allows adding/removing/editing Annotation fields.
* Allows adding/removing/editing Fidelity fields(Int/Float/Enum).
* Allows adding/removing/editing Quality levels with different parameters.
* Contains local debugging tool and monitoring tab to analyse render time histograms
* Shows the debug information sent from TuningFork as human-readable text.
* Enables the user to change 'Fidelity' settings in APK mid-game from the plugin directly.
* Has warnings for any unrecommended settings. Such as:
    * Too small upload interval which might lead to bandwidth abuse.
    * Histograms not covering the [30,60] FPS range.

## Monitoring Tab

The monitoring tab heavily depends on the Histogram tree. Once a request has been received.
It's converted to a tree of nodes. For example, Let's say we got a request with the following
features.

Phone Model: PM

Telemetries:
* Telemetry 1
    * Annotation 1
    * Fidelity 1
    * ID1 -> Values
    * ID2 -> Values
* Telemetry 2
    * Annotation 1
    * Fidelity 2
    * ID1 -> Values

This is converted into nodes with the following hierarchy.

__Only leaf node has values__
 ```
  Phone Node(Name: PM, Tooltip: Device information)
  |- Annotation 1(Tooltip: annotation as text)
  |  |- Fidelity 1(Tooltip: fidelity as text)
  |  |  |- ID1(Has no tooltip, but has value which is the list of integers)
  |  |  |- ID2(Has no tooltip, but has value which is the list of integers)
  |- Annotation 1(Tooltip: annotation as text)
  |  |- Fidelity 2(Tooltip: fidelity as text)
  |     |- ID1(has no tooltip, but has value which is the list of integers)
  ```
After the nodes have been constructed in this shape. It's added to the histogram tree.

__Histogram Tree Doesn't Allow Duplicate Nodes__.

So If there's duplicate nodes, They'll be merged together. So the final result in the Histogram tree
would be like this.
```
  Invisible Root
  |- Phone Node(Same)
  |  |- Annotation 1(Same tooltip)
  |  |  |- Fidelity 1(Same tooltip)
  |  |  |  |- ID1
  |  |  |  |- ID2
  |  |  |- Fidelity 2(Same tooltip)
  |  |  |  |- ID1
```
If the plugin received __Phone Node->Annotation1->Fidelity1->ID1__, Then It'd be merged with the
the already existing node in the histogram by adding both values together.

Once the histogram tree has been constructed. It's used to render "Data Collected" Tree.
and then all filters will be rechecked and updated with the new chart panels according to the new
tree values.