package com.google.sample.agdktunnel;

import android.content.Context;

/**
 * The PlayGamesUtils is the delegate to include Google Play Games dependencies and basic operations
 * to initialize and finalize the game on the platform. If the game is not running in Google Play
 * Games then a default implementation takes its place.
 */
public interface PlayGamesUtils {

    /* Initialize utilities of Google Play Games */
    void init(Context context);

    /* Finalize utilities of Google Play Games */
    void close(Context context);
}
