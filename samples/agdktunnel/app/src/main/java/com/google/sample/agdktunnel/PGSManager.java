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
import com.google.android.gms.tasks.Tasks;

import java.io.IOException;

public class PGSManager {

    private static PGSManager singletonInstance;
    private final Context mContext;
    private final SnapshotsClient mSnapshotClient;
    private final String snapshotName = "AGDKTunnelLevel";
    private final String TAG = "AGDK Tunnel (PGS)";

    private PGSManager(Context context) {
        mContext = context;
        mSnapshotClient = PlayGames.getSnapshotsClient((android.app.Activity)mContext);
        PlayGamesSdk.initialize(mContext);
    }

    public static PGSManager getInstance(Context context) {
        if (singletonInstance == null) {
            singletonInstance = new PGSManager(context);
        }
        return singletonInstance;
    }

    void saveCheckpoint(int level) {
        // In the case of a conflict, the most recently modified version of this snapshot will be
        // used.
        int conflictResolutionPolicy = SnapshotsClient.RESOLUTION_POLICY_MOST_RECENTLY_MODIFIED;
        // In the case a Snapshot is not found, a new snapshot will be created
        boolean snapshotNotFoundPolicy = true;

        byte[] data = {(byte) level};
        mSnapshotClient.open(snapshotName, snapshotNotFoundPolicy, conflictResolutionPolicy)
                .addOnCompleteListener(
            new OnCompleteListener<SnapshotsClient.DataOrConflict<Snapshot>>() {
                @Override
                public void onComplete(
                        @NonNull Task<SnapshotsClient.DataOrConflict<Snapshot>> task) {
                    Log.i(TAG, "Attempting to save level on the cloud");
                    Snapshot snapshotToWrite = task.getResult().getData();

                    if (snapshotToWrite == null) {
                        // No snapshot available
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
    }

    private Task<SnapshotMetadata> writeSnapshot(
            Snapshot snapshot, byte[] data, String description) {
        // Set the data payload for the snapshot
        snapshot.getSnapshotContents().writeBytes(data);

        // Create the change operation
        SnapshotMetadataChange metadataChange = new SnapshotMetadataChange.Builder()
            .setDescription(description)
            .build();

        // Commit the operation
        return mSnapshotClient.commitAndClose(snapshot, metadataChange);
    }

    int loadCheckpoint() {
        // Do not call this function from the main (UI) thread
        try {
            byte[] result = Tasks.await(loadSnapshot());
            if (result != null && result.length > 0) {
                Log.i(TAG, "Snapshot read successfully.");
                return (int)result[0];
            }
        } catch (Exception e) {
            Log.e(TAG, "Error while reading Snapshot, level 0 will load instead.", e);
        }
        return 0;
    }

    private Task<byte[]> loadSnapshot() {
        // In the case of a conflict, the most recently modified version of this snapshot will be
        // used.
        int conflictResolutionPolicy = SnapshotsClient.RESOLUTION_POLICY_MOST_RECENTLY_MODIFIED;
        // In the case a Snapshot is not found, a new snapshot will be created
        boolean snapshotNotFoundPolicy = true;

        // Open the saved game using its name.
        return mSnapshotClient.open(snapshotName, snapshotNotFoundPolicy, conflictResolutionPolicy)
            .addOnFailureListener(new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception e) {
                    Log.e(TAG, "Error while opening Snapshot.", e);
                }
            }).continueWith(new Continuation<SnapshotsClient.DataOrConflict<Snapshot>, byte[]>() {
                @Override
                public byte[] then(@NonNull Task<SnapshotsClient.DataOrConflict<Snapshot>> task)
                        throws Exception {
                    Snapshot snapshot = task.getResult().getData();
                    // Opening the snapshot was a success and any conflicts have been resolved.
                    try {
                        // Extract the raw data from the snapshot.
                        Log.i(TAG, "Reading the Snapshot content.");
                        return snapshot.getSnapshotContents().readFully();
                    } catch (IOException e) {
                        Log.e(TAG, "Error while reading Snapshot.", e);
                    }
                    return null;
                }
            });
    }

    boolean isUserAuthenticated() {
        // Do not call this function from the main (UI) thread
        GamesSignInClient gamesSignInClient =
                PlayGames.getGamesSignInClient((android.app.Activity)mContext);
        Log.i(TAG, "Verifying user authentication");
        Task<AuthenticationResult> task = gamesSignInClient.isAuthenticated();
        try {
            AuthenticationResult result = Tasks.await(task);
            if (task.isSuccessful() && result.isAuthenticated()) {
                Log.i(TAG, "the user is authenticated with Play Games Services");
                return true;
            } else {
                Log.e(TAG, "the user is not authenticated with Play Games Services");
                return false;
            }
        } catch (Exception e) {
            Log.e(TAG, "Error while authenticating the user", e);
        }
        return false;
    }
}
