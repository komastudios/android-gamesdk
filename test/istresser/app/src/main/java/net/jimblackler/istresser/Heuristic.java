package net.jimblackler.istresser;

import static net.jimblackler.istresser.Heuristics.getMemoryInfo;
import static net.jimblackler.istresser.Heuristics.processMeminfo;

import android.app.Activity;
import android.app.ActivityManager;
import android.os.Debug;
import com.google.common.collect.ImmutableList;
import java.util.List;

public abstract class Heuristic {

  public enum Indicator {
    GREEN,
    YELLOW,
    RED
  }

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

  public static List<Heuristic> availableHeuristics =
      ImmutableList.of(
          new Heuristic(Identifier.OOM) {
            @Override
            public Indicator getSignal(Activity activity) {
              return Heuristics.getOomScore(activity) <= 650 ? Indicator.GREEN : Indicator.RED;
            }
          },
          new Heuristic(Identifier.TRY) {
            @Override
            public Indicator getSignal(Activity activity) {
              return tryAlloc(1024 * 1024 * 32) ? Indicator.GREEN : Indicator.RED;
            }
          },
          new Heuristic(Identifier.LOW) {
            @Override
            public Indicator getSignal(Activity activity) {
              return getMemoryInfo(activity).lowMemory ? Indicator.RED : Indicator.GREEN;
            }
          },
          new Heuristic(Identifier.CL) {
            @Override
            public Indicator getSignal(Activity activity) {
              Long value = processMeminfo().get("CommitLimit");
              if (value == null) {
                return Indicator.GREEN;
              }
              return Debug.getNativeHeapAllocatedSize() / 1024 > value
                  ? Indicator.RED
                  : Indicator.GREEN;
            }
          },
          new Heuristic(Identifier.AVAIL) {
            @Override
            public Indicator getSignal(Activity activity) {
              ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activity);
              return memoryInfo.availMem < memoryInfo.threshold * 2
                  ? Indicator.RED
                  : Indicator.GREEN;
            }
          },
          new Heuristic(Identifier.CACHED) {
            @Override
            public Indicator getSignal(Activity activity) {
              Long value = processMeminfo().get("Cached");
              if (value == null || value == 0) {
                return Indicator.GREEN;
              }
              ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activity);
              return value < memoryInfo.threshold / 1024 ? Indicator.RED : Indicator.GREEN;
            }
          },
          new Heuristic(Identifier.AVAIL2) {
            @Override
            public Indicator getSignal(Activity activity) {
              Long value = processMeminfo().get("MemAvailable");
              if (value == null || value == 0) {
                return Indicator.GREEN;
              }
              ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activity);
              return value < memoryInfo.threshold * 2 / 1024 ? Indicator.RED : Indicator.GREEN;
            }
          });

  private Identifier identifier;

  Heuristic(Identifier identifier) {
    this.identifier = identifier;
  }

  public Identifier getIdentifier() {
    return identifier;
  }

  public abstract Indicator getSignal(Activity activity);

  static {
    System.loadLibrary("native-lib");
  }

  public native boolean tryAlloc(int bytes);
}
