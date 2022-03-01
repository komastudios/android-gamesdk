/*
 * Copyright 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.google.sample.agdktunnel;

import android.content.Context;
import android.util.Log;

import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;

public class PGSManager {

    private static PGSManager singletonInstance;
    private final Context mContext;

    private PGSManager(Context context) {
        mContext = context;
        PlayGamesSdk.initialize(mContext);
        verifyAuthentication();
    }

    public static PGSManager getInstance(Context context) {
        if (singletonInstance == null) {
            singletonInstance = new PGSManager(context);
        }
        return singletonInstance;
    }

    private void verifyAuthentication() {
        GamesSignInClient gamesSignInClient =
                PlayGames.getGamesSignInClient((android.app.Activity)mContext);
        gamesSignInClient.isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
            boolean isAuthenticated = (isAuthenticatedTask.isSuccessful() &&
                    isAuthenticatedTask.getResult().isAuthenticated());
            if (isAuthenticated) {
                // Continue with Play Games Services
                Log.i("AGDK Tunnel","the user is authenticated with Play Games Services");
            } else {
                // Disable your integration with Play Games Services or show a login button to ask
                // players to sign-in. Clicking it should call GamesSignInClient.signIn().
                Log.e("AGDK Tunnel","the user is not authenticated with Play Games Services");
                gamesSignInClient.signIn();
            }
        });
    }
}
