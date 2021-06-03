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
#include "GameControllerManager.h"

#include <cstdlib>
#include <memory>
#include <android/api-level.h>

#include "GameControllerInternalConstants.h"
#include "GameControllerLog.h"
#include "GameControllerMappingUtils.h"
#include "InternalControllerTable.h"
#include "Log.h"

#define LOG_TAG "GameControllerManager"
// If defined, output information about qualifying game controller
// event data to log
//#define LOG_INPUT_EVENTS

namespace paddleboat {
    constexpr const char *CLASSLOADER_CLASS = "java/lang/ClassLoader";
    constexpr const char *CLASSLOADER_GETCLASSLOADER_METHOD_NAME = "getClassLoader";
    constexpr const char *CLASSLOADER_GETCLASSLOADER_METHOD_SIG = "()Ljava/lang/ClassLoader;";
    constexpr const char *CLASSLOADER_LOADCLASS_METHOD_NAME = "loadClass";
    constexpr const char *CLASSLOADER_LOADCLASS_METHOD_SIG =
        "(Ljava/lang/String;)Ljava/lang/Class;";

    constexpr const char *GCM_CLASSNAME =
        "com/google/android/games/paddleboat/GameControllerManager";
    constexpr const char *GCM_INIT_METHOD_NAME = "<init>";
    constexpr const char *GCM_INIT_METHOD_SIGNATURE =
        "(Landroid/app/Activity;Ljava/lang/String;Z)V";
    constexpr const char *GCM_ONPAUSE_METHOD_NAME = "onPause";
    constexpr const char *GCM_ONRESUME_METHOD_NAME = "onResume";
    constexpr const char *GCM_GETAPILEVEL_METHOD_NAME = "getApiLevel";
    constexpr const char *GCM_GETAPILEVEL_METHOD_SIGNATURE = "()I";
    constexpr const char *GCM_SETNATIVEREADY_METHOD_NAME = "setNativeReady";
    constexpr const char *GCM_SETVIBRATION_METHOD_NAME = "setVibration";
    constexpr const char *GCM_SETVIBRATION_METHOD_SIGNATURE = "(IIIII)V";

    constexpr const char *VOID_METHOD_SIGNATURE = "()V";

    constexpr float VIBRATION_INTENSITY_SCALE = 255.0f;

    std::mutex GameControllerManager::sInstanceMutex;
    std::mutex GameControllerManager::sUpdateMutex;
    std::unique_ptr<GameControllerManager> GameControllerManager::sInstance;

    bool GameControllerManager::init(JNIEnv *env, jobject jactivity, const char *libraryName) {
        std::lock_guard<std::mutex> lock(sInstanceMutex);
        if (sInstance) {
            ALOGE("Attempted to initialize Paddleboat twice");
            return false;
        }

        sInstance = std::make_unique<GameControllerManager>(env, jactivity, ConstructorTag{});
        if (!sInstance->mInitialized) {
            ALOGE("Failed to initialize Paddleboat");
            return false;
        }

        // Load our GameControllerManager class
        jclass activityClass = env->GetObjectClass(jactivity);
        jmethodID getClassLoaderID = env->GetMethodID(activityClass,
                                                      CLASSLOADER_GETCLASSLOADER_METHOD_NAME,
                                                      CLASSLOADER_GETCLASSLOADER_METHOD_SIG);
        jobject classLoaderObject = env->CallObjectMethod(jactivity, getClassLoaderID);
        jclass classLoader = env->FindClass(CLASSLOADER_CLASS);
        jmethodID loadClassID = env->GetMethodID(classLoader, CLASSLOADER_LOADCLASS_METHOD_NAME,
                                                 CLASSLOADER_LOADCLASS_METHOD_SIG);
        jstring gcmClassName = env->NewStringUTF(GCM_CLASSNAME);
        jclass gcmClass = jclass(
                env->CallObjectMethod(classLoaderObject, loadClassID, gcmClassName));
        if (gcmClass == NULL) {
            ALOGE("Failed to find GameControllerManager class");
            return false;
        }
        jclass global_gcmClass = static_cast<jclass>(env->NewGlobalRef(gcmClass));

        // Create a GameControllerManager and initialize it, saving global references to its
        // class and the instantiated object
        jmethodID gcmInitMethod = env->GetMethodID(global_gcmClass, GCM_INIT_METHOD_NAME,
                                                   GCM_INIT_METHOD_SIGNATURE);
        if (gcmInitMethod == NULL) {
            ALOGE("Failed to find GameControllerManager init method");
            return false;
        }

        jmethodID getApiLevelMethodId = env->GetMethodID(global_gcmClass,
                                                         GCM_GETAPILEVEL_METHOD_NAME,
                                                         GCM_GETAPILEVEL_METHOD_SIGNATURE);
        if (getApiLevelMethodId == NULL) {
            ALOGE("Failed to find GameControllerManager getApiLevel method");
            return false;
        }
        jmethodID setVibrationMethodId = env->GetMethodID(global_gcmClass,
                                                          GCM_SETVIBRATION_METHOD_NAME,
                                                          GCM_SETVIBRATION_METHOD_SIGNATURE);
        if (setVibrationMethodId == NULL) {
            ALOGE("Failed to find GameControllerManager setVibration method");
            return false;
        }

#if _DEBUG
        jboolean printControllerInfo = JNI_TRUE;
#else
        jboolean printControllerInfo = JNI_FALSE;
#endif
        jstring libraryNameString = NULL;
        if (libraryName != nullptr) {
            libraryNameString = env->NewStringUTF(libraryName);
        }

        jobject gcmObject = env->NewObject(global_gcmClass, gcmInitMethod, jactivity,
                                           libraryNameString, printControllerInfo);
        if (gcmObject == NULL) {
            ALOGE("Failed to create GameControllerManager");
            return false;
        }
        jobject global_gcmObject = env->NewGlobalRef(gcmObject);
        env->DeleteLocalRef(gcmObject);

        if (libraryNameString != NULL) {
            env->DeleteLocalRef(libraryNameString);
        }

        GameControllerManager *gcm = sInstance.get();
        if (gcm) {
            gcm->mGameControllerClass = global_gcmClass;
            gcm->mGameControllerObject = global_gcmObject;
            gcm->mGetApiLevelMethodId = getApiLevelMethodId;
            gcm->mSetVibrationMethodId = setVibrationMethodId;
        }

        return true;
    }

    void GameControllerManager::destroyInstance() {
        std::lock_guard<std::mutex> lock(sInstanceMutex);
        sInstance.reset();
    }

    GameControllerManager *GameControllerManager::getInstance() {
        std::lock_guard<std::mutex> lock(sInstanceMutex);
        return sInstance.get();
    }

    bool GameControllerManager::isEnabled() {
        GameControllerManager *gcm = getInstance();
        if (!gcm) {
            // This is a case of error.
            // We do not log anything here, so that we do not spam
            // the user when this function is called each frame.
            return false;
        }
        return gcm->mGCMClassInitialized;
    }

    GameControllerManager::GameControllerManager(JNIEnv *env, jobject jactivity, ConstructorTag) {
        mStatusCallback = nullptr;
        mJNIEnv = env;
        mActivity = jactivity;
        mMouseData.buttonsDown = 0;
        mMouseData.mouseScrollDeltaH = 0;
        mMouseData.mouseScrollDeltaV = 0;
        mMouseData.mouseX = 0.0f;
        mMouseData.mouseY = 0.0f;
        mInitialized = true;
        memset(mMappingTable, 0, sizeof(mMappingTable));
        mRemapEntryCount = PB_getInternalControllerDataCount();
        const Paddleboat_Controller_Mapping_Data *mappingData = PB_getInternalControllerData();
        const size_t copySize = mRemapEntryCount * sizeof(Paddleboat_Controller_Mapping_Data);
        memcpy(mMappingTable, mappingData, copySize);
        // Our minimum supported API level, we will retrieve the actual runtime API
        // level later on calling getApiLevel
        mApiLevel = 16;
    }

    GameControllerManager::~GameControllerManager() {
        if (mGameControllerClass != NULL) {
            mJNIEnv->DeleteGlobalRef(mGameControllerClass);
        }
        if (mGameControllerObject != NULL) {
            mJNIEnv->DeleteGlobalRef(mGameControllerObject);
        }
        mInitialized = false;
    }

    int32_t GameControllerManager::processInputEvent(const AInputEvent *event) {
        int32_t handledEvent = IGNORED_EVENT;
        if (event != nullptr) {
            std::lock_guard<std::mutex> lock(sUpdateMutex);
            GameControllerManager *gcm = getInstance();
            if (gcm) {
                const int32_t eventSource = AInputEvent_getSource(event);
                const int32_t dpadSource = eventSource & AINPUT_SOURCE_DPAD;
                const int32_t gamepadSource = eventSource & AINPUT_SOURCE_GAMEPAD;
                const int32_t joystickSource = eventSource & AINPUT_SOURCE_JOYSTICK;
                const int32_t mouseSource = eventSource & AINPUT_SOURCE_MOUSE;
                if (dpadSource == AINPUT_SOURCE_DPAD || gamepadSource == AINPUT_SOURCE_GAMEPAD ||
                    joystickSource == AINPUT_SOURCE_JOYSTICK) {
                    const int32_t eventDeviceId = AInputEvent_getDeviceId(event);
                    for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
                        if (gcm->mGameControllers[i].getConnectionIndex() >= 0) {
                            if (gcm->mGameControllers[i].getControllerStatus() ==
                                PADDLEBOAT_CONTROLLER_ACTIVE) {
                                const GameControllerDeviceInfo &deviceInfo =
                                    gcm->mGameControllers[i].getDeviceInfo();
                                if (deviceInfo.getInfo().mDeviceId == eventDeviceId) {
                                    const int32_t eventType = AInputEvent_getType(event);
                                    if (eventType == AINPUT_EVENT_TYPE_KEY) {
                                        handledEvent = gcm->processControllerKeyEvent(event,
                                            gcm->mGameControllers[i]);
                                    } else if (eventType == AINPUT_EVENT_TYPE_MOTION) {
                                        handledEvent = gcm->mGameControllers[i].processMotionEvent(
                                                event);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                } else if (mouseSource == AINPUT_SOURCE_MOUSE) {
                    handledEvent = gcm->processMouseEvent(event);
                }
#if defined LOG_INPUT_EVENTS
                paddleboat_LogInputEvent(event);
#endif
            }
        }
        return handledEvent;
    }

    int32_t GameControllerManager::processGameActivityInputEvent(
            const Paddleboat_GameActivityEvent eventType, const void *event,
            const size_t eventSize) {
        int32_t handledEvent = IGNORED_EVENT;
        if (event != nullptr) {
            std::lock_guard<std::mutex> lock(sUpdateMutex);
            GameControllerManager *gcm = getInstance();
            if (gcm) {
                const int32_t eventSource = PBGameActivityUtil_getEventSource(eventType, event,
                                                                              eventSize);
                const int32_t eventDeviceId = PBGameActivityUtil_getEventDeviceId(eventType, event,
                                                                                  eventSize);
                const int32_t dpadSource = eventSource & AINPUT_SOURCE_DPAD;
                const int32_t gamepadSource = eventSource & AINPUT_SOURCE_GAMEPAD;
                const int32_t joystickSource = eventSource & AINPUT_SOURCE_JOYSTICK;
                const int32_t mouseSource = eventSource & AINPUT_SOURCE_MOUSE;
                if (dpadSource == AINPUT_SOURCE_DPAD || gamepadSource == AINPUT_SOURCE_GAMEPAD ||
                    joystickSource == AINPUT_SOURCE_JOYSTICK) {
                    for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
                        if (gcm->mGameControllers[i].getConnectionIndex() >= 0) {
                            if (gcm->mGameControllers[i].getControllerStatus() ==
                                PADDLEBOAT_CONTROLLER_ACTIVE) {
                                const GameControllerDeviceInfo &deviceInfo =
                                    gcm->mGameControllers[i].getDeviceInfo();
                                if (deviceInfo.getInfo().mDeviceId == eventDeviceId) {
                                    if (eventType == PADDLEBOAT_GAMEACTIVITY_EVENT_KEY) {
                                        const Paddleboat_GameActivityKeyEvent *keyEvent =
                                            reinterpret_cast<const
                                            Paddleboat_GameActivityKeyEvent *>(event);
                                        handledEvent = gcm->processControllerGameActivityKeyEvent(
                                                keyEvent, eventSize, gcm->mGameControllers[i]);
                                    } else if (eventType == PADDLEBOAT_GAMEACTIVITY_EVENT_MOTION) {
                                        const Paddleboat_GameActivityMotionEvent *motionEvent =
                                            reinterpret_cast<const
                                            Paddleboat_GameActivityMotionEvent *>(event);
                                        handledEvent =
                                        gcm->mGameControllers[i].processGameActivityMotionEvent(
                                                motionEvent, eventSize);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                } else if (mouseSource == AINPUT_SOURCE_MOUSE) {
                    handledEvent = gcm->processGameActivityMouseEvent(eventType, event, eventSize,
                                                                      eventDeviceId);
                }
            }
        }
        return handledEvent;
    }

    int32_t GameControllerManager::processControllerKeyEvent(const AInputEvent *event,
                                                             GameController &gameController) {
        int32_t handledEvent = HANDLED_EVENT;
        const int32_t eventKeycode = AKeyEvent_getKeyCode(event);

        mLastKeyEventKeyCode = eventKeycode;
        if (eventKeycode == AKEYCODE_BACK) {
            if (!mBackButtonConsumed) {
                handledEvent = IGNORED_EVENT;
            }
        } else {
            handledEvent = gameController.processKeyEvent(event);
        }
        return handledEvent;
    }

    int32_t GameControllerManager::processControllerGameActivityKeyEvent(
            const Paddleboat_GameActivityKeyEvent *event, const size_t eventSize,
            GameController &gameController) {
        int32_t handledEvent = HANDLED_EVENT;
        const int32_t eventKeyCode = event->keyCode;

        mLastKeyEventKeyCode = eventKeyCode;
        if (eventKeyCode == AKEYCODE_BACK) {
            if (!mBackButtonConsumed) {
                handledEvent = IGNORED_EVENT;
            }
        } else {
            handledEvent = gameController.processGameActivityKeyEvent(event, eventSize);
        }
        return handledEvent;
    }

    int32_t GameControllerManager::processMouseEvent(const AInputEvent *event) {
        int32_t handledEvent = IGNORED_EVENT;
        const int32_t eventDeviceId = AInputEvent_getDeviceId(event);
        const int32_t eventType = AInputEvent_getType(event);

        // Always update the virtual pointer data in the appropriate controller data structures
        if (eventType == AINPUT_EVENT_TYPE_MOTION) {
            for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
                if (mGameControllers[i].getConnectionIndex() >= 0) {
                    if (mGameControllers[i].getControllerStatus() ==
                        PADDLEBOAT_CONTROLLER_ACTIVE) {
                        const Paddleboat_Controller_Info &controllerInfo =
                            mGameControllers[i].getControllerInfo();
                        if (controllerInfo.deviceId == eventDeviceId) {
                            Paddleboat_Controller_Data &controllerData =
                                mGameControllers[i].getControllerData();
                            controllerData.virtualX = AMotionEvent_getAxisValue(event,
                                AMOTION_EVENT_AXIS_X, 0);
                            controllerData.virtualY = AMotionEvent_getAxisValue(event,
                                AMOTION_EVENT_AXIS_Y, 0);
                            const float axisP = AMotionEvent_getAxisValue(event,
                                AMOTION_EVENT_AXIS_PRESSURE, 0);

                            const bool hasTouchpadButton = ((controllerInfo.controllerFlags &
                                                             PADDLEBOAT_CONTROLLER_FLAG_TOUCHPAD)
                                                             != 0);
                            if (hasTouchpadButton) {
                                if (axisP > 0.0f) {
                                    controllerData.buttonsDown |= PADDLEBOAT_BUTTON_TOUCHPAD;
                                } else {
                                    controllerData.buttonsDown &= (~PADDLEBOAT_BUTTON_TOUCHPAD);
                                }
                            }
                            mGameControllers[i].setControllerDataDirty(true);

                            // If this controller is our 'active' virtual mouse,
                            // update the mouse data
                            if (mMouseStatus == PADDLEBOAT_MOUSE_CONTROLLER_EMULATED
                                && mMouseControllerIndex == static_cast<int32_t>(i)) {
                                mMouseData.mouseX = controllerData.virtualX;
                                mMouseData.mouseY = controllerData.virtualY;
                                mMouseData.buttonsDown =
                                    static_cast<uint32_t>(AMotionEvent_getButtonState(event));
                                mMouseData.buttonsDown |= axisP > 0.0f ? 1 : 0;
                            }
                            break;
                        }
                    }
                }
            }
        }

        if (mMouseStatus == PADDLEBOAT_MOUSE_PHYSICAL) {
            for (size_t i = 0; i < MAX_MOUSE_DEVICES; ++i) {
                if (mMouseDeviceIds[i] == eventDeviceId) {
                    if (eventType == AINPUT_EVENT_TYPE_MOTION) {
                        mMouseData.mouseX = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X,
                            0);
                        mMouseData.mouseY = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y,
                            0);
                        const int32_t buttonState = AMotionEvent_getButtonState(event);
                        mMouseData.buttonsDown = static_cast<uint32_t>(buttonState);
                        const float axisHScroll = AMotionEvent_getAxisValue(event,
                            AMOTION_EVENT_AXIS_HSCROLL, 0);
                        const float axisVScroll = AMotionEvent_getAxisValue(event,
                            AMOTION_EVENT_AXIS_VSCROLL, 0);
                        // These are treated as cumulative deltas and reset when the
                        // calling application requests the mouse data.
                        mMouseData.mouseScrollDeltaH += static_cast<int32_t>(axisHScroll);
                        mMouseData.mouseScrollDeltaV += static_cast<int32_t>(axisVScroll);
                    }
                    handledEvent = HANDLED_EVENT;
                    break;
                }
            }
        }

        return handledEvent;
    }

    int32_t GameControllerManager::processGameActivityMouseEvent(
            const Paddleboat_GameActivityEvent eventType, const void *event,
            const size_t eventSize, const int32_t eventDeviceId) {
        int32_t handledEvent = IGNORED_EVENT;

        // Always update the virtual pointer data in the appropriate controller data structures
        if (eventType == PADDLEBOAT_GAMEACTIVITY_EVENT_MOTION) {
            const Paddleboat_GameActivityMotionEvent *motionEvent =
                reinterpret_cast<const Paddleboat_GameActivityMotionEvent *>(event);
            if (motionEvent->pointerCount > 0 && motionEvent->pointers != nullptr) {
                const Paddleboat_GameActivityPointerInfo *pointerInfo = motionEvent->pointers;
                for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
                    if (mGameControllers[i].getConnectionIndex() >= 0) {
                        if (mGameControllers[i].getControllerStatus() ==
                            PADDLEBOAT_CONTROLLER_ACTIVE) {
                            const Paddleboat_Controller_Info &controllerInfo =
                                mGameControllers[i].getControllerInfo();
                            if (controllerInfo.deviceId == eventDeviceId) {
                                Paddleboat_Controller_Data &controllerData =
                                    mGameControllers[i].getControllerData();
                                controllerData.virtualX =
                                    pointerInfo->axisValues[AMOTION_EVENT_AXIS_X];
                                controllerData.virtualY =
                                    pointerInfo->axisValues[AMOTION_EVENT_AXIS_Y];
                                const float axisP =
                                    pointerInfo->axisValues[AMOTION_EVENT_AXIS_PRESSURE];

                                const bool hasTouchpadButton = ((controllerInfo.controllerFlags &
                                    PADDLEBOAT_CONTROLLER_FLAG_TOUCHPAD) != 0);
                                if (hasTouchpadButton) {
                                    if (axisP > 0.0f) {
                                        controllerData.buttonsDown |= PADDLEBOAT_BUTTON_TOUCHPAD;
                                    } else {
                                        controllerData.buttonsDown &=
                                            (~PADDLEBOAT_BUTTON_TOUCHPAD);
                                    }
                                }
                                mGameControllers[i].setControllerDataDirty(true);

                                // If this controller is our 'active' virtual mouse,
                                // update the mouse data
                                if (mMouseStatus == PADDLEBOAT_MOUSE_CONTROLLER_EMULATED
                                    && mMouseControllerIndex == static_cast<int32_t>(i)) {
                                    mMouseData.mouseX = controllerData.virtualX;
                                    mMouseData.mouseY = controllerData.virtualY;
                                    mMouseData.buttonsDown = motionEvent->buttonState;
                                    mMouseData.buttonsDown |= axisP > 0.0f ? 1 : 0;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (mMouseStatus == PADDLEBOAT_MOUSE_PHYSICAL) {
            for (size_t i = 0; i < MAX_MOUSE_DEVICES; ++i) {
                if (mMouseDeviceIds[i] == eventDeviceId) {
                    if (eventType == PADDLEBOAT_GAMEACTIVITY_EVENT_MOTION) {
                        const Paddleboat_GameActivityMotionEvent *motionEvent =
                            reinterpret_cast<const Paddleboat_GameActivityMotionEvent *>(event);
                        if (motionEvent->pointerCount > 0 && motionEvent->pointers != nullptr) {
                            const Paddleboat_GameActivityPointerInfo *pointerInfo =
                                motionEvent->pointers;

                            mMouseData.mouseX = pointerInfo->axisValues[AMOTION_EVENT_AXIS_X];
                            mMouseData.mouseY = pointerInfo->axisValues[AMOTION_EVENT_AXIS_Y];
                            const int32_t buttonState = motionEvent->buttonState;
                            mMouseData.buttonsDown = static_cast<uint32_t>(buttonState);
                            const float axisHScroll =
                                pointerInfo->axisValues[AMOTION_EVENT_AXIS_HSCROLL];
                            const float axisVScroll =
                                pointerInfo->axisValues[AMOTION_EVENT_AXIS_VSCROLL];
                            // These are treated as cumulative deltas and reset when the
                            // calling application requests the mouse data.
                            mMouseData.mouseScrollDeltaH += static_cast<int32_t>(axisHScroll);
                            mMouseData.mouseScrollDeltaV += static_cast<int32_t>(axisVScroll);
                        }
                        handledEvent = HANDLED_EVENT;
                        break;
                    }
                }
            }
        }

        return handledEvent;
    }

    uint64_t GameControllerManager::getActiveAxisMask() {
        uint64_t returnMask = 0;
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            returnMask = gcm->mActiveAxisMask;
        }
        return returnMask;
    }

    void GameControllerManager::update() {
        GameControllerManager *gcm = getInstance();
        if (!gcm) {
            return;
        }
        if (!gcm->mGCMClassInitialized && gcm->mGameControllerObject != NULL) {
            gcm->mApiLevel = gcm->mJNIEnv->CallIntMethod(gcm->mGameControllerObject,
                                                         gcm->mGetApiLevelMethodId);

            // Tell the GCM class we are ready to receive information about controllers
            gcm->mGCMClassInitialized = true;
            jmethodID setNativeReady = gcm->mJNIEnv->GetMethodID(gcm->mGameControllerClass,
                                                                 GCM_SETNATIVEREADY_METHOD_NAME,
                                                                 VOID_METHOD_SIGNATURE);
            if (setNativeReady != NULL) {
                gcm->mJNIEnv->CallVoidMethod(gcm->mGameControllerObject, setNativeReady);
            }
        }

        std::lock_guard<std::mutex> lock(sUpdateMutex);

        // Process pending connections/disconnections
        if (gcm->mStatusCallback != nullptr) {
            for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
                if (gcm->mGameControllers[i].getConnectionIndex() >= 0) {
                    if (gcm->mGameControllers[i].getControllerStatus() ==
                        PADDLEBOAT_CONTROLLER_JUST_CONNECTED) {
                        // Look for a mapping table entry for this controller device,
                        // if one does not exist, nullptr is passed and GameController
                        // will fallback to default axis and button mapping.
                        const Paddleboat_Controller_Mapping_Data *mapData =
                            gcm->getMapForController(gcm->mGameControllers[i]);
#if defined LOG_INPUT_EVENTS
                        if (mapData != nullptr) {
                            ALOGI("Found controller map for vId/pId: %x %x",
                                  gcm->mGameControllers[i].getDeviceInfo().getInfo()->mVendorId,
                                  gcm->mGameControllers[i].getDeviceInfo().getInfo()->mProductId);
                        } else {
                            ALOGI("No controller map found, using defaults for vId/pId: %x %x",
                                  gcm->mGameControllers[i].getDeviceInfo().getInfo()->mVendorId,
                                  gcm->mGameControllers[i].getDeviceInfo().getInfo()->mProductId);
                        }
#endif
                        gcm->mGameControllers[i].setupController(mapData);
                        // Update the active axis mask to include any new axis used by the
                        // new controller
                        gcm->mActiveAxisMask |= gcm->mGameControllers[i].getControllerAxisMask();
                        gcm->mGameControllers[i].setControllerStatus(PADDLEBOAT_CONTROLLER_ACTIVE);
                        if (gcm->mStatusCallback != nullptr) {
#if defined LOG_INPUT_EVENTS
                            ALOGI("statusCallback PADDLEBOAT_CONTROLLER_JUST_CONNECTED on %d",
                                  static_cast<int>(i));
#endif
                            gcm->mStatusCallback(i, PADDLEBOAT_CONTROLLER_JUST_CONNECTED);
                        }
                        gcm->rescanVirtualMouseControllers();
                    } else if (gcm->mGameControllers[i].getControllerStatus() ==
                               PADDLEBOAT_CONTROLLER_JUST_DISCONNECTED) {
                        gcm->mGameControllers[i].setControllerStatus(
                                PADDLEBOAT_CONTROLLER_INACTIVE);
                        // free the controller for reuse
                        gcm->mGameControllers[i].setConnectionIndex(-1);
                        if (gcm->mStatusCallback != nullptr) {
#if defined LOG_INPUT_EVENTS
                            ALOGI("statusCallback PADDLEBOAT_CONTROLLER_JUST_DISCONNECTED on %d",
                                  static_cast<int>(i));
#endif
                            gcm->mStatusCallback(i, PADDLEBOAT_CONTROLLER_JUST_DISCONNECTED);
                        }
                        gcm->rescanVirtualMouseControllers();
                    }
                }
            }
        }
    }

    bool GameControllerManager::getControllerData(const int32_t controllerIndex,
                                                  Paddleboat_Controller_Data *controllerData) {
        bool success = false;
        if (controllerData != nullptr) {
            if (controllerIndex >= 0 && controllerIndex < PADDLEBOAT_MAX_CONTROLLERS) {
                GameControllerManager *gcm = getInstance();
                if (gcm) {
                    if (gcm->mGameControllers[controllerIndex].getConnectionIndex() ==
                        controllerIndex) {
                        if (gcm->mGameControllers[controllerIndex].getControllerDataDirty()) {
                            gcm->mGameControllers[controllerIndex].setControllerDataDirty(false);
                        }
                        memcpy(controllerData,
                               &gcm->mGameControllers[controllerIndex].getControllerData(),
                               sizeof(Paddleboat_Controller_Data));
                        success = true;
                    }
                }
            }
        }
        return success;
    }

    bool GameControllerManager::getControllerInfo(const int32_t controllerIndex,
                                                  Paddleboat_Controller_Info *controllerInfo) {
        bool success = false;
        if (controllerInfo != nullptr) {
            if (controllerIndex >= 0 && controllerIndex < PADDLEBOAT_MAX_CONTROLLERS) {
                GameControllerManager *gcm = getInstance();
                if (gcm) {
                    if (gcm->mGameControllers[controllerIndex].getConnectionIndex() ==
                        controllerIndex) {
                        memcpy(controllerInfo,
                               &gcm->mGameControllers[controllerIndex].getControllerInfo(),
                               sizeof(Paddleboat_Controller_Info));
                        success = true;
                    }
                }
            }
        }
        return success;
    }

    Paddleboat_ControllerStatus
    GameControllerManager::getControllerStatus(const int32_t controllerIndex) {
        Paddleboat_ControllerStatus controllerStatus = PADDLEBOAT_CONTROLLER_INACTIVE;
        if (controllerIndex >= 0 && controllerIndex < PADDLEBOAT_MAX_CONTROLLERS) {
            GameControllerManager *gcm = getInstance();
            if (gcm) {
                controllerStatus = gcm->mGameControllers[controllerIndex].getControllerStatus();
            }
        }
        return controllerStatus;
    }

    bool GameControllerManager::setControllerVibrationData(const int32_t controllerIndex,
        const Paddleboat_Vibration_Data *vibrationData) {
        bool success = false;
        if (vibrationData != nullptr) {
            if (controllerIndex >= 0 && controllerIndex < PADDLEBOAT_MAX_CONTROLLERS) {
                GameControllerManager *gcm = getInstance();
                if (gcm) {
                    if (gcm->mGameControllers[controllerIndex].getConnectionIndex() ==
                        controllerIndex) {
                        const Paddleboat_Controller_Info &controllerInfo =
                            gcm->mGameControllers[controllerIndex].getControllerInfo();
                        if ((controllerInfo.controllerFlags &
                             PADDLEBOAT_CONTROLLER_FLAG_VIBRATION) != 0) {
                            if (gcm->mGameControllerObject != NULL &&
                                gcm->mSetVibrationMethodId != NULL) {
                                const jint intensityLeft = static_cast<jint>(
                                        vibrationData->intensityLeft * VIBRATION_INTENSITY_SCALE);
                                const jint intensityRight = static_cast<jint>(
                                        vibrationData->intensityRight * VIBRATION_INTENSITY_SCALE);
                                const jint durationLeft =
                                    static_cast<jint>(vibrationData->durationLeft);
                                const jint durationRight =
                                    static_cast<jint>(vibrationData->durationRight);
                                gcm->mJNIEnv->CallVoidMethod(gcm->mGameControllerObject,
                                                             gcm->mSetVibrationMethodId,
                                                             controllerInfo.deviceId,
                                                             intensityLeft,
                                                             durationLeft, intensityRight,
                                                             durationRight);
                                success = true;
                            }
                        }
                    }
                }
            }
        }
        return success;
    }

    bool GameControllerManager::setControllerLight(const int32_t controllerIndex,
                                                   const Paddleboat_LightType lightType,
                                                   const uint32_t lightData) {
        bool success = false;
        return success;
    }

    GameControllerDeviceInfo *GameControllerManager::onConnection() {
        std::lock_guard<std::mutex> lock(sUpdateMutex);
        GameControllerDeviceInfo *deviceInfo = nullptr;
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
                if (gcm->mGameControllers[i].getConnectionIndex() < 0) {
                    gcm->mGameControllers[i].setConnectionIndex(i);
                    gcm->mGameControllers[i].resetControllerData();
                    deviceInfo = &gcm->mGameControllers[i].getDeviceInfo();
                    gcm->mGameControllers[i].setControllerStatus(
                            PADDLEBOAT_CONTROLLER_JUST_CONNECTED);
#if defined LOG_INPUT_EVENTS
                    ALOGI("Setting PADDLEBOAT_CONTROLLER_JUST_CONNECTED on index %d",
                          static_cast<int>(i));
#endif
                    break;
                }
            }
        }
        return deviceInfo;
    }

    void GameControllerManager::onDisconnection(const int32_t deviceId) {
        std::lock_guard<std::mutex> lock(sUpdateMutex);
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
                if (gcm->mGameControllers[i].getConnectionIndex() >= 0) {
                    const GameControllerDeviceInfo &deviceInfo =
                        gcm->mGameControllers[i].getDeviceInfo();
                    if (deviceInfo.getInfo().mDeviceId == deviceId) {
                        gcm->mGameControllers[i].setControllerStatus(
                                PADDLEBOAT_CONTROLLER_JUST_DISCONNECTED);
#if defined LOG_INPUT_EVENTS
                        ALOGI("Setting PADDLEBOAT_CONTROLLER_JUST_DISCONNECTED on index %d",
                              static_cast<int>(i));
#endif
                    }
                }
            }
        }
    }

    bool GameControllerManager::getMouseData(Paddleboat_Mouse_Data *mouseData) {
        bool success = false;
        if (mouseData != nullptr) {
            GameControllerManager *gcm = getInstance();
            if (gcm) {
                if (gcm->mMouseStatus != PADDLEBOAT_MOUSE_NONE) {
                    memcpy(mouseData,
                           &gcm->mMouseData,
                           sizeof(Paddleboat_Mouse_Data));
                    // We reset the scroll wheel(s) values after each read
                    gcm->mMouseData.mouseScrollDeltaH = 0;
                    gcm->mMouseData.mouseScrollDeltaV = 0;
                    success = true;
                }
            }
        }
        return success;
    }

    Paddleboat_MouseStatus GameControllerManager::getMouseStatus() {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            return gcm->mMouseStatus;
        }
        return PADDLEBOAT_MOUSE_NONE;
    }

    void GameControllerManager::onMouseConnection(const int32_t deviceId) {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            for (size_t i = 0; i < MAX_MOUSE_DEVICES; ++i) {
                if (gcm->mMouseDeviceIds[i] == INVALID_MOUSE_ID) {
                    gcm->mMouseDeviceIds[i] = deviceId;
                    break;
                }
            }
            if (gcm->mMouseStatus != PADDLEBOAT_MOUSE_PHYSICAL) {
                gcm->mMouseStatus = PADDLEBOAT_MOUSE_PHYSICAL;
                if (gcm->mMouseCallback != nullptr) {
                    gcm->mMouseCallback(gcm->mMouseStatus);
                }
            }
        }
    }

    void GameControllerManager::onMouseDisconnection(const int32_t deviceId) {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            int mouseDeviceCount = 0;
            for (size_t i = 0; i < MAX_MOUSE_DEVICES; ++i) {
                if (gcm->mMouseDeviceIds[i] == deviceId) {
                    gcm->mMouseDeviceIds[i] = INVALID_MOUSE_ID;
                } else if (gcm->mMouseDeviceIds[i] != INVALID_MOUSE_ID) {
                    ++mouseDeviceCount;
                }
            }

            // If no other physical mice are connected, see if we downgrade to
            // a controller virtual mouse or no mouse at all
            if (mouseDeviceCount == 0) {
                gcm->mMouseStatus = (gcm->mMouseControllerIndex == INVALID_MOUSE_ID)
                                    ? PADDLEBOAT_MOUSE_NONE : PADDLEBOAT_MOUSE_CONTROLLER_EMULATED;
                if (gcm->mMouseCallback != nullptr) {
                    gcm->mMouseCallback(gcm->mMouseStatus);
                }
            }
        }
    }

    // Make sure the 'virtual' mouse is the first active controller index
    // which supports a virtual pointer. If no mouse is currently active,
    // upgrade to a virtual mouse and send a mouse status callback.
    // If only a virtual mouse was active, and it vanished, set no mouse and
    // send a mouse status callback.
    void GameControllerManager::rescanVirtualMouseControllers() {
        mMouseControllerIndex = INVALID_MOUSE_ID;
        for (size_t i = 0; i < PADDLEBOAT_MAX_CONTROLLERS; ++i) {
            if (mGameControllers[i].getControllerStatus() == PADDLEBOAT_CONTROLLER_ACTIVE) {
                if ((mGameControllers[i].getControllerInfo().controllerFlags &
                     PADDLEBOAT_CONTROLLER_FLAG_VIRTUAL_MOUSE) != 0) {
                    mMouseControllerIndex = i;
                    if (mMouseStatus == PADDLEBOAT_MOUSE_NONE) {
                        mMouseStatus = PADDLEBOAT_MOUSE_CONTROLLER_EMULATED;
                        if (mMouseCallback != nullptr) {
                            mMouseCallback(mMouseStatus);
                        }
                    }
                    break;
                }
            }
        }

        // If no virtual mouse exists, downgrade to no mouse and send a mouse status callback.
        if (mMouseControllerIndex == INVALID_MOUSE_ID
            && mMouseStatus == PADDLEBOAT_MOUSE_CONTROLLER_EMULATED) {
            mMouseStatus = PADDLEBOAT_MOUSE_NONE;
            if (mMouseCallback != nullptr) {
                mMouseCallback(mMouseStatus);
            }
        }
    }

    void GameControllerManager::setMouseStatusCallback(
            Paddleboat_MouseStatusCallback statusCallback) {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            gcm->mMouseCallback = statusCallback;
        }
    }

    jclass GameControllerManager::getGameControllerClass() {
        GameControllerManager *gcm = getInstance();
        if (!gcm) {
            return NULL;
        }
        return gcm->mGameControllerClass;
    }

    jobject GameControllerManager::getGameControllerObject() {
        GameControllerManager *gcm = getInstance();
        if (!gcm) {
            return NULL;
        }
        return gcm->mGameControllerObject;
    }

    bool GameControllerManager::getBackButtonConsumed() {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            return gcm->mBackButtonConsumed;
        }
        return false;
    }

    void GameControllerManager::setBackButtonConsumed(bool consumed) {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            gcm->mBackButtonConsumed = consumed;
        }
    }

    void GameControllerManager::setControllerStatusCallback(
            Paddleboat_ControllerStatusCallback statusCallback) {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            gcm->mStatusCallback = statusCallback;
        }
    }

// device debug helper function
    int32_t GameControllerManager::getLastKeycode() {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            return gcm->mLastKeyEventKeyCode;
        }
        return 0;
    }

    void GameControllerManager::onPause() {
        GameControllerManager *gcm = getInstance();
        if (!gcm) {
            return;
        }
        if (gcm->mGameControllerObject != NULL) {
            jmethodID onPauseID = gcm->mJNIEnv->GetMethodID(gcm->mGameControllerClass,
                                                            GCM_ONPAUSE_METHOD_NAME,
                                                            VOID_METHOD_SIGNATURE);
            if (onPauseID != NULL) {
                gcm->mJNIEnv->CallVoidMethod(gcm->mGameControllerObject, onPauseID);
            }
        }
    }

    void GameControllerManager::onResume() {
        GameControllerManager *gcm = getInstance();
        if (!gcm) {
            return;
        }
        if (gcm->mGameControllerObject != NULL) {
            jmethodID onResumeID = gcm->mJNIEnv->GetMethodID(gcm->mGameControllerClass,
                                                             GCM_ONRESUME_METHOD_NAME,
                                                             VOID_METHOD_SIGNATURE);
            if (onResumeID != NULL) {
                gcm->mJNIEnv->CallVoidMethod(gcm->mGameControllerObject, onResumeID);
            }
        }
    }

    void GameControllerManager::addControllerRemapData(const Paddleboat_Remap_Addition_Mode addMode,
        const int32_t remapTableEntryCount,
        const Paddleboat_Controller_Mapping_Data *mappingData) {
        GameControllerManager *gcm = getInstance();
        if (gcm) {
            if (addMode == PADDLEBOAT_REMAP_ADD_MODE_REPLACE_ALL) {
                gcm->mRemapEntryCount = (remapTableEntryCount < MAX_REMAP_TABLE_SIZE)
                                        ? remapTableEntryCount : MAX_REMAP_TABLE_SIZE;
                const size_t copySize =
                        gcm->mRemapEntryCount * sizeof(Paddleboat_Controller_Mapping_Data);
                memcpy(gcm->mMappingTable, mappingData, copySize);
            } else if (addMode == PADDLEBOAT_REMAP_ADD_MODE_DEFAULT) {
                for (int32_t i = 0; i < remapTableEntryCount; ++i) {
                    MappingTableSearch mapSearch(&gcm->mMappingTable[0], gcm->mRemapEntryCount);
                    mapSearch.initSearchParameters(mappingData[i].vendorId,
                                                   mappingData[i].productId,
                                                   mappingData[i].minimumEffectiveApiLevel,
                                                   mappingData[i].maximumEffectiveApiLevel);
                    GameControllerMappingUtils::findMatchingMapEntry(&mapSearch);
                    bool success = GameControllerMappingUtils::insertMapEntry(&mappingData[i],
                                                                              &mapSearch);
                    if (!success) {
                        break;
                    }
                    gcm->mRemapEntryCount += 1;
                }
            }
        }
    }

    int32_t
    GameControllerManager::getControllerRemapTableData(const int32_t destRemapTableEntryCount,
        Paddleboat_Controller_Mapping_Data *mappingData) {
        GameControllerManager *gcm = getInstance();
        if (!gcm) {
            return 0;
        }
        if (mappingData != nullptr) {
            size_t copySize = (gcm->mRemapEntryCount < destRemapTableEntryCount)
                              ? gcm->mRemapEntryCount : destRemapTableEntryCount;
            copySize *= sizeof(Paddleboat_Controller_Mapping_Data);
            memcpy(mappingData, gcm->mMappingTable, copySize);
        }
        return gcm->mRemapEntryCount;
    }

    const Paddleboat_Controller_Mapping_Data *
    GameControllerManager::getMapForController(const GameController &gameController) {
        const Paddleboat_Controller_Mapping_Data *returnMap = nullptr;
        const GameControllerDeviceInfo &deviceInfo = gameController.getDeviceInfo();
        MappingTableSearch mapSearch(&mMappingTable[0], mRemapEntryCount);
        mapSearch.initSearchParameters(deviceInfo.getInfo().mVendorId,
                                       deviceInfo.getInfo().mProductId, mApiLevel, mApiLevel);
        bool success = GameControllerMappingUtils::findMatchingMapEntry(&mapSearch);
        if (success) {
            returnMap = &mapSearch.mappingRoot[mapSearch.tableIndex];
        }
        return returnMap;
    }
}

// JNI interface functions

extern "C" {

void Java_com_google_android_games_paddleboat_GameControllerManager_onControllerConnected(
        JNIEnv *env, jobject gcmObject, jintArray deviceInfoArray,
        jfloatArray axisMinArray, jfloatArray axisMaxArray,
        jfloatArray axisFlatArray, jfloatArray axisFuzzArray) {
    paddleboat::GameControllerDeviceInfo *deviceInfo =
        paddleboat::GameControllerManager::onConnection();
    if (deviceInfo != nullptr) {
        // Copy the array contents into the DeviceInfo array equivalents
        const jsize infoArraySize = env->GetArrayLength(deviceInfoArray);
        if ((infoArraySize * sizeof(int32_t)) ==
            sizeof(paddleboat::GameControllerDeviceInfo::InfoFields)) {
            env->GetIntArrayRegion(deviceInfoArray, 0, infoArraySize,
                                   reinterpret_cast<jint *>(deviceInfo->getInfo()));
        } else {
            ALOGE("deviceInfoArray/GameControllerDeviceInfo::InfoFields size mismatch");
        }
#if defined LOG_INPUT_EVENTS
        ALOGI("onControllerConnected deviceId %d", deviceInfo->getInfo()->mDeviceId);
#endif

        // all axis arrays are presumed to be the same size
        const jsize axisArraySize = env->GetArrayLength(axisMinArray);
        if ((axisArraySize * sizeof(float)) ==
            (sizeof(float) * paddleboat::MAX_AXIS_COUNT)) {
            env->GetFloatArrayRegion(axisMinArray, 0, axisArraySize, deviceInfo->getMinArray());
            env->GetFloatArrayRegion(axisMaxArray, 0, axisArraySize, deviceInfo->getMaxArray());
            env->GetFloatArrayRegion(axisFlatArray, 0, axisArraySize,
                                     deviceInfo->getFlatArray());
            env->GetFloatArrayRegion(axisFuzzArray, 0, axisArraySize,
                                     deviceInfo->getFuzzArray());
        } else {
            ALOGE("axisArray/GameControllerDeviceInfo::axisArray size mismatch");
        }

        // Retrieve the device name string
        const jint deviceId = deviceInfo->getInfo()->mDeviceId;
        jmethodID getDeviceNameById = env->GetMethodID(
                paddleboat::GameControllerManager::getGameControllerClass(),
                "getDeviceNameById",
                "(I)Ljava/lang/String;");
        if (getDeviceNameById != NULL) {
            jstring deviceNameJstring = reinterpret_cast<jstring>(env->CallObjectMethod(
                    paddleboat::GameControllerManager::getGameControllerObject(),
                    getDeviceNameById, deviceId));
            const char *deviceName = env->GetStringUTFChars(deviceNameJstring, NULL);
            if (deviceName != nullptr) {
                deviceInfo->setName(deviceName);
            }
            env->ReleaseStringUTFChars(deviceNameJstring, deviceName);
        }
    }
}

void Java_com_google_android_games_paddleboat_GameControllerManager_onControllerDisconnected(
        JNIEnv *env, jobject gcmObject, jint deviceId) {
    paddleboat::GameControllerManager::onDisconnection(deviceId);
}

void Java_com_google_android_games_paddleboat_GameControllerManager_onMouseConnected(
        JNIEnv *env, jobject gcmObject, jint deviceId) {
    paddleboat::GameControllerManager::onMouseConnection(deviceId);
}

void Java_com_google_android_games_paddleboat_GameControllerManager_onMouseDisconnected(
        JNIEnv *env, jobject gcmObject, jint deviceId) {
    paddleboat::GameControllerManager::onMouseDisconnection(deviceId);
}

}
