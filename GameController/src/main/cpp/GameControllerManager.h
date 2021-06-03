/*
 * Copyright (C) 2021 The Android Open Source Project
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

#pragma once

#include <jni.h>
#include <mutex>
#include <android/input.h>
#include "GameController.h"
#include "ThreadUtil.h"

namespace paddleboat {

    class GameControllerManager {
    private:
        // Allows construction with std::unique_ptr from a static method, but
        // disallows construction outside of the class since no one else can
        // construct a ConstructorTag
        struct ConstructorTag {
        };

        static constexpr int32_t MAX_MOUSE_DEVICES = 2;
        static constexpr int32_t INVALID_MOUSE_ID = -1;
        static constexpr int32_t MAX_REMAP_TABLE_SIZE = 256;
    public:
        GameControllerManager(JNIEnv *env, jobject jactivity, ConstructorTag);

        ~GameControllerManager();

        static inline int32_t getRemapTableSize() { return MAX_REMAP_TABLE_SIZE; }

        static bool init(JNIEnv *env, jobject jactivity, const char *libraryName);

        static void destroyInstance();

        static bool isEnabled();

        // Get/Set whether AKEYCODE_BACK is 'eaten' or allowed to pass through to the system
        // This can be used to block the OS backing out of the game, or allowing it if the
        // game is in an appropriate state (i.e. the title screen)
        static bool getBackButtonConsumed();

        static void setBackButtonConsumed(bool consumed);

        static void setControllerStatusCallback(
                Paddleboat_ControllerStatusCallback statusCallback);

        static void setMouseStatusCallback(
                Paddleboat_MouseStatusCallback statusCallback);

        static void onPause();

        static void onResume();

        static void update();

        static bool getControllerData(const int32_t controllerIndex,
                                      Paddleboat_Controller_Data *controllerData);

        static bool
        getControllerInfo(const int32_t controllerIndex, Paddleboat_Controller_Info *deviceInfo);

        static Paddleboat_ControllerStatus getControllerStatus(const int32_t controllerIndex);

        static bool setControllerVibrationData(const int32_t controllerIndex,
                                               const Paddleboat_Vibration_Data *vibrationData);

        static bool
        setControllerLight(const int32_t controllerIndex, const Paddleboat_LightType lightType,
                           const uint32_t lightData);

        static bool getMouseData(Paddleboat_Mouse_Data *mouseData);

        static Paddleboat_MouseStatus getMouseStatus();

        static int32_t processInputEvent(const AInputEvent *event);

        static int32_t processGameActivityInputEvent(const Paddleboat_GameActivityEvent eventType,
                                                     const void *event,
                                                     const size_t eventSize);

        static uint64_t getActiveAxisMask();

        static void addControllerRemapData(const Paddleboat_Remap_Addition_Mode addMode,
                                           const int32_t remapTableEntryCount,
                                           const Paddleboat_Controller_Mapping_Data *mappingData);

        static int32_t getControllerRemapTableData(const int32_t destRemapTableEntryCount,
                                                   Paddleboat_Controller_Mapping_Data *mappingData);

        // Called from the JNI bridge functions
        static GameControllerDeviceInfo *onConnection();

        static void onDisconnection(const int32_t deviceId);

        static void onMouseConnection(const int32_t deviceId);

        static void onMouseDisconnection(const int32_t deviceId);

        static jclass getGameControllerClass();

        static jobject getGameControllerObject();

        // device debug helper function
        static int32_t getLastKeycode();

    private:
        static GameControllerManager *getInstance();

        int32_t processControllerKeyEvent(const AInputEvent *event,
                                          GameController &gameController);

        int32_t processControllerGameActivityKeyEvent(const Paddleboat_GameActivityKeyEvent *event,
                                                      const size_t eventSize,
                                                      GameController &gameController);

        int32_t processMouseEvent(const AInputEvent *event);

        int32_t processGameActivityMouseEvent(const Paddleboat_GameActivityEvent eventType,
                                              const void *event,
                                              const size_t eventSize, const int32_t eventDeviceId);

        void rescanVirtualMouseControllers();

        const Paddleboat_Controller_Mapping_Data *
        getMapForController(const GameController &gameController);

        bool mInitialized = false;
        bool mGCMClassInitialized = false;
        bool mBackButtonConsumed = true;

        int32_t mApiLevel = 16;
        JNIEnv *mJNIEnv = nullptr;
        jobject mActivity = NULL;
        jclass mGameControllerClass = NULL;
        jobject mGameControllerObject = NULL;
        jmethodID mGetApiLevelMethodId = NULL;
        jmethodID mSetVibrationMethodId = NULL;

        uint64_t mActiveAxisMask = 0;

        int32_t mRemapEntryCount = 0;
        Paddleboat_Controller_Mapping_Data mMappingTable[MAX_REMAP_TABLE_SIZE];

        GameController mGameControllers[PADDLEBOAT_MAX_CONTROLLERS];
        Paddleboat_ControllerStatusCallback mStatusCallback = nullptr;
        // device debug helper
        int32_t mLastKeyEventKeyCode = 0;

        Paddleboat_MouseStatus mMouseStatus = PADDLEBOAT_MOUSE_NONE;
        int32_t mMouseDeviceIds[MAX_MOUSE_DEVICES] = {INVALID_MOUSE_ID, INVALID_MOUSE_ID};
        int32_t mMouseControllerIndex = INVALID_MOUSE_ID;
        Paddleboat_Mouse_Data mMouseData;
        Paddleboat_MouseStatusCallback mMouseCallback = nullptr;

        static std::mutex sInstanceMutex;
        static std::mutex sUpdateMutex;
        static std::unique_ptr<GameControllerManager> sInstance GUARDED_BY(sInstanceMutex);
    };
}
