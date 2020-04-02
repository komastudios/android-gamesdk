package net.jimblackler.istresser;

import static net.jimblackler.istresser.Utils.getMemoryInfo;
import static net.jimblackler.istresser.Utils.getOomScore;
import static net.jimblackler.istresser.Utils.processMeminfo;

import android.app.ActivityManager;
import android.os.Debug;
import com.google.common.collect.ImmutableList;
import java.util.List;

/** An instance of this class describes a single mechanism for determining memory pressure. */
public abstract class Heuristic {

  /**
   * The value a heuristic returns when asked for memory pressure on the device through the
   * getSignal method. GREEN indicates it is safe to allocate further, YELLOW indicates further
   * allocation shouldn't happen, and RED indicates high memory pressure.
   */
  public enum Indicator {
    GREEN,
    YELLOW,
    RED
  }

  /** One word identifiers for various different heuristics. */
  public enum Identifier {
    TRIM,
    OOM,
    LOW,
    TRY,
    CL,
    AVAIL,
    CACHED,
    AVAIL2
  }

  static List<Heuristic> availableHeuristics =
      ImmutableList.of(
          /*
           * Checks the OOM Score of the app; returns RED if the score is over 650, GREEN otherwise.
           */
          new Heuristic(Identifier.OOM) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              return getOomScore(activityManager) <= 650 ? Indicator.GREEN : Indicator.RED;
            }
          },
          /*
           * Tries allocating and then immediately freeing 32 MB memory; returns GREEN if
           * allocation was successful, RED otherwise.
           */
          new Heuristic(Identifier.TRY) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              return tryAlloc(1024 * 1024 * 32) ? Indicator.GREEN : Indicator.RED;
            }
          },
          /*
           * Checks the lowMemory signal from activityManager's memInfo; returns RED if this signal
           * is present, GREEN otherwise.
           */
          new Heuristic(Identifier.LOW) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              return getMemoryInfo(activityManager).lowMemory ? Indicator.RED : Indicator.GREEN;
            }
          },
          /*
           * Returns RED if the commit limit is above a level that requires memory to be freed to
           * prevent the application from being killed, GREEN otherwise.
           */
          new Heuristic(Identifier.CL) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              Long value = processMeminfo().get("CommitLimit");
              if (value == null) {
                return Indicator.GREEN;
              }
              return Debug.getNativeHeapAllocatedSize() / 1024 > value
                  ? Indicator.RED
                  : Indicator.GREEN;
            }
          },
          /*
           * Returns RED if the availMem value is below a level that requires memory to be freed
           * to prevent the application from being killed, GREEN otherwise.
           */
          new Heuristic(Identifier.AVAIL) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
              if (memoryInfo.availMem < memoryInfo.threshold * 2) {
                return Indicator.RED;
              } else if (memoryInfo.availMem < memoryInfo.threshold * 3) {
                return Indicator.YELLOW;
              } else {
                return Indicator.GREEN;
              }
            }
          },
          /*
           * Returns RED if the cached value is at a level that requires memory to be freed to
           * prevent the application from being killed, GREEN otherwise.
           */
          new Heuristic(Identifier.CACHED) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              Long value = processMeminfo().get("Cached");
              if (value == null || value == 0) {
                return Indicator.GREEN;
              }
              ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
              return value < memoryInfo.threshold / 1024 ? Indicator.RED : Indicator.GREEN;
            }
          },
          /*
           * Returns RED if the MemAvailable value is at a level that requires memory to be freed to
           * prevent the application from being killed, GREEN otherwise.
           */
          new Heuristic(Identifier.AVAIL2) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              Long value = processMeminfo().get("MemAvailable");
              if (value == null || value == 0) {
                return Indicator.GREEN;
              }
              ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
              return value < memoryInfo.threshold * 2 / 1024 ? Indicator.RED : Indicator.GREEN;
            }
          });

  private final Identifier identifier;

  Heuristic(Identifier identifier) {
    this.identifier = identifier;
  }

  public Identifier getIdentifier() {
    return identifier;
  }

  public abstract Indicator getSignal(ActivityManager activityManager);

  static {
    System.loadLibrary("native-lib");
  }

  public native boolean tryAlloc(int bytes);
}
