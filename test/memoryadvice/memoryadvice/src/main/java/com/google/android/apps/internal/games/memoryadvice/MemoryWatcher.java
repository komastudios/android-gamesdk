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

        // Calculate the run time required to have enough budget for the time actually spent
        // (in milliseconds).
        long timeRequired = totalTimeSpent * 1000 / maxMillisecondsPerSecond;

        // The total time the object has been created (in milliseconds).
        long timeSinceStart = end - watcherStartTime;

        // Sleep until the moment that the method will be within its budget, plus the estimated run
        // time to avoid the method being 'in arrears' of its budget.
        long sleepFor = timeRequired - timeSinceStart + duration;
        if (sleepFor < 1) {
          sleepFor = 1;  // Run immediately in the unlikely case of being well under budget.
        }

        timer.schedule(new TimerTask() {
          @Override
          public void run() {
            runner.run();
          }
        }, sleepFor);
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
