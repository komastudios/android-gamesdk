# About the library

The Android Memory Assistance API is an experimental library to help
applications avoid exceeding safe limits of memory use on devices. It can be
considered an alternative to using
[onTrimMemory](https://developer.android.com/reference/android/content/ComponentCallbacks2#onTrimMemory\(int\))
events to manage memory limits.

It provides an estimate of the percentage of potential total memory resources
used at that moment.

# Specifics and limitations

The library is compatible with API level 19 (Android 4.4 / KitKat) and higher.
The library gathers metrics from a few different sources but has no special
access to the Android OS. These sources already available to apps, but not all
will be familiar to developers. The library is standalone with no dependencies
beyond the Android SDK

The metric sources available are currently:

*   The file `/proc/meminfo` for values such as `Active`, `Active(anon)`.
    `Active(file)`, `AnonPages`, `MemAvailable`, `MemFree`, `VmData`, `VmRSS`,
    `CommitLimit`, `HighTotal`, `LowTotal` and `MemTotal`.

*   The file `/proc/(pid)/status` for values such as `VmRSS` and `VmSize`.

*   `ActivityManager.getMemoryInfo()` for values such as `totalMem`,
    `threshold`, `availMem` and `lowmemory`.

*   The file `/proc/(pid)/oom_score`

*   `Debug.getNativeHeapAllocatedSize()`

*   `ActivityManager.getProcessMemoryInfo()`. It is not recommended for
    real-time use due to the high cost of calls and being rate throttled to one
    call per five minutes on newer devices.

These signals are combined with a machine learning model trained from real user
devices in order to generate the usage estimate.

The library is only intended for use while applications are running in the
foreground (currently operated by users). (In the event that the application is
put into the background, this will be reported in the `MemoryState`.)

"Memory" means specifically native heap memory allocated by malloc, and graphics
memory allocated by the OpenGL ES and Vulkan Graphics APIs. (Note: Memory is
only one limit affecting graphics use; for example, the number of active GL
allocations is limited to the value `vm.max_map_count` (65536 on most devices).
However, memory is the only resource that this library attempts to track.)

If the preferred metrics or suitable lab readings are not available on any
device for some reason, then alternatives will be used instead.

The advice is given on a best effort basis without guarantees. The methods used
to generate this advice are quite simple. The library was built having studied
all available signals to determine which ones act as reliable predictors of
memory overload. The value added by the library is that these methods have been
found in lab tests to be effective,

The library is experimental until the assumption is proven that devices will
behave similarly to other devices with the same hardware and version of Android.

The API is intended for use by native applications (i.e., applications that are
written primarily in C/C++) but is itself written in Java. (As long as Android
platform calls are needed, such as those based on ActivityManager, some Java
components will be required in the library.)

Currently the API will not work effectively for applications that run in 32 bit
mode on devices that have 64 bit mode available (in other words where 32 bit
mode is "forced"). This is because the stress tests that calibrate the library
are run in 64 bit mode wherever available.

# API capabilities

The API can be called at any time to discover:

*   A memory warning signal that indicates whether significant allocation should
    stop ("yellow"), or memory should be freed ("red").

*   A collection of raw memory metrics that can be captured for diagnostic
    purposes.

The library offers an optional class `MemoryWatcher` that can perform the task
of polling the `MemoryAdvisor` on behalf of the application; calling back the
application when the advice state changes at a rate selected by the application.

Otherwise, the choice of polling rate is left to the developer, to strike the
correct balance between calling cost (this varies significantly by device but
the ballpark is between 1 and 3 ms per call) and the rate of memory allocation
performed by the app (higher rate allows a more timely reaction to warnings ).
The API does not cache or rate limit, nor does it use a timer or other thread
mechanism.

All this information is provided as a Java map to allow easy integration across
language barriers, C++, Java, and in the case of Unity projects, C# and to allow
capture of all data using telemetry for diagnostic purposes without devising a
separate schema for the telemetry.

The library provides methods that can be called to get simple recommendations
from the map returned. However, as long as the library is experimental, this
interface is not guaranteed to be stable.

# Recommended strategies

The library will only be of use if client applications can vary the amount of
memory used. The variation could take the form of model detail, texture detail,
optional enhancements such as particle effects or shadows. Audio assets could be
reduced in fidelity, or dropped entirely.

Games must be able to vary memory used at startup, at runtime, or both.

## Startup strategies

If the application has a range of increasing quality assets and can estimate the
memory footprint of each option, it can query the library for the estimated
amount of memory remaining and select the best quality options that fit. Models
of increasing detail could be loaded until the yellow signal was received. At
this point, the application could stop increasing the quality of assets to avoid
making further significant allocations.

## Runtime strategies

Games could react to the "red" signal by unloading assets such as audio,
particle effects, or shadows, reducing screen resolution, or reducing texture
resolution. These can be restored when no more memory warnings (including
"yellow" signal) are received.

# Limitations of estimates

Specific memory quantity estimates are currently a highly experimental part of
the library, as estimates may be inaccurate for combinations of devices that we
have not seen in lab testing.

They may suggest a lower limit than can be allocated by an application in
practice, because:

*   Different types of memory allocation (e.g. heap allocation vs allocation via
    graphics API) experience different limits, MB for MB. The library is
    pessimistic so will report the lower figure.

*   The memory advice library is pessimistic about the effects of zram
    compression on memory availability. Allocated memory that is both rarely
    used, and has compressible contents (e.g. contains repeated data) can be
    compressed by [zram](https://en.wikipedia.org/wiki/Zram) on Android. In
    extreme cases this could even result in apps apparently allocating more
    memory that was actually present on the device.

The estimate is of remaining memory to allocate. This may change over time
affected by other activity on the device. After memory has been allocated by the
app, further calls may receive more accurate results than calls made at the
start of the app's lifetime.

# API specifics

## Adding the library to an Android project

The library is published on
[Google's Maven repository](https://maven.google.com/web/index.html?q=com.google.android.games#com.google.android.games:memory-advice:0.24).

In the application root `build.gradle` file, ensure `google()` is specified as a
repository for the project, as well as `jitpack.io` for some of its
dependencies. For example:

```gradle
allprojects {
    repositories {
        google()
        mavenCentral()
        maven {
            url 'https://jitpack.io'
        }
    }
}
```

In the application module's `build.gradle` file, add an `implementation` line to
the `dependencies` section:

```gradle
dependencies {
    // ..
    implementation 'com.google.android.games:memory-advice:0.24'

}
```

Since the library has *AndroidX* dependencies, it is necessary to enable
*AndroidX* in the application, if it is not already. Instructions can be
[found here](https://developer.android.com/jetpack/androidx/migrate).

### Building the library from source

If you prefer to build your own version of the library, get the
[repo tool](https://gerrit.googlesource.com/git-repo/) and sync the Games SDK
project
[games-sdk project](https://android.googlesource.com/platform/frameworks/opt/gamesdk/+/refs/heads/master);

```bash
repo init -u https://android.googlesource.com/platform/manifest -b my-branch
```

Built with `gradle publishToMavenLocal` and add `mavenLocal()` to `repositories`
in the `build.gradle` of clients.

### Code

In your main activity, initialize a single memoryAdviser object for the lifetime
of your app.

```java
import android.app.Activity;
import android.os.Bundle;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;

class MyActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    // ...
    memoryAdvisor = new MemoryAdvisor(this);
  }
}
```

At some frequency, call the memory advisor to get recommendations. The choice of
frequency should reflect the cost of calling `getAdvice()`; approximately 5 to
20 milliseconds per call.

```java
import android.app.Activity;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;

class MyActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;
  // ...
  void myMethod() {
    Map<String, Object> advice = memoryAdvisor.getAdvice();
    // ...
  }
}
```

One option is to call the library back with the object to interpret the
recommendations.

```java
import android.app.Activity;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;

class MyActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;
  // ...
  void myMethod() {
    Map<String, Object> advice = memoryAdvisor.getAdvice();
    MemoryAdvisor.MemoryState memoryState = MemoryAdvisor.getMemoryState(advice);
    switch (memoryState) {
      case OK:
        // The application can safely allocate significant memory.
        break;
      case APPROACHING_LIMIT:
        // The application should not allocate significant memory.
        break;
      case CRITICAL:
        // The application should free memory as soon as possible, until the memory state changes.
        break;
      case BACKGROUNDED:
        // The application has been put into the background.
        break;
    }
  }
}
```

Another options is to initialize a `MemoryWatcher` object. This will call back
the application when the memory warning changes. In this example the fixed
sample rate is 1/4 second, or 250 milliseconds.

```java
import android.app.Activity;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;
import com.google.android.apps.internal.games.memoryadvice.MemoryWatcher;
import java.util.Map;

class MyActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;
  // ...
  void myMethod() {
    Map<String, Object> advice = memoryAdvisor.getAdvice();
    MemoryWatcher memoryWatcher =
        new MemoryWatcher(memoryAdvisor, 250, new MemoryWatcher.DefaultClient() {
          @Override
          public void newState(MemoryAdvisor.MemoryState memoryState) {
            switch (memoryState) {
              case OK:
                // The application can safely allocate significant memory.
                break;
              case APPROACHING_LIMIT:
                // The application should not allocate significant memory.
                break;
              case CRITICAL:
                // The application should free memory as soon as possible, until the memory state
                // changes.
                break;
              case BACKGROUNDED:
                // The application has been put into the background.
                break;
            }
          }
        });
  }
}
```

Call the library back with the object to get an estimate in bytes of the memory
that can safely be allocated by the application.

```java
import android.app.Activity;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;

class MainActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;
  // ...
  void myMethod() {
    Map<String, Object> advice = memoryAdvisor.getAdvice();
    long availabilityEstimate = MemoryAdvisor.availabilityEstimate(advice);
    // ...
  }
}
```

## Telemetry / debugging info

Logging and debugging telemetry can be obtained from the advisor object.
Information about the device itself:

```java
import android.app.Activity;
import android.os.Bundle;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;

class MyActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    // ...
    memoryAdvisor = new MemoryAdvisor(this);
    Map<String, Object> deviceInfo = memoryAdvisor.getDeviceInfo(MainActivity.this);
    // Now convert the deviceInfo to a string and log it.
  }
}
```

The data returned by `getAdvice()`:

```java
import android.app.Activity;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;

class MyActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;
  // ...
  void myMethod() {
    Map<String, Object> advice = memoryAdvisor.getAdvice();
    // Now convert the advice object to a string and log it.
  }
}
```

## Unity integration

1.  In Android Studio, build the library
    [memoryadvice](https://android.googlesource.com/platform/frameworks/opt/gamesdk/+/refs/heads/master/test/memoryadvice/).
2.  Drag the generated AAR (e.g.
    `gamesdk/test/memoryadvice/memoryadvice/build/outputs/aar/memoryadvice-debug.aar`)
    into the Assets folder of your Unity project. See
    [Unity instructions](https://docs.unity3d.com/Manual/AndroidAARPlugins.html)
    for including Android libraries in projects.
3.  Optional: add the
    [JSON Object package](https://assetstore.unity.com/packages/tools/input-management/json-object-710)
    to your Unity project.
4.  Copy
    [UnityMemoryHandler.cs](https://android.googlesource.com/platform/frameworks/opt/gamesdk/+/refs/heads/master/test/unitymemory/UnityMemoryHandler.cs)
    and drag the script into the Scripts folder of your Unity project.
5.  Create a new Game Object in your game scene, and drag the
    `UnityMemoryHandler.cs` script from your Assets folder to the new Game
    Object to associate it with the script.
6.  Customize the script as required.

## Reporting bugs

Email [jimblackler@google.com](mailto:jimblackler@google.com) Please include the
output from memoryAdvisor.getDeviceInfo().toString().

# Appendix

## Metrics groups

This is a comprehensive list of all the memory groups available, and the metrics
contained in those groups.

### "debug"

Metrics obtained from `android.os.debug`.

*   `nativeHeapAllocatedSize`

    From `Debug.getNativeHeapAllocatedSize()`

*   `NativeHeapFreeSize`

    From `Debug.getNativeHeapFreeSize()`

*   `NativeHeapSize`

    From `Debug.getNativeHeapSize()`

*   `Pss`

    From `Debug.getNativeHeapSize()`

#### Timing

Average 0.388 milliseconds.

### "MemoryInfo"

Metrics obtained from `android.app.ActivityManager.MemoryInfo`.

*   `availMem`

    From `memoryInfo.availMem`

*   `lowMemory`

    From `memoryInfo.lowMemory`

*   `totalMem`

    From `memoryInfo.totalMem`

*   `threshold`

    From `memoryInfo.threshold`

#### Timing

Average 0.907 milliseconds.

### "ActivityManager"

From `android.app.ActivityManager`.

*   `MemoryClass`

    From `activityManager.getMemoryClass()`

*   `LargeMemoryClass`

    From `activityManager.getLargeMemoryClass()`

*   `LowRamDevice`

    From `activityManager.isLowRamDevice()`

These values are all constant so only taken at startup.

### "proc"

*   `oom_score`

    From `proc/oom_score`.

#### Timing

Average 0.711 milliseconds.

### "summary"

From `android.os.Debug.MemoryInfo[]`.

This metric group is not recommended for use as it is very expensive to obtain
and is only availble in a throttled form on newer Android versions.

All fields via `android.app.ActivityManager.getProcessMemoryInfo`

*   `summary.java-heap`

*   `summary.native-heap`

*   `summary.code`

*   `summary.stack`

*   `summary.graphics`

*   `summary.private-other`

*   `summary.system`

*   `summary.total-pss`

*   `summary.total-swap`

#### Timing

Average 76.485 milliseconds.

### "memInfo"

From `/proc/meminfo`.

Fields are different per device, but examples include:

*   `Active`

*   `Active(anon)`

*   `Active(file)`

*   `AnonPages`

*   `Bounce`

*   `Buffers`

*   `Cached`

*   `CmaTotal`

*   `CommitLimit`

*   `Committed_AS`

*   `Dirty`

*   `Inactive`

*   `Inactive(anon)`

*   `Inactive(file)`

*   `KernelStack`

*   `Mapped`

*   `MemAvailable`

*   `MemFree`

*   `MemTotal`

*   `Mlocked`

*   `NFS_Unstable`

*   `PageTables`

*   `SReclaimable`

*   `SUnreclaim`

*   `Shmem`

*   `Slab`

*   `SwapCached`

*   `SwapFree`

*   `SwapTotal`

*   `Unevictable`

*   `VmallocChunk`

*   `VmallocTotal`

*   `VmallocUsed`

*   `Writeback`

*   `WritebackTmp`

#### Timing

Average 3.313 milliseconds.

### "status"

From `/proc/(pid)/status`.

Fields are different per device, but examples include:

*   `VmData`

*   `VmExe`

*   `VmHWM`

*   `VmLck`

*   `VmLib`

*   `VmPMD`

*   `VmPTE`

*   `VmPeak`

*   `VmPin`

*   `VmRSS`

*   `VmSize`

*   `VmStk`

*   `VmSwap`

#### Timing

Average 4.318 milliseconds.

### Notes on timing

Figures are based on 150+ lab devices, excluding outlier. The measurement is the
average time it takes for the library to obtain all these metrics.
