/*
 * Copyright 2021 The Android Open Source Project
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

import androidx.annotation.NonNull;

import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.drive.Drive;
import com.google.android.gms.games.AuthenticationResult;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;
import com.google.android.gms.games.SnapshotsClient;
import com.google.android.gms.games.snapshot.Snapshot;
import com.google.android.gms.games.snapshot.SnapshotMetadata;
import com.google.android.gms.games.snapshot.SnapshotMetadataChange;
import com.google.android.gms.tasks.Continuation;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.Task;

import java.io.IOException;

public class PGSManager {

    private static PGSManager singletonInstance;
    private final Context mContext;
    private final SnapshotsClient mSnapshotClient;
    private final String SNAPSHOT_NAME = "AGDKTunnelLevel";
    private final boolean CREATE_SNAPSHOT_IF_NOT_FOUND = true;
    private final String TAG = "RUVALCABAC FLAG (PGS)";

    private PGSManager(Context context) {
        mContext = context;
        mSnapshotClient = PlayGames.getSnapshotsClient((android.app.Activity)mContext);
        PlayGamesSdk.initialize(mContext);
        if (!isUserAuthenticated()) {
            GamesSignInClient gamesSignInClient =
                    PlayGames.getGamesSignInClient((android.app.Activity)mContext);
            gamesSignInClient.signIn();
        }
    }

    public static PGSManager getInstance(Context context) {
        if (singletonInstance == null) {
            singletonInstance = new PGSManager(context);
        }
        return singletonInstance;
    }

    public void saveCheckpoint(int level) {
        // In the case of a conflict, the most recently modified version of this snapshot will be used.
        int conflictResolutionPolicy = SnapshotsClient.RESOLUTION_POLICY_MOST_RECENTLY_MODIFIED;
        Log.i(TAG, "setting snapshot listener");
        byte[] data = {(byte) level};
        mSnapshotClient.open(SNAPSHOT_NAME, CREATE_SNAPSHOT_IF_NOT_FOUND, conflictResolutionPolicy)
                .addOnCompleteListener(
            new OnCompleteListener<SnapshotsClient.DataOrConflict<Snapshot>>() {

                @Override
                public void onComplete(@NonNull Task<SnapshotsClient.DataOrConflict<Snapshot>> task) {
                    Log.i(TAG, "Attempting to save level on the cloud");
                    Snapshot snapshotToWrite = task.getResult().getData();

                    Log.i(TAG, "checking snapshot");
                    if (snapshotToWrite == null) {
                        // No snapshot available
                        Log.i(TAG, "No snapshot available");
                        return;
                    }
                    writeSnapshot(snapshotToWrite, data, "AGDK Tunnel checkpoint level")
                        .addOnCompleteListener(new OnCompleteListener<SnapshotMetadata>() {
                            @Override
                            public void onComplete(@NonNull Task<SnapshotMetadata> task) {
                                if (task.isSuccessful()) {
                                    Log.i(TAG, "Snapshot saved!");
                                } else {
                                    Log.e(TAG, "Snapshot was not saved");
                                }
                            }
                        });
                }
            });
        Log.i(TAG, "setting snapshot listener (2/2)");
    }

    private Task<SnapshotMetadata> writeSnapshot(Snapshot snapshot, byte[] data, String description) {
        // Set the data payload for the snapshot
        snapshot.getSnapshotContents().writeBytes(data);

        Log.i(TAG, "message from the writeSnapshot function (1/2)");
        // Create the change operation
        SnapshotMetadataChange metadataChange = new SnapshotMetadataChange.Builder()
                .setDescription(description)
                .build();

        Log.i(TAG, "message from the writeSnapshot function (2/2)");
        // Commit the operation
        return mSnapshotClient.commitAndClose(snapshot, metadataChange);
    }

    public int loadCheckpoint() {
        Log.i(TAG, "logging from loadCheckpoint");
        Task<byte[]> task = loadSnapshot();
        byte[] result = task.getResult();
        if (result != null && result.length > 0) {
            Log.i(TAG, "found snapshot, level: " + String.valueOf((int)result[0]));
            return (int)result[0];
        }
        Log.i(TAG, "snapshot was not found");
        return 0;
    }

    Task<byte[]> loadSnapshot() {
        // In the case of a conflict, the most recently modified version of this snapshot will be used.
        int conflictResolutionPolicy = SnapshotsClient.RESOLUTION_POLICY_MOST_RECENTLY_MODIFIED;

        // Open the saved game using its name.
        return mSnapshotClient.open(
                SNAPSHOT_NAME, CREATE_SNAPSHOT_IF_NOT_FOUND, conflictResolutionPolicy)
            .addOnFailureListener(new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception e) {
                    Log.e(TAG, "Error while opening Snapshot.", e);
                }
            }).continueWith(new Continuation<SnapshotsClient.DataOrConflict<Snapshot>, byte[]>() {
                @Override
                public byte[] then(@NonNull Task<SnapshotsClient.DataOrConflict<Snapshot>> task) throws Exception {
                    Log.i(TAG, "loading snapshot!");
                    Snapshot snapshot = task.getResult().getData();
                    Log.i(TAG, "snapshot loaded!");
                    // Opening the snapshot was a success and any conflicts have been resolved.
                    try {
                        // Extract the raw data from the snapshot.
                        Log.i(TAG, "the content of the snapshot is: " + String.valueOf(snapshot.getSnapshotContents().readFully()));
                        return snapshot.getSnapshotContents().readFully();
                    } catch (IOException e) {
                        Log.e(TAG, "Error while reading Snapshot.", e);
                    }
                    Log.i(TAG, "found a null snapshot");

                    return null;
                }
            });
    }

    public boolean isUserAuthenticated() {
        GamesSignInClient gamesSignInClient =
                PlayGames.getGamesSignInClient((android.app.Activity)mContext);
        Task<AuthenticationResult> task = gamesSignInClient
                .isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
            boolean isAuthenticated = (isAuthenticatedTask.isSuccessful() &&
                    isAuthenticatedTask.getResult().isAuthenticated());
            if (isAuthenticated) {
                // Continue with Play Games Services
                Log.i(TAG,"the user is authenticated with Play Games Services");
            } else {
                // Disable your integration with Play Games Services or show a login button to ask
                // players to sign-in. Clicking it should call GamesSignInClient.signIn().
                Log.e(TAG,"the user is not authenticated with Play Games Services");
            }
        });

        return task.isSuccessful() && task.getResult().isAuthenticated();
    }
}
