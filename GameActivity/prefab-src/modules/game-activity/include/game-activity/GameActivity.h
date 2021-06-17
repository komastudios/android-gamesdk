/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup GameActivity Game Activity
 * The interface to use GameActivity.
 * @{
 */

/**
 * @file GameActivity.h
 */

#ifndef ANDROID_GAME_SDK_GAME_ACTIVITY_H
#define ANDROID_GAME_SDK_GAME_ACTIVITY_H

#include <android/asset_manager.h>
#include <android/input.h>
#include <android/native_window.h>
#include <jni.h>
#include <stdint.h>
#include <sys/types.h>

#include "game-text-input/gametextinput.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * {@link GameActivityCallbacks}
 */
struct GameActivityCallbacks;

/**
 * This structure defines the native side of an android.app.GameActivity.
 * It is created by the framework, and handed to the application's native
 * code as it is being launched.
 */
typedef struct GameActivity {
  /**
   * Pointer to the callback function table of the native application.
   * You can set the functions here to your own callbacks.  The callbacks
   * pointer itself here should not be changed; it is allocated and managed
   * for you by the framework.
   */
  struct GameActivityCallbacks* callbacks;

  /**
   * The global handle on the process's Java VM.
   */
  JavaVM* vm;

  /**
   * JNI context for the main thread of the app.  Note that this field
   * can ONLY be used from the main thread of the process; that is, the
   * thread that calls into the GameActivityCallbacks.
   */
  JNIEnv* env;

  /**
   * The GameActivity object handle.
   */
  jobject javaGameActivity;

  /**
   * Path to this application's internal data directory.
   */
  const char* internalDataPath;

  /**
   * Path to this application's external (removable/mountable) data directory.
   */
  const char* externalDataPath;

  /**
   * The platform's SDK version code.
   */
  int32_t sdkVersion;

  /**
   * This is the native instance of the application.  It is not used by
   * the framework, but can be set by the application to its own instance
   * state.
   */
  void* instance;

  /**
   * Pointer to the Asset Manager instance for the application.  The application
   * uses this to access binary assets bundled inside its own .apk file.
   */
  AAssetManager* assetManager;

  /**
   * Available starting with Honeycomb: path to the directory containing
   * the application's OBB files (if any).  If the app doesn't have any
   * OBB files, this directory may not exist.
   */
  const char* obbPath;
} GameActivity;

/**
 * The maximum number of axes supported in an Android MotionEvent.
 * See https://developer.android.com/ndk/reference/group/input.
 */
#define GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT 48

/**
 * \brief Describe information about a pointer, found in a
 * GameActivityMotionEvent.
 *
 * You can read values directly from this structure, or use helper functions
 * (`GameActivityInputInfo_getX`, `GameActivityInputInfo_getY` and
 * `GameActivityInputInfo_getAxisValue`).
 *
 * The X axis and Y axis are enabled by default but any other axis that you want
 * to read **must** be enabled first, using
 * `GameActivityInputInfo_enableAxis`.
 *
 * \see GameActivityMotionEvent
 */
typedef struct GameActivityInputInfo {
  int32_t id;
  float axisValues[GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT];
  float rawX;
  float rawY;
} GameActivityInputInfo;

/** \brief Get the current X coordinate of the pointer. */
inline float GameActivityInputInfo_getX(
    GameActivityInputInfo* pointerInfo) {
  return pointerInfo->axisValues[AMOTION_EVENT_AXIS_X];
}

/** \brief Get the current Y coordinate of the pointer. */
inline float GameActivityInputInfo_getY(
    GameActivityInputInfo* pointerInfo) {
  return pointerInfo->axisValues[AMOTION_EVENT_AXIS_Y];
}

/**
 * \brief Enable the specified axis, so that its value is reported in the
 * GameActivityInputInfo structures stored in a motion event.
 *
 * You must enable any axis that you want to read, apart from
 * `AMOTION_EVENT_AXIS_X` and `AMOTION_EVENT_AXIS_Y` that are enabled by
 * default.
 *
 * If the axis index is out of range, nothing is done.
 */
void GameActivityInputInfo_enableAxis(int32_t axis);

/**
 * \brief Disable the specified axis. Its value won't be reported in the
 * GameActivityInputInfo structures stored in a motion event anymore.
 *
 * Apart from X and Y, any axis that you want to read **must** be enabled first,
 * using `GameActivityInputInfo_enableAxis`.
 *
 * If the axis index is out of range, nothing is done.
 */
void GameActivityInputInfo_disableAxis(int32_t axis);

/**
 * \brief Get the value of the requested axis.
 *
 * Apart from X and Y, any axis that you want to read **must** be enabled first,
 * using `GameActivityInputInfo_enableAxis`.
 *
 * Find the valid enums for the axis (`AMOTION_EVENT_AXIS_X`,
 * `AMOTION_EVENT_AXIS_Y`, `AMOTION_EVENT_AXIS_PRESSURE`...)
 * in https://developer.android.com/ndk/reference/group/input.
 *
 * @param pointerInfo The structure containing information about the pointer,
 * obtained from GameActivityMotionEvent.
 * @param axis The axis to get the value from
 * @return The value of the axis, or 0 if the axis is invalid or was not
 * enabled.
 */
inline float GameActivityInputInfo_getAxisValue(
    GameActivityInputInfo* pointerInfo, int32_t axis) {
  if (axis < 0 || axis >= GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT) {
    return 0;
  }

  return pointerInfo->axisValues[axis];
}

/**
 * \brief Describe a motion event that happened on the GameActivity SurfaceView.
 *
 * This is 1:1 mapping to the information contained in a Java `MotionEvent`
 * (see https://developer.android.com/reference/android/view/MotionEvent).
 */
typedef struct GameActivityMotionEvent {
  int32_t deviceId;
  int32_t source;
  int32_t action;

  int64_t eventTime;
  int64_t downTime;

  int32_t flags;
  int32_t metaState;

  int32_t actionButton;
  int32_t buttonState;
  int32_t classification;
  int32_t edgeFlags;

  uint32_t pointerCount;
  GameActivityInputInfo* pointers;

  float precisionX;
  float precisionY;
} GameActivityMotionEvent;

/**
 * \brief Describe a key event that happened on the GameActivity SurfaceView.
 *
 * This is 1:1 mapping to the information contained in a Java `KeyEvent`
 * (see https://developer.android.com/reference/android/view/KeyEvent).
 */
typedef struct GameActivityKeyEvent {
  int32_t deviceId;
  int32_t source;
  int32_t action;

  int64_t eventTime;
  int64_t downTime;

  int32_t flags;
  int32_t metaState;

  int32_t modifiers;
  int32_t repeatCount;
  int32_t keyCode;
} GameActivityKeyEvent;

/**
 * These are the callbacks the framework makes into a native application.
 * All of these callbacks happen on the main thread of the application.
 * By default, all callbacks are NULL; set to a pointer to your own function
 * to have it called.
 */
typedef struct GameActivityCallbacks {
  /**
   * GameActivity has started.  See Java documentation for Activity.onStart()
   * for more information.
   */
  void (*onStart)(GameActivity* activity);

  /**
   * GameActivity has resumed.  See Java documentation for Activity.onResume()
   * for more information.
   */
  void (*onResume)(GameActivity* activity);

  /**
   * Framework is asking GameActivity to save its current instance state.
   * See Java documentation for Activity.onSaveInstanceState() for more
   * information.  The returned pointer needs to be created with malloc();
   * the framework will call free() on it for you.  You also must fill in
   * outSize with the number of bytes in the allocation.  Note that the
   * saved state will be persisted, so it can not contain any active
   * entities (pointers to memory, file descriptors, etc).
   */
  void* (*onSaveInstanceState)(GameActivity* activity, size_t* outSize);

  /**
   * GameActivity has paused.  See Java documentation for Activity.onPause()
   * for more information.
   */
  void (*onPause)(GameActivity* activity);

  /**
   * GameActivity has stopped.  See Java documentation for Activity.onStop()
   * for more information.
   */
  void (*onStop)(GameActivity* activity);

  /**
   * GameActivity is being destroyed.  See Java documentation for
   * Activity.onDestroy() for more information.
   */
  void (*onDestroy)(GameActivity* activity);

  /**
   * Focus has changed in this GameActivity's window.  This is often used,
   * for example, to pause a game when it loses input focus.
   */
  void (*onWindowFocusChanged)(GameActivity* activity, int hasFocus);

  /**
   * The drawing window for this native activity has been created.  You
   * can use the given native window object to start drawing.
   */
  void (*onNativeWindowCreated)(GameActivity* activity, ANativeWindow* window);

  /**
   * The drawing window for this native activity has been resized.  You should
   * retrieve the new size from the window and ensure that your rendering in
   * it now matches.
   */
  void (*onNativeWindowResized)(GameActivity* activity, ANativeWindow* window);

  /**
   * The drawing window for this native activity needs to be redrawn.  To avoid
   * transient artifacts during screen changes (such resizing after rotation),
   * applications should not return from this function until they have finished
   * drawing their window in its current state.
   */
  void (*onNativeWindowRedrawNeeded)(GameActivity* activity,
                                     ANativeWindow* window);

  /**
   * The drawing window for this native activity is going to be destroyed.
   * You MUST ensure that you do not touch the window object after returning
   * from this function: in the common case of drawing to the window from
   * another thread, that means the implementation of this callback must
   * properly synchronize with the other thread to stop its drawing before
   * returning from here.
   */
  void (*onNativeWindowDestroyed)(GameActivity* activity,
                                  ANativeWindow* window);

  /**
   * The rectangle in the window in which content should be placed has changed.
   */
  void (*onContentRectChanged)(GameActivity* activity, const ARect* rect);

  /**
   * The current device AConfiguration has changed.  The new configuration can
   * be retrieved from assetManager.
   */
  void (*onConfigurationChanged)(GameActivity* activity);

  /**
   * The system is running low on memory.  Use this callback to release
   * resources you do not need, to help the system avoid killing more
   * important processes.
   */
  void (*onLowMemory)(GameActivity* activity);

  /**
   * Callback called for every MotionEvent done on the GameActivity SurfaceView.
   * After `event` was handled by the application/game code, it must be released
   * using `GameActivityMotionEvent_release`.
   */
  void (*onTouchEvent)(GameActivity* activity, GameActivityMotionEvent* event);

  /**
   * Callback called for every key down event on the GameActivity SurfaceView.
   * After `event` was handled by the application/game code, it must be released
   * using `GameActivityKeyEvent_release`.
   */
  void (*onKeyDown)(GameActivity* activity, GameActivityKeyEvent* event);

  /**
   * Callback called for every key up event on the GameActivity SurfaceView.
   * After `event` was handled by the application/game code, it must be released
   * using `GameActivityKeyEvent_release`.
   */
  void (*onKeyUp)(GameActivity* activity, GameActivityKeyEvent* event);

  /**
   * Callback called for every soft-keyboard text input event.
   * Call GameActivity_getTextInputState to get the new state of the soft
   * keyboard.
   */
  void (*onTextInputEvent)(GameActivity* activity, const GameTextInputState* state);
} GameActivityCallbacks;

/**
 * \brief Convert a Java `MotionEvent` to a `GameActivityMotionEvent`.
 *
 * This is done automatically by the GameActivity: see `onTouchEvent` to set
 * a callback to consume the received events.
 * This function can be used if you re-implement events handling in your own
 * activity.
 */
GameActivityMotionEvent* GameActivityMotionEvent_fromJava(JNIEnv* env,
                                                          jobject motionEvent);

/**
 * \brief Free the specified event, deallocating it from memory.
 *
 * Call this function once the application/game is done handling the event.
 * After this is called, any reference to the event is invalid and must be
 * dropped.
 */
void GameActivityMotionEvent_release(GameActivityMotionEvent* event);

/**
 * \brief Convert a Java `KeyEvent` to a `GameActivityKeyEvent`.
 *
 * This is done automatically by the GameActivity: see `onKeyUp` and `onKeyDown`
 * to set a callback to consume the received events.
 * This function can be used if you re-implement events handling in your own
 * activity.
 */
GameActivityKeyEvent* GameActivityKeyEvent_fromJava(JNIEnv* env,
                                                    jobject motionEvent);

/**
 * \brief Free the specified event, deallocating it from memory.
 *
 * Call this function once the application/game is done handling the event.
 * After this is called, any reference to the event is invalid and must be
 * dropped.
 */
void GameActivityKeyEvent_release(GameActivityKeyEvent* event);

/**
 * This is the function that must be in the native code to instantiate the
 * application's native activity.  It is called with the activity instance (see
 * above); if the code is being instantiated from a previously saved instance,
 * the savedState will be non-NULL and point to the saved data.  You must make
 * any copy of this data you need -- it will be released after you return from
 * this function.
 */
typedef void GameActivity_createFunc(GameActivity* activity, void* savedState,
                                     size_t savedStateSize);

/**
 * The name of the function that NativeInstance looks for when launching its
 * native code.  This is the default function that is used, you can specify
 * "android.app.func_name" string meta-data in your manifest to use a different
 * function.
 */
extern GameActivity_createFunc GameActivity_onCreate;

/**
 * Finish the given activity.  Its finish() method will be called, causing it
 * to be stopped and destroyed.  Note that this method can be called from
 * *any* thread; it will send a message to the main thread of the process
 * where the Java finish call will take place.
 */
void GameActivity_finish(GameActivity* activity);

/**
 * Change the window format of the given activity.  Calls
 * getWindow().setFormat() of the given activity.  Note that this method can be
 * called from *any* thread; it will send a message to the main thread of the
 * process where the Java finish call will take place.
 */
void GameActivity_setWindowFormat(GameActivity* activity, int32_t format);

/**
 * Change the window flags of the given activity.  Calls getWindow().setFlags()
 * of the given activity.  Note that this method can be called from
 * *any* thread; it will send a message to the main thread of the process
 * where the Java finish call will take place.  See window.h for flag constants.
 */
void GameActivity_setWindowFlags(GameActivity* activity, uint32_t addFlags,
                                 uint32_t removeFlags);

/**
 * Flags for GameActivity_showSoftInput; see the Java InputMethodManager
 * API for documentation.
 */
enum {
  /**
   * Implicit request to show the input window, not as the result
   * of a direct request by the user.
   */
  GAMEACTIVITY_SHOW_SOFT_INPUT_IMPLICIT = 0x0001,

  /**
   * The user has forced the input method open (such as by
   * long-pressing menu) so it should not be closed until they
   * explicitly do so.
   */
  GAMEACTIVITY_SHOW_SOFT_INPUT_FORCED = 0x0002,
};

/**
 * Show the IME while in the given activity.  Calls
 * InputMethodManager.showSoftInput() for the given activity.  Note that this
 * method can be called from *any* thread; it will send a message to the main
 * thread of the process where the Java call will take place.
 */
void GameActivity_showSoftInput(GameActivity* activity, uint32_t flags);

/**
 * Set the text entry state (see documentation of the GameTextInputState struct
 * in the Game Text Input library reference).
 *
 * Ownership of the state is maintained by the caller.
 */
void GameActivity_setTextInputState(GameActivity* activity,
                                    const GameTextInputState* state);

/**
 * Get the last-received text entry state (see documentation of the
 * GameTextInputState struct in the Game Text Input library reference).
 *
 * Ownership of the returned value is maintained by the GameActivity: do not
 * delete it.
 */
const GameTextInputState* GameActivity_getTextInputState(GameActivity* activity);

/**
 * Flags for GameActivity_hideSoftInput; see the Java InputMethodManager
 * API for documentation.
 */
enum {
  /**
   * The soft input window should only be hidden if it was not
   * explicitly shown by the user.
   */
  GAMEACTIVITY_HIDE_SOFT_INPUT_IMPLICIT_ONLY = 0x0001,
  /**
   * The soft input window should normally be hidden, unless it was
   * originally shown with {@link GAMEACTIVITY_SHOW_SOFT_INPUT_FORCED}.
   */
  GAMEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS = 0x0002,
};

/**
 * Hide the IME while in the given activity.  Calls
 * InputMethodManager.hideSoftInput() for the given activity.  Note that this
 * method can be called from *any* thread; it will send a message to the main
 * thread of the process where the Java finish call will take place.
 */
void GameActivity_hideSoftInput(GameActivity* activity, uint32_t flags);

#ifdef __cplusplus
};
#endif

/** @} */

#endif  // ANDROID_GAME_SDK_GAME_ACTIVITY_H
