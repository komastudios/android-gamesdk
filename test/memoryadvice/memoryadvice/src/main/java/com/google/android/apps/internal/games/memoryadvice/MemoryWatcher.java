package com.google.android.apps.internal.games.memoryadvice;

import java.util.TimerTask;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import org.json.JSONObject;

/**
 * A MemoryWatcher automatically polls the memory advisor and calls back a client a soon as possible
 * when the state changes.
 */
public class MemoryWatcher {
  private static final int UNRESPONSIVE_THRESHOLD = 100;

  private final long watcherStartTime;
  private final Runnable runner;

  private long expectedTime;
  private long unresponsiveTime;
  private MemoryAdvisor.MemoryState lastReportedState = MemoryAdvisor.MemoryState.UNKNOWN;
  private long totalTimeSpent;

  /**
   * Create a MemoryWatcher object. This calls back the supplied client when the memory state
   * changes.
   *
   * @param memoryAdvisor            The memory advisor object to employ.
   * @param maxMillisecondsPerSecond The budget for overhead introduced by the advisor and watcher.
   * @param minimumFrequency         The minimum time duration between iterations, in milliseconds.
   * @param maximumFrequency         The maximum time duration between iterations, in milliseconds.
   * @param client                   The client to call back when the state changes.
   */
  public MemoryWatcher(MemoryAdvisor memoryAdvisor, long maxMillisecondsPerSecond,
      long minimumFrequency, long maximumFrequency, Client client) {
    watcherStartTime = System.currentTimeMillis();
    expectedTime = watcherStartTime;

    ScheduledExecutorService scheduledExecutorService =
        Executors.newSingleThreadScheduledExecutor(runnable -> {
          Thread thread = Executors.defaultThreadFactory().newThread(runnable);
          thread.setPriority(Thread.MIN_PRIORITY);
          return thread;
        });

    runner = new Runnable() {
      @Override
      public void run() {
        long start = System.currentTimeMillis();
        JSONObject advice = memoryAdvisor.getAdvice();
        client.receiveAdvice(advice);
        MemoryAdvisor.MemoryState memoryState = MemoryAdvisor.getMemoryState(advice);
        long late = start - expectedTime;
        if (late > UNRESPONSIVE_THRESHOLD) {
          // The timer fired very late. We deduct the 'lost' time from the runtime used for
          // calculation.
          unresponsiveTime += late;
        }
        long end = System.currentTimeMillis();
        if (memoryState != lastReportedState) {
          lastReportedState = memoryState;
          client.newState(memoryState);
        }

        // Time spent this iteration.
        long duration = end - start;
        totalTimeSpent += duration;

        // Calculate the run time required to have enough budget for the time actually spent
        // (in milliseconds).
        long targetTime = totalTimeSpent * 1000 / maxMillisecondsPerSecond;

        // The total time the object has been created (in milliseconds), minus any time when the
        // watcher was unresponsive.
        long timeSinceStart = end - watcherStartTime - unresponsiveTime;

        // Sleep until the moment that the method will be within its budget.
        long sleepFor = targetTime - timeSinceStart;

        if (sleepFor < minimumFrequency) {
          sleepFor = minimumFrequency;  // Impose minimum frequency.
        } else if (sleepFor > maximumFrequency) {
          sleepFor = maximumFrequency;  // Impose maximum frequency.
        }
        expectedTime = System.currentTimeMillis() + sleepFor;
        scheduledExecutorService.schedule(runner, sleepFor, TimeUnit.MILLISECONDS);
      }
    };
    runner.run();
  }

  /**
   * A client for the MemoryWatcher class.
   */
  public interface Client {
    void newState(MemoryAdvisor.MemoryState state);
    void receiveAdvice(JSONObject advice);
  }

  /**
   * A client for the MemoryWatcher class, which allows only the methods required to be overridden.
   */
  public abstract static class DefaultClient implements Client {
    @Override
    public void newState(MemoryAdvisor.MemoryState state) {}

    @Override
    public void receiveAdvice(JSONObject advice) {}
  }
}
