package net.jimblackler.istresser;

import static net.jimblackler.istresser.Heuristics.getMemoryInfo;
import static net.jimblackler.istresser.Heuristics.processMeminfo;

import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.os.Debug;
import com.google.common.collect.ImmutableList;
import java.util.List;

public abstract class Heuristic {

  private static final long AVAIL_THRESHOLD_1 = 1024 * 1024 * 300;
  private static final long AVAIL_THRESHOLD_2 = 1024 * 1024 * 200;

  public enum Indicator {
    GREEN,
    YELLOW,
    RED
  }

  public enum Identifier {
    VMSIZE,
    TRIM,
    OOM,
    LOW,
    TRY,
    CL,
    CL2,
    AVAIL,
    CACHED,
    AVAIL2
  }

  static final List<Heuristic> availableHeuristics =
      ImmutableList.of(
          /*
           * Checks the OOM Score of the app; returns RED if the score is over 650, GREEN otherwise.
           */
          new Heuristic(Identifier.VMSIZE) {


            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              long value = processMeminfo().get("CommitLimit");
              long vmSize = Heuristics.processStatus(activityManager).get("VmSize");

              Indicator indicator;
              if (vmSize > value * 0.95f) {
                indicator = Indicator.RED;
              } else {
                indicator = Indicator.GREEN;
              }
;
              return indicator;

            }
          },
          /*
           * Checks the OOM Score of the app; returns RED if the score is over 650, GREEN otherwise.
           */
          new Heuristic(Identifier.OOM) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              return Heuristics.getOomScore(activityManager) <= 650
                  ? Indicator.GREEN
                  : Indicator.RED;
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

          new Heuristic(Identifier.CL2) {
            @Override
            public Indicator getSignal(ActivityManager activityManager) {
              Long value = processMeminfo().get("CommitLimit");
              if (value == null) {
                return Indicator.GREEN;
              }
              return Debug.getNativeHeapAllocatedSize() / 1024 > value * 0.90f
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
              MemoryInfo memoryInfo = getMemoryInfo(activityManager);
              return memoryInfo.availMem < AVAIL_THRESHOLD_1
                  ? Indicator.RED
                  : Indicator.GREEN;
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
              MemoryInfo memoryInfo = getMemoryInfo(activityManager);
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
              MemoryInfo memoryInfo = getMemoryInfo(activityManager);
              Long value = processMeminfo().get("MemAvailable");
              if (value == null || value == 0) {
                return Indicator.GREEN;
              }
              return value < AVAIL_THRESHOLD_1 ? Indicator.RED : Indicator.GREEN;
            }
          });

  private Identifier identifier;

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
