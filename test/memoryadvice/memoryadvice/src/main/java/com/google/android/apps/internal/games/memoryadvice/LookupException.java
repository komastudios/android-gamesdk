package com.google.android.apps.internal.games.memoryadvice;

/**
 * Denotes an error that occured during variable expression evaluation.
 */
class LookupException extends Exception {
  LookupException(String message) {
    super(message);
  }
}
