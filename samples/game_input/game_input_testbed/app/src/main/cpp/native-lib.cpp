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
#include <android/log.h>
#include <jni.h>

#include <memory>

#define LOG_TAG "NativeLib"
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__);

// Import the Game Input interfae:
#include <game-input/gameinput.h>

// Also include the source of Game Input in one of your C++ file, so that the
// library is compiled as part of your build:
#include <game-input/gameinput.cpp>

static GameInput *gameInput = nullptr;
static jobject activity = nullptr;
static jmethodID setDisplayedText = nullptr;

static void onEvent(void *context, const GameInputState *state) {
  ALOGD("OnEvent");
  ALOGD("String entered is: %s, len: %d", state->text_UTF8, state->text_length);
  JNIEnv *env = (JNIEnv *)context;

  jstring currentText = env->NewStringUTF(state->text_UTF8);
  env->CallVoidMethod(activity, setDisplayedText, currentText);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameinput_testbed_MainActivity_onCreated(JNIEnv *env, jobject thiz) {
  ALOGD("Calling GameInput_init...");
  gameInput = GameInput_init(env);
  activity = env->NewGlobalRef(thiz);
  ALOGD("activity is %p...", thiz);
  jclass activityClass = env->FindClass("com/gameinput/testbed/MainActivity");

  setDisplayedText = env->GetMethodID(activityClass, "setDisplayedText",
                                      "(Ljava/lang/String;)V");
  env->DeleteLocalRef(activityClass);
  ALOGD("setDisplayedText is %p...", setDisplayedText);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameinput_testbed_InputEnabledTextView_onTextInputEventNative(
    JNIEnv *env, jobject thiz, jobject soft_keyboard_event) {
  ALOGD("Calling GameInput_processEvent...");
  ALOGD("activity is in this method %p.", thiz);
  GameInput_processEvent(gameInput, soft_keyboard_event);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameinput_testbed_MainActivity_setInputConnectionNative(
    JNIEnv *env, jobject thiz, jobject inputConnection) {
  ALOGD("Calling GameInput_setInputConnection...");
  GameInput_setInputConnection(gameInput, inputConnection);
  GameInput_setEventCallback(gameInput, onEvent, env);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameinput_testbed_MainActivity_showIme(JNIEnv *env, jobject thiz) {
  GameInput_showIme(gameInput, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameinput_testbed_MainActivity_hideIme(JNIEnv *env, jobject thiz) {
  GameInput_hideIme(gameInput, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameinput_testbed_MainActivity_sendSelectionToStart(JNIEnv *env,
                                                             jobject thiz) {
  const GameInputState *state = GameInput_getState(gameInput);
  GameInputState *state_copy = (GameInputState *)malloc(sizeof(GameInputState));
  GameInputState_constructEmpty(state_copy);
  GameInputState_set(state_copy, state);

  state_copy->selection = {0, 0};

  GameInput_setState(gameInput, state_copy);
  free(state_copy);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameinput_testbed_MainActivity_sendSelectionToEnd(JNIEnv *env,
                                                           jobject thiz) {
  const GameInputState *state = GameInput_getState(gameInput);
  GameInputState *state_copy = (GameInputState *)malloc(sizeof(GameInputState));
  GameInputState_constructEmpty(state_copy);
  GameInputState_set(state_copy, state);

  state_copy->selection = {(int)state->text_length, (int)state->text_length};

  GameInput_setState(gameInput, state_copy);
  free(state_copy);
}