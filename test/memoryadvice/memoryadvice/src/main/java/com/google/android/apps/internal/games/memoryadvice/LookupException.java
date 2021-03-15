package com.google.android.apps.internal.games.memoryadvice;

/**
 * Denotes an error that occurred during variable expression evaluation.
 */
class LookupException extends Exception {
  LookupException(String message) {
    super(message);
  }
}
