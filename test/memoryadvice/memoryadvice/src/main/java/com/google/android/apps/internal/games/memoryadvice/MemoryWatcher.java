package com.google.android.apps.internal.games.memoryadvice;

import java.io.Closeable;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * A MemoryWatcher automatically polls the memory advisor and calls back a client a soon as possible
 * when the state changes.
 */
public class MemoryWatcher implements Closeable {
  private final Runnable runner;

  private MemoryAdvisor.MemoryState lastReportedState = MemoryAdvisor.MemoryState.UNKNOWN;
  private boolean interrupt;

  /**
   * Create a MemoryWatcher object. This calls back the supplied client when the memory state
   * changes.
   *
   * @param memoryAdvisor            The memory advisor object to employ.
   * @param delay                    Delay between polls.
   * @param client                   The client to call back when the state changes.
   */
  public MemoryWatcher(MemoryAdvisor memoryAdvisor, long delay, Client client) {
    ScheduledExecutorService scheduledExecutorService = Executors.newSingleThreadScheduledExecutor(
        runnable -> Executors.defaultThreadFactory().newThread(runnable));

    runner = new Runnable() {
      @Override
      public void run() {
        if (interrupt) {
          return;
        }
        Map<String, Object> advice = memoryAdvisor.getAdvice();
        client.receiveAdvice(advice);
        MemoryAdvisor.MemoryState memoryState = MemoryAdvisor.getMemoryState(advice);
        if (memoryState != lastReportedState) {
          lastReportedState = memoryState;
          client.newState(memoryState);
        }
        scheduledExecutorService.schedule(runner, delay, TimeUnit.MILLISECONDS);
      }
    };
    runner.run();
  }

  @Override
  public void close() {
    interrupt = true;
  }

  /**
   * A client for the MemoryWatcher class.
   */
  public interface Client {
    void newState(MemoryAdvisor.MemoryState state);
    void receiveAdvice(Map<String, Object> advice);
  }

  /**
   * A client for the MemoryWatcher class, which allows only the methods required to be overridden.
   */
  public abstract static class DefaultClient implements Client {
    @Override
    public void newState(MemoryAdvisor.MemoryState state) {}

    @Override
    public void receiveAdvice(Map<String, Object> advice) {}
  }
}
