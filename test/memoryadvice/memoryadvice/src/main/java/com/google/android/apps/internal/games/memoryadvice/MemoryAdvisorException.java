package com.google.android.apps.internal.games.memoryadvice;

import android.os.RemoteException;

/**
 * Internal or configuration errors in the Memory Advisor.
 */
class MemoryAdvisorException extends RuntimeException {
  MemoryAdvisorException(String message) {
    super(message);
  }

  MemoryAdvisorException(Throwable cause) {
    super(cause);
  }

  MemoryAdvisorException(String message, Throwable cause) {
    super(message, cause);
  }
}
