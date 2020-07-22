package com.google.android.apps.internal.games.memoryadvice;

import java.util.Timer;
import java.util.TimerTask;
import org.json.JSONObject;

/**
 * A MemoryWatcher automatically polls the memory advisor and calls back a client a soon as possible
 * when the state changes.
 */
public class MemoryWatcher {
  private final long watcherStartTime;
  private final Runnable runner;
  private final Timer timer = new Timer();
  private MemoryAdvisor.MemoryState lastReportedState = MemoryAdvisor.MemoryState.UNKNOWN;
  private long totalTimeSpent;

  /**
   * Create a MemoryWatcher object. This calls back the supplied client when the memory state
   * changes.
   * @param memoryAdvisor The memory advisor object to employ.
   * @param maxMillisecondsPerSecond The budget for overhead introduced by the advisor and watcher.
   * @param client The client to call back when the state changes.
   */
  public MemoryWatcher(
      final MemoryAdvisor memoryAdvisor, final long maxMillisecondsPerSecond, final Client client) {
    watcherStartTime = System.currentTimeMillis();
    runner = new Runnable() {
      @Override
      public void run() {
        long start = System.currentTimeMillis();
        JSONObject advice = memoryAdvisor.getAdvice();
        MemoryAdvisor.MemoryState memoryState = MemoryAdvisor.getMemoryState(advice);
        if (memoryState != lastReportedState) {
          lastReportedState = memoryState;
          client.newState(memoryState);
        }
        long end = System.currentTimeMillis();
        long duration = end - start;  // Time spent this iteration.
        totalTimeSpent += duration;

        // The total time the object has been created (in milliseconds).
        long timeSinceStart = end - watcherStartTime;

        // The total amount of time that the method has been allotted to date (in milliseconds).
        long totalBudget = timeSinceStart * maxMillisecondsPerSecond / 1000;

        // How much more than the allotted time was actually used (in milliseconds).
        long overBudget = totalTimeSpent - totalBudget;

        // Sleep until the moment that the method will be within its budget. Then invoke the next
        // iteration. The method therefore runs 'in arrears' of its budget but it is still
        // guaranteed to be under budget at the start of each iteration,.
        timer.schedule(new TimerTask() {
          @Override
          public void run() {
            runner.run();
          }
        }, Math.max(overBudget, 1));  // Rerun immediately if still under budget after an iteration.
      }
    };
    runner.run();
  }

  /**
   * A client for the MemoryWatcher class.
   */
  public abstract static class Client {
    public abstract void newState(MemoryAdvisor.MemoryState state);
  }
}
