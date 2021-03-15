package com.google.android.apps.internal.games.memoryadvice;

/**
 * An interface for evaluating variables during a forumula evaluation.
 */
interface Lookup {
  Double lookup(String parameter) throws LookupException;
}
