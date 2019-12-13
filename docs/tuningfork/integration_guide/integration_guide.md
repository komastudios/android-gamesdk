
# Tuning Fork Game Engine Integration Guide

Instructions on how to integrate Tuning Fork into a game or game engine running on Android.

| | |
|-|-|
| Author | willosborn@google.com |
| Contributors | idries-team@google.com |
| Intended audience | Android Game Engineers |
| Status | WIP |
| Created | 2019-12-05 |
| Self-link | [go/tuningfork-integration-guide](go/tuningfork-integration-guide) |

<!-- vscode-markdown-toc -->
* 1. [Introduction](#Introduction)
	* 1.1. [How does Tuning Fork record and upload data?](#HowdoesTuningForkrecordanduploaddata)
	* 1.2. [What is an Instrument Key?](#WhatisanInstrumentKey)
	* 1.3. [What are Annotations?](#WhatareAnnotations)
	* 1.4. [What are Fidelity Parameters?](#WhatareFidelityParameters)
	* 1.5. [What memory and CPU overhead is there?](#WhatmemoryandCPUoverheadisthere)
* 2. [Integrating the Games SDK into your build](#IntegratingtheGamesSDKintoyourbuild)
	* 2.1. [Swappy Integration](#SwappyIntegration)
* 3. [Integrating Tuning Fork into your code](#IntegratingTuningForkintoyourcode)
* 4. [Defining Annotations and Fidelity Parameters in dev_tuningfork.proto](#DefiningAnnotationsandFidelityParametersindev_tuningfork.proto)
* 5. [Defining Settings in tuningfork_settings.txt](#DefiningSettingsintuningfork_settings.txt)
	* 5.1. [Loading Annotations](#LoadingAnnotations)
* 6. [Defining Fidelity Parameter Sets / Quality Levels](#DefiningFidelityParameterSetsQualityLevels)
* 7. [Web Requests](#WebRequests)
	* 7.1. [What happens when the player is offline?](#Whathappenswhentheplayerisoffline)
* 8. [Packaging your APK](#PackagingyourAPK)
* 9. [Validation](#Validation)
* 10. [Protocol buffers](#Protocolbuffers)
	* 10.1. [Proto2 vs proto3](#Proto2vsproto3)
	* 10.2. [Text vs binary representations](#Textvsbinaryrepresentations)
	* 10.3. [Full vs Lite vs Nano](#FullvsLitevsNano)
* 11. [Tuning Fork API Overview](#TuningForkAPIOverview)
	* 11.1. [Lifecycle Functions](#LifecycleFunctions)
		* 11.1.1. [11.1.1 Expert usage](#Expertusage)
	* 11.2. [Per-Frame Functions](#Per-FrameFunctions)
	* 11.3. [Annotations](#Annotations)
	* 11.4. [Advanced Functions in tuningfork_extra.h](#AdvancedFunctionsintuningfork_extra.h)
		* 11.4.1. [Finding and loading files in your APK](#FindingandloadingfilesinyourAPK)
	* 11.5. [Download Fidelity Parameters on a separate thread](#DownloadFidelityParametersonaseparatethread)
	* 11.6. [Saving and deleting fidelity parameters stored on device](#Savinganddeletingfidelityparametersstoredondevice)
* 12. [Notes](#Notes)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

---

##  1. <a name='Introduction'></a>Introduction

Tuning Fork is a support library developed by the Android Platform team that records live frame timing information, aggregates it, and uploads it for viewing and analysis within the Play console.

Using this information gathered ‘in the wild’, you can see the performance of your game across the ecosystem of devices and see recommendations for tuning its quality.

Tuning Fork has two modes, 'scaled' and 'expert'. In scaled mode, you upload data to Play and the Play backend analyzes the data in order to produce recommendations for changes to the game's quality settings. In expert mode, you define experiments that are run across the population of devices and remotely configure the quality settings of your game. The bulk of this guide covers the 'scaled' version of Tuning Fork, and more advanced configuration is covered [elsewhere](#TODO).

###  1.1. <a name='HowdoesTuningForkrecordanduploaddata'></a>How does Tuning Fork record and upload data?

Tuning Fork relies on one of its [‘tick’](#LifecycleFunctions) functions being called each frame either by the [Android Frame Pacing Library](https://developer.android.com/games/sdk/frame-pacing) (also called Swappy) or by the game directly.
Within the library, this tick information is aggregated into histograms which are then periodically uploaded to Play through an HTTP endpoint.

Each tick is recorded as being associated with an 'instrument key' and an 'annotation', the definitions for which you specify in a protocol buffer file.

###  1.2. <a name='WhatisanInstrumentKey'></a>What is an Instrument Key?

An instrument key indicates where in the frame the tick comes from, e.g. frame start, GPU fence, or a trace for a particular function call. The instrument key is an integer that must be passed to each tick function call. The game must specify the maximum number of valid instrument keys at initialization. Keys 64000 and higher are reserved for Google-defined ticks.

###  1.3. <a name='WhatareAnnotations'></a>What are Annotations?

You define annotations in order to give contextual information about what your game is doing when a tick is recorded, e.g. which level is current, whether a scene is loading, whether a boss is on screen or any other relevant state. An annotation can be specified by defining the `com.google.tuningfork.Annotation` protocol buffer message; all fields are restricted to enums. In order to set the current annotation, you pass a serialization of the message you have defined to `TuningFork_setCurrentAnnotation`. All subsequent tick data is associated with this annotation until another annotation is set.

An example annotation proto definition:

```
import "tuningfork.proto"
enum LoadingState {
  INVALID_LS = 0;
  LOADING = 1;
  NOT_LOADING = 2;
}
enum Level {
  INVALID_LEVEL = 0;
  Level_1 = 1;
  Level_2 = 2;
  Level_3 = 3;
}
message Annotation {
  optional LoadingState loading_state = 1;
  optional Level level = 2;
}
```

###  1.4. <a name='WhatareFidelityParameters'></a>What are Fidelity Parameters?

Fidelity Parameters are parameters you define that influence the performance and graphical fidelity of your game, such as mesh level-of-detail, texture resolution and anti-aliasing method. Each parameter must be of type `enum`, `float`, or `int32`. Like annotations, fidelity parameters are defined as a protocol buffer message, this time called `com.google.tuningfork.FidelityParams`. At Tuning Fork initialization, you pass a serialization of the parameters the game will be using and you can change these parameters if, for instance, the user changes the game rendering settings.

In order for Play to understand the Annotations and Fidelity Parameters that you define, the proto file holding their definitions must be bundled within the game’s APK, together with initialization settings, as detailed [below](#DefiningAnnotationsandFidelityParametersindev_tuningfork.proto).


###  1.5. <a name='WhatmemoryandCPUoverheadisthere'></a>What memory and CPU overhead is there?

All memory that Tuning Fork uses is allocated up-front at initialization in order to avoid surprises during gameplay. The size of the data depends on the number of instrument keys, number of possible annotations, and number of buckets in each histogram: it is a multiple of all these times 4 bytes for each bucket. There are also two copies of all histograms in order to allow for submission in a double-buffered fashion.

Submission occurs on a separate thread and will not block tick calls. If no upload connection is available, the submission is queued for later upload.

There is little processing overhead to calling a 'tick' function: they simply calculate an index into the array of histogram buckets and increment an integer count.

---

##  2. <a name='IntegratingtheGamesSDKintoyourbuild'></a>Integrating the Games SDK into your build

![alt_text](images/Overview.png "image_tooltip")

Tuning Fork is packaged as part of the Android Games SDK, which is available as a binary release of shared libraries, static libraries, headers and samples on [https://developer.android.com (DAC)](https://developer.android.com).
The entire source is also available via the Android Open Source project [[gitiles](https://android.googlesource.com/platform/frameworks/opt/gamesdk/+/refs/heads/master/README.md)].

It is recommended that you start by looking at and building the tftestapp sample. This uses CMake for native code building and shows how to use the gamesdk.cmake file, which is the easiest way to get started.
If you are not using CMake, you will need to:

*   Add &lt;gamesdk&gt;/include to your compiler include paths.
*   Add &lt;gamesdk&gt;/libs/&lt;build_version&gt; to your linker library path, where build_version is an concatenation of ABI, SDK version, NDK version, STL and build variant, e.g. arm64-v8a_API21_NDK19_cpp_static_Release. Look in &lt;gamesdk&gt;/libs and pick the one that best suits your toolchain or choose the latest.
*   Add -lgamesdk to your linker command line.
*   Add internet permissions to your AndroidManifest.xml file with the following line at the top level : \
  `  <uses-permission android:name="android.permission.INTERNET" />`

###  2.1. <a name='SwappyIntegration'></a>Swappy Integration

It is recommended that you integrate the [Swappy frame pacing library](https://developer.android.com/games/sdk/frame-pacing) before integrating Tuning Fork.
If you do this and pass in the Swappy_injectTracers function at Tuning Fork initialization, Tuning Fork can automatically record frame timing without you explicitly calling the tick functions yourself.

Tracers with the following instrument keys are added:

*   Frame start.
*   GPU fence (previous frame consumed by the display pipeline).
*   Trace of the time Swappy waited until the GPU fence.
*   Trace of the time taken by eglSwapBuffers.

---

##  3. <a name='IntegratingTuningForkintoyourcode'></a>Integrating Tuning Fork into your code

The core interface for Tuning Fork can be found in `include/tuningfork/tuningfork.h`. There are also utility functions in `include/tuningfork/tuningfork_extra.h`.

Several functions take serializations of protocol buffers, which are Google’s language-neutral, structured, data-interchange format. For more on using protocol buffers within your game, see [below](#Protocolbuffers).

Function parameters and return values are explained in the headers and reference API documentation.

---

##  4. <a name='DefiningAnnotationsandFidelityParametersindev_tuningfork.proto'></a>Defining Annotations and Fidelity Parameters in dev_tuningfork.proto

The possible annotations and fidelity parameters for your game must be defined in a file called dev_tuningfork.proto placed in the assets/tuningfork directory of your project. An example from the 'tftestapp' sample is:

```
syntax = "proto3";

package com.google.tuningfork;

enum LoadingState {
  LOADING_INVALID = 0;
  NOT_LOADING = 1;
  LOADING = 2;
}

enum Level {
  // 0 is not a valid value
  LEVEL_INVALID = 0;
  LEVEL_1 = 1;
  LEVEL_2 = 2;
  LEVEL_3 = 3;
};

message Annotation {
  LoadingState loading = 1;
  Level level = 2;
}

message FidelityParams {
  int32 num_spheres = 1;
  float tesselation_percent = 2;
}
```
To note:
* The package must be com.google.tuningfork.
* The message names must be exactly _Annotation_ and _FidelityParams_.
* Only enums defined in this file can be used as part of annotations.
* FidelityParams fields can only be enums, int32s or floats.
* The [validation tool](#Validation) enforces these conventions.

##  5. <a name='DefiningSettingsintuningfork_settings.txt'></a>Defining Settings in tuningfork_settings.txt

The 'Settings' message is defined in tuningfork.proto and a full example is given in the tftestapp sample. The only fields you need to enter
for Tuning Fork Scaled are:

 * api_key - your app's Cloud API key, used to validate requests to the endpoint. If you see connection errors in logcat, please check that the API key is correct *and that you have been whitelisted by Play TODO: check this is needed for Scaled*
 * default_fidelity_parameters_filename - the fidelity parameter set used at initialization.
 * loading_annotation_index - the index in your annotation fields of the loading/no-loading flag (optional).
 * level_annotation_index - the index in your annotation fields of the level number (optional).

An example text representation is:

```
api_key: "get-me-from-play"
default_fidelity_parameters_filename: "dev_tuningfork_fidelityparams_3.bin"
loading_annotation_index: 1
level_annotation_index: 2
```

###  5.1. <a name='LoadingAnnotations'></a>Loading Annotations

If you set the _loading_annotation_index_ field in your settings, a special meaning is given to the
annotation with that index.
While the loading annotation is set to _LOADING_, frame ticks will be ignored. When the loading annotation is reset to _NOT_LOADING_ the time spent in _LOADING_ state is recorded in the histogram for that annotation.

In the above settings, since _loading_annotation_index_ is 1, the first annotation is taken to mean whether the game is loading the next scene or not.
Since _level_annotation_index_ is 2, the second annotation is taken to be the level number.

---

##  6. <a name='DefiningFidelityParameterSetsQualityLevels'></a>Defining Fidelity Parameter Sets / Quality Levels

You must define at least one and preferably several fidelity parameter sets for your game.
These must be given in increasing fidelity order, named so: dev_tuningfork_fidelityparams_<i>.txt where i is an index starting at 1.

The tftestapp sample shows an example of this.

##  7. <a name='WebRequests'></a>Web Requests

Tuning Fork makes three kinds of requests to the server endpoint:

1. At initialization a 'generateTuningParameters' request is made.
2. During gameplay, an 'uploadTelemetry' request is periodically made to send data to the server.
3. Debug APKs can also perform 'debugInfo' requests, which will inform a debug server of the settings, default fidelity parameters and dev_tuningfork.proto structure.

###  7.1. <a name='Whathappenswhentheplayerisoffline'></a>What happens when the player is offline?

If there is no available connection at initialization, the request is retried several times with increasing back-off time.

If there is no connection at upload, the upload is cached. You can provide your own caching mechanism by passing a TFCache object at initialization. If you do not provide your own cache, uploads are stored as files in temporary storage.

##  8. <a name='PackagingyourAPK'></a>Packaging your APK

Your dev_tuningfork.proto file, tuningfork_settings.txt file and dev_tuningfork_fidelityparams.txt files should be placed in your APK assets folder under assets/tuningfork.

The latter are fidelity parameter sets that you can choose between in expert mode within the Play store interface.

---

##  9. <a name='Validation'></a>Validation

The validation tool is placed in the game sdk under **src/tuningfork/tools/validation.**

All details including how to run it are in the corresponding **README** file.

The validation tool will generate binary serialized protocol buffers of the settings and default fidelity parameters files as *.bin files in your assets/tuningfork directory.

---

##  10. <a name='Protocolbuffers'></a>Protocol buffers

Tuning Fork uses Google’s protocol buffer format for settings, annotations and fidelity parameters. This is a well-defined, multi-language protocol for extensible, structured data https://developers.google.com/protocol-buffers/ but it does have some idiosyncrasies and things of which to be aware.


###  10.1. <a name='Proto2vsproto3'></a>Proto2 vs proto3

The version of the protocol buffer format is set in the first line of the file, e.g.


```
syntax="proto2";
```


These two commonly-used versions of protocol buffers use the same wire format but the definition files are not compatible. The main differences are that the optional and required keywords are no longer allowed in proto3: everything is effectively optional. Also, extensions are not supported.

We recommend using proto3 in your proto files, since these can be compiled to C#, although proto2 will work just as well with the limited feature set we use in Tuning Fork.


###  10.2. <a name='Textvsbinaryrepresentations'></a>Text vs binary representations

The binary protobuf wire-format is well-defined and stable across different protobuf versions (the generated code is not). There is also a text format that the full version of the protobuf library can generate and read. This format is not so well defined, but it is stable for the limited set of features we use. You can convert between binary and text formats using the protoc compiler, e.g. the following will convert from text to binary:


```
protoc --encode com.google.tuningfork.Settings tuningfork.proto < tuningfork_settings.txt > tuningfork_settings.bin
```


We require that you include binary files rather than text files in your APK because the full protobuf library is several MB in size and making Tuning Fork depend on it would increase the size of your game by a similar amount.


###  10.3. <a name='FullvsLitevsNano'></a>Full vs Lite vs Nano

As well as the full protobuf library, there is a ‘lite’ version that reduces code footprint by removing some features such as reflection, FileDescriptors and streaming to/from text formats. We found that this version still requires several MB extra code footprint and so instead internally we use the nanopb library (https://github.com/nanopb/nanopb). The source code for this library is included in AOSP in external/nanopb-c and it is part of the gamesdk branch. We recommend using this in your game if code size is an issue.

There are CMake files in **gamesdk/src/protobuf** that can help you in integrating all 3 versions of protobuf and the samples include examples of using both nanopb and full protobuf.

---

##  11. <a name='TuningForkAPIOverview'></a>Tuning Fork API Overview

###  11.1. <a name='LifecycleFunctions'></a>Lifecycle Functions

```
TFErrorCode TuningFork_init(const TFSettings *settings, JNIEnv* env, jobject context);
```

This must be called once at start-up, typically from within Java-based code executed by the app’s `onCreate()` method. It allocates the data needed by Tuning Fork.

You must have a tuningfork_settings.bin file present in assets/tuningfork within your app, which contains the histogram and annotation settings.

The fields you fill in _settings_ determine how TuningFork initializes itself.

```
/**
 * @brief Initialization settings
 *   Zero any values that are not being used.
 */
struct TFSettings {
  /**
   * Cache object to be used for upload data persistence.
   * If unset, data is persisted to /data/local/tmp/tuningfork
   */
  const TFCache* persistent_cache;
  /**
   * The address of the Swappy_injectTracers function.
   * If this is unset, you need to call TuningFork_tick yourself.
   * If it is set, telemetry for 4 instrument keys is automatically recorded.
   */
  SwappyTracerFn swappy_tracer_fn;
  /**
   * Callback
   * If set, this is called with the fidelity parameters that are downloaded.
   * If unset, you need to call TuningFork_getFidelityParameters yourself.
   */
  ProtoCallback fidelity_params_callback;
  /**
   * A serialized protobuf containing the fidelity parameters to be uploaded
   *  for training.
   * Set this to nullptr if you are not using training mode. Note that these
   *  are used instead of the default parameters loaded from the APK, if they
   *  are present and there are neither a successful download nor saved parameters.
   */
  const CProtobufSerialization* training_fidelity_params;
};
```

```
TFErrorCode TuningFork_destroy();
```

This can optionally be called at shut-down and will attempt to submit all currently stored histogram data for later upload before deallocating any memory used by tuning fork.


```
TFErrorCode TuningFork_flush();
```

This can optionally be called to flush the recorded histograms, e.g. when the game is backgrounded or foregrounded. It will not flush data if the minimum upload period (default 1 minute) has not elapsed since the previous upload.

```
TFErrorCode TuningFork_setFidelityParameters(const CProtobufSerialization *defaultParams);
```

This function can be used to override the current fidelity parameters with which frame data is associated. You should call it when a player manually changes the quality settings of the game.


####  11.1.1. <a name='Expertusage'></a>11.1.1 Expert usage

```
TFErrorCode TuningFork_getFidelityParameters(const CProtobufSerialization *defaultParams, CProtobufSerialization *returnedParams, uint32_t timeout_ms);
```

If you pass _fidelity_params_callback_ to `TuningFork_init` in _settings_ or you are in 'scaled' mode, you do not need to call this function.

This function contacts a server to retrieve fidelity parameters. It will block until one of the following occurs:

*   Fidelity parameters are retrieved, when the return value will be **TFERROR_OK **and _returnedParams_ will hold the parameters. In this case, all subsequent tick data is associated with _returnedParams_
*   A number of milliseconds equal to _timeout_ms_ passes, when the return value will be **TFERROR_TIMEOUT**. In this case, all subsequent tick data is associated with _defaultFidelityParams._

`TuningFork_init` must have been called before this function and it must be called on a separate thread to the main thread (see **TuningFork_startFidelityParamDownloadThread** below for a utility function to do this for you).
It can also be called again, for example at level-loading time, in order to re-get fidelity parameters from the server.
This allows you to dynamically update parameters rather than having to reload them only at start-up.
If new fidelity parameters are downloaded or a new default is used, all previous tick data will be submitted.


###  11.2. <a name='Per-FrameFunctions'></a>Per-Frame Functions


```
TFErrorCode TuningFork_frameTick(int instrument_key);
```


This function will record the time between the previous tick with the given _instrument_key_ and the current time[^1] in the histogram associated with _instrument_key_ and the current annotation.


```
TFErrorCode TuningFork_frameDeltaTimeNanos(int instrument_key, uint64_t duration);
```


This function will record _duration_ in the histogram associated with _instrument_key_ and the current annotation.


```
TFErrorCode TuningFork_startTrace(int instrument_key, TraceHandle* handle);
```


This function will set _handle_ to a trace handle associated with the given _instrument_key._


```
TFErrorCode TuningFork_endTrace(TraceHandle);
```


This function will record the time interval since the associated _TuningFork_startTrace_ call in the histogram associated with the _instrument_key_ that was used and the current annotation.


###  11.3. <a name='Annotations'></a>Annotations


```
TFErrorCode TuningFork_setCurrentAnnotation(const CProtobufSerialization* annotation);
```


This function will set the annotation to associate with subsequent ticks. It returns **TFERROR_INVALID_ANNOTATION** if there was an error decoding the annotation and **TFERROR_OK** if there was no error.


###  11.4. <a name='AdvancedFunctionsintuningfork_extra.h'></a>Advanced Functions in tuningfork_extra.h

####  11.4.1. <a name='FindingandloadingfilesinyourAPK'></a>Finding and loading files in your APK

```
TFErrorCode TuningFork_findFidelityParamsInAPK(JNIEnv* env,
    jobject context, const char* filename,
    CProtobufSerialization* fidelityParams);
```


This function loads _fidelityParams_ from the **assets/tuningfork** directory in the APK with the given filename, which must be a serialization of a FidelityParams message. The functions should be named as detailed above: **dev_tuningfork_fidelityparams_.{1,15}.bin**.

Ownership of the serialization is passed to the caller who must call **CProtobufSerialization_Free** to deallocate any memory held.


###  11.5. <a name='DownloadFidelityParametersonaseparatethread'></a>Download Fidelity Parameters on a separate thread

```
void TuningFork_startFidelityParamDownloadThread(
          const CProtobufSerialization* defaultParams,
          ProtoCallback fidelity_params_callback,
          int initialTimeoutMs, int ultimateTimeoutMs);
```


This function will call **TuningFork_getFidelityParams** on a separate thread and, if no parameters could be downloaded, it will back off and try again until _ultimateTimeoutMs_ has passed. _fidelity_params_callback _will be called after the first call to **TuningFork_getFidelityParams**, whether parameters could be downloaded or not, and then upon a successful download, if the first download was unsuccessful.


###  11.6. <a name='Savinganddeletingfidelityparametersstoredondevice'></a>Saving and deleting fidelity parameters stored on device

```
TFErrorCode TuningFork_saveOrDeleteFidelityParamsFile(JNIEnv* env, jobject context,
                        const CProtobufSerialization* fidelity_params);
```

This function is only needed in Expert mode where fidelity parameters are downloaded from a server. It either saves over or deletes (if fidelity_params is null) the locally stored files that are used when the server cannot be connected to.

---

<!-- Footnotes themselves at the bottom. -->
##  12. <a name='Notes'></a>Notes

[^1]:
     Using std::chrono::steady_clock::now(), internally
