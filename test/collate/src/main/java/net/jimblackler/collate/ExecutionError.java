package net.jimblackler.collate;

import java.io.IOException;

public class ExecutionError extends IOException {
  public ExecutionError() {}

  public ExecutionError(String message) {
    super(message);
  }

  public ExecutionError(String message, Throwable cause) {
    super(message, cause);
  }

  public ExecutionError(Throwable cause) {
    super(cause);
  }
}
