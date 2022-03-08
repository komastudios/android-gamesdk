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
#ifndef agdktunnel_data_loader_machine_hpp
#define agdktunnel_data_loader_machine_hpp

#include "common.hpp"

// save file name
#define SAVE_FILE_NAME "tunnel.dat"

// checkpoint (save progress) every how many levels?
#define LEVELS_PER_CHECKPOINT 4

class DataLoaderStateMachine {
private:

    enum DataLoadStates {
        LOAD_NOT_STARTED,
        DATA_INITIALIZED,
        CLOUD_USER_AUTHENTICATED,
        CLOUD_DATA_FOUND,
        DATA_LOADED
    };

    DataLoadStates mCurrentState;

    // Level to start from in play scene
    int mLevelLoaded;

    // name of the save file
    char *mSaveFileName;

    // Flag to know if cloud save is enabled
    bool mIsCloudSaveEnabled;

public:

    DataLoaderStateMachine(bool isCloudSaveEnabled, char *savePath);

    ~DataLoaderStateMachine();

    void init();

    void authenticationCompleted();

    void authenticationFailed();

    void savedStateSnapshotNotFound();

    void savedStateLoadingFailed();

    void savedStateLoadingCompleted(int level);

    void savedStateCloudDataFound();

    int getLevelLoaded();

    int getTotalSteps();

    int getStepsCompleted();

    bool isLoadingDataCompleted();

    void LoadLocalProgress();

    void SaveLocalProgress(int level);

};

#endif //agdktunnel_data_loader_machine_hpp
