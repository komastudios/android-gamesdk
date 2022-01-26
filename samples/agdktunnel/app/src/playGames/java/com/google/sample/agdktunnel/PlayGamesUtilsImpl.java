package com.google.sample.agdktunnel;

import android.content.Context;

import com.google.android.libraries.play.games.inputmapping.InputMappingClient;
import com.google.android.libraries.play.games.inputmapping.InputMappingProvider;
import com.google.android.libraries.play.games.inputmapping.Input;

/**
 * Implementation of Google Play Games utilities to add dependencies and support basic operations.
 */
public class PlayGamesUtilsImpl implements PlayGamesUtils {

    public void init(Context context) {
        InputMappingProvider inputMappingProvider = new InputSDKProvider();
        InputMappingClient inputMappingClient = Input.getInputMappingClient(context);
        inputMappingClient.setInputMappingProvider(inputMappingProvider);
    }

    public void close(Context context) {
        InputMappingClient inputMappingClient = Input.getInputMappingClient(context);
        inputMappingClient.clearInputMappingProvider();
    }
}
