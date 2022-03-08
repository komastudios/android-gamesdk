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

#include "data_loader_machine.hpp"

void DataLoaderStateMachine::init() {
    mLocalDataLoadingCompleted = false;
    if (mIsCloudSaveEnabled) {
        // Make sure the last loading attempt finished correctly
        MY_ASSERT(mCloudDataLoadingCompleted && !mCloudInitializationCompleted);

        // Restart loading flags
        mCloudInitializationCompleted = true;
        mUserAuthenticationCompleted = false;
        mCloudDataFound = false;
        mCloudDataLoadingCompleted = false;
    }
}

int DataLoaderStateMachine::getLevelLoaded() {
    return mLevelLoaded;
}

void DataLoaderStateMachine::LoadLocalProgress() {
    ALOGI("Attempting to load locally: %s", mSaveFileName);
    mLevelLoaded = 0;
    FILE *f = fopen(mSaveFileName, "r");
    if (f) {
        ALOGI("Local file found. Loading data.");
        if (1 != fscanf(f, "v1 %d", &mLevelLoaded)) {
            ALOGE("Error parsing save file.");
            mLevelLoaded = 0;
        } else {
            ALOGI("Loaded. Level = %d", mLevelLoaded);
        }
        fclose(f);
    } else {
        ALOGI("Save file not present.");
    }
    mLocalDataLoadingCompleted = true;
}

void DataLoaderStateMachine::SaveLocalProgress(int level) {
    mLevelLoaded = level;
    ALOGI("Saving progress (level %d) to file: %s", level, mSaveFileName);
    FILE *f = fopen(mSaveFileName, "w");
    if (!f) {
        ALOGE("Error writing to save game file.");
        return;
    }
    fprintf(f, "v1 %d", level);
    fclose(f);
    ALOGI("Save file written.");
}

DataLoaderStateMachine::DataLoaderStateMachine(bool isCloudSaveEnabled, char *savePath) {
    mIsCloudSaveEnabled = isCloudSaveEnabled;
    int len = strlen(savePath) + strlen(SAVE_FILE_NAME) + 2;
    mSaveFileName = new char[len];
    strcpy(mSaveFileName, savePath);
    strcat(mSaveFileName, "/");
    strcat(mSaveFileName, SAVE_FILE_NAME);
    mLevelLoaded = 0;

    // At the beginning, we assume default level 0 is already loaded so completed flags are set
    // to true. NativeEngine::LoadProgress will then manage the flags.
    mLocalDataLoadingCompleted = true;
    mCloudDataLoadingCompleted = true;
    mCloudInitializationCompleted = false;
}

int DataLoaderStateMachine::getTotalSteps() {
    if (mIsCloudSaveEnabled) {
        return 4;
    }
    return 1;
}

int DataLoaderStateMachine::getStepsCompleted() {
    if (mIsCloudSaveEnabled) {
        if (mCloudDataLoadingCompleted) {
            return 4;
        }
        if (mCloudDataFound) {
            return 3;
        }
        if (mUserAuthenticationCompleted) {
            return 2;
        }
        if (mCloudInitializationCompleted) {
            return 1;
        }
    } else if(mLocalDataLoadingCompleted) {
        return 1;
    }
    return 0;
}

bool DataLoaderStateMachine::isLoadingDataCompleted() {
    if (mIsCloudSaveEnabled) {
        return mCloudDataLoadingCompleted;
    }
    return mLocalDataLoadingCompleted;
}

void DataLoaderStateMachine::authenticationCompleted() {
    MY_ASSERT(mCloudInitializationCompleted && !mUserAuthenticationCompleted);
    ALOGI("Cloud authentication completed, searching saved cloud data.");
    mUserAuthenticationCompleted = true;
}

void DataLoaderStateMachine::savedStateCloudDataFound() {
    MY_ASSERT(mUserAuthenticationCompleted && !mCloudDataFound);
    ALOGI("Cloud saved data found, continuing reading data.");
    mCloudDataFound = true;
}

void DataLoaderStateMachine::savedStateLoadingCompleted(int level) {
    MY_ASSERT(mCloudDataFound && !mCloudDataLoadingCompleted);
    ALOGI("Using cloud save data. Level loaded: %d", level);
    mLevelLoaded = level;
    mCloudDataLoadingCompleted = true;
    mCloudInitializationCompleted = false;
}

void DataLoaderStateMachine::authenticationFailed() {
    ALOGE("Error authenticating the user. Using local data instead.");
    LoadLocalProgress();
    mCloudDataLoadingCompleted = true;
    mCloudInitializationCompleted = false;
}

void DataLoaderStateMachine::savedStateSnapshotNotFound() {
    ALOGI("Cloud saved data not found. Using local data instead.");
    LoadLocalProgress();
    mCloudDataLoadingCompleted = true;
    mCloudInitializationCompleted = false;
}

void DataLoaderStateMachine::savedStateLoadingFailed() {
    ALOGE("Error on loading cloud data. Using local data instead.");
    LoadLocalProgress();
    mCloudDataLoadingCompleted = true;
    mCloudInitializationCompleted = false;
}
