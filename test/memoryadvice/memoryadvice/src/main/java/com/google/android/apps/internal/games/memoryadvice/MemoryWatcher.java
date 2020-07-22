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
        long duration = end - start;
        totalTimeSpent += duration;
        long timeSinceStart = end - watcherStartTime;
        long totalBudget = timeSinceStart * maxMillisecondsPerSecond / 1000;
        long overBudget = totalTimeSpent - totalBudget;
        timer.schedule(new TimerTask() {
          @Override
          public void run() {
            runner.run();
          }
        }, Math.max(overBudget, 1));
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
