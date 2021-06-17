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
#include "game-text-input/gametextinput.h"

#include <android/log.h>
#include <jni.h>
#include <stdlib.h>
#include <string.h>

// Convert to a Java structure
extern "C" jobject GameTextInputState_ToJava(JNIEnv *env, jclass stateClass,
                                         const GameTextInputState *state) {
  static jmethodID constructor = nullptr;
  if (constructor == nullptr) {
    constructor =
        env->GetMethodID(stateClass, "<init>", "(Ljava/lang/String;IIII)V");
    if (constructor == nullptr) {
      __android_log_print(ANDROID_LOG_ERROR, "GameTextInput",
                          "Can't find gametextinput.State constructor");
      return nullptr;
    }
  }
  const char *text = state->text_UTF8;
  if (text == nullptr) {
    static char empty_string[] = "";
    text = empty_string;
  }
  // Note that this expects 'modified' UTF-8 which is not the same as UTF-8
  // https://en.wikipedia.org/wiki/UTF-8#Modified_UTF-8
  jstring jtext = env->NewStringUTF(text);
  jobject jobj =
      env->NewObject(stateClass, constructor, jtext, state->selection.start,
                     state->selection.end, state->composingRegion.start,
                     state->composingRegion.end);
  env->DeleteLocalRef(jtext);
  return jobj;
}

static struct {
  jfieldID text;
  jfieldID selectionStart;
  jfieldID selectionEnd;
  jfieldID composingRegionStart;
  jfieldID composingRegionEnd;
} gStateClassInfo;

static char *string_copy(const char *c, size_t len) {
  if (c == nullptr) return nullptr;
  char *ret = (char *)malloc(len + 1);  // Include null
  memcpy(ret, c, len);
  ret[len] = 0;  // Add a null at the end, rather than assuming that the input
                 // is null-termninated.
  return ret;
}

extern "C" GameTextInputState GameTextInputState_FromJava(JNIEnv *env,
                                                  jclass stateClass,
                                                  jobject textInputEvent) {
  static bool gStateClassInfoInitialized = false;
  if (!gStateClassInfoInitialized) {
    gStateClassInfo.text =
        env->GetFieldID(stateClass, "text", "Ljava/lang/String;");
    gStateClassInfo.selectionStart =
        env->GetFieldID(stateClass, "selectionStart", "I");
    gStateClassInfo.selectionEnd =
        env->GetFieldID(stateClass, "selectionEnd", "I");
    gStateClassInfo.composingRegionStart =
        env->GetFieldID(stateClass, "composingRegionStart", "I");
    gStateClassInfo.composingRegionEnd =
        env->GetFieldID(stateClass, "composingRegionEnd", "I");

    gStateClassInfoInitialized = true;
  }

  jstring text =
      (jstring)env->GetObjectField(textInputEvent, gStateClassInfo.text);
  // Note this is 'modified' UTF-8, not true UTF-8. It has no NULLs in it,
  // except at the end. It's actually not specified whether the value returned
  // by GetStringUTFChars includes a null at the end, but it *seems to* on
  // Android.
  const char *text_chars = env->GetStringUTFChars(text, NULL);
  int text_len = env->GetStringUTFLength(
      text);  // Length in bytes, *not* including the null.
  int selectionStart =
      env->GetIntField(textInputEvent, gStateClassInfo.selectionStart);
  int selectionEnd =
      env->GetIntField(textInputEvent, gStateClassInfo.selectionEnd);
  int composingRegionStart =
      env->GetIntField(textInputEvent, gStateClassInfo.composingRegionStart);
  int composingRegionEnd =
      env->GetIntField(textInputEvent, gStateClassInfo.composingRegionEnd);
  GameTextInputState state;
  GameTextInputState_construct(&state, text_chars, text_len, selectionStart,
                           selectionEnd, composingRegionStart,
                           composingRegionEnd);
  env->ReleaseStringUTFChars(text, text_chars);
  env->DeleteLocalRef(text);

  return state;
}

extern "C" void GameTextInputState_constructEmpty(GameTextInputState *s) { *s = {}; }
extern "C" void GameTextInputState_construct(GameTextInputState *s, const char *t,
                                         int64_t l, int s0, int s1, int c0,
                                         int c1) {
  s->text_UTF8 = string_copy(t, l);
  s->text_length = l;
  s->selection = {s0, s1};
  s->composingRegion = {c0, c1};
  s->dealloc = free;
}
extern "C" void GameTextInputState_destruct(GameTextInputState *s) {
  if (s->text_UTF8 && s->dealloc) s->dealloc((void *)s->text_UTF8);
}
extern "C" void GameTextInputState_set(GameTextInputState *s,
                                   const GameTextInputState *rhs) {
  GameTextInputState_destruct(s);
  GameTextInputState_construct(
      s, rhs->text_UTF8, rhs->text_length, rhs->selection.start,
      rhs->selection.end, rhs->composingRegion.start, rhs->composingRegion.end);
}

struct GameTextInput {
  JNIEnv *env;
  // Needed because we can't find the class on a non-Java thread
  jclass gameTextInputStateClass;
  // The latest text input update
  GameTextInputState gameTextInputState;
  // An instance of gametextinput.InputConnection
  jclass inputConnectionClass;
  jobject inputConnection;
  jmethodID inputConnectionSetStateMethod;
  jmethodID setSoftKeyboardActiveMethod;

  void (*eventCallback)(void *, const struct GameTextInputState *);

  void *eventCallbackContext;
};

// TODO(b/187149533): this shouldn't be static.
GameTextInput s_gameTextInput;

extern "C" {

struct GameTextInput *GameTextInput_init(JNIEnv *env) {
  s_gameTextInput.env = env;
  GameTextInputState_constructEmpty(&s_gameTextInput.gameTextInputState);
  s_gameTextInput.gameTextInputStateClass = (jclass)env->NewGlobalRef(
      env->FindClass("com/google/androidgamesdk/gametextinput/State"));
  s_gameTextInput.inputConnectionClass = (jclass)env->NewGlobalRef(
      env->FindClass("com/google/androidgamesdk/gametextinput/InputConnection"));
  s_gameTextInput.inputConnectionSetStateMethod =
      env->GetMethodID(s_gameTextInput.inputConnectionClass, "setState",
                       "(Lcom/google/androidgamesdk/gametextinput/State;)V");
  s_gameTextInput.inputConnection = nullptr;
  s_gameTextInput.eventCallback = nullptr;
  s_gameTextInput.setSoftKeyboardActiveMethod = env->GetMethodID(
      s_gameTextInput.inputConnectionClass, "setSoftKeyboardActive", "(ZI)V");
  return &s_gameTextInput;
}

void GameTextInput_destroy(GameTextInput *input) {
  if (input == nullptr) return;
  if (input->gameTextInputStateClass != NULL) {
    input->env->DeleteGlobalRef(input->gameTextInputStateClass);
    input->gameTextInputStateClass = NULL;
  }
  if (input->inputConnectionClass != NULL) {
    input->env->DeleteGlobalRef(input->inputConnectionClass);
    input->inputConnectionClass = NULL;
  }
  if (input->inputConnection != NULL) {
    input->env->DeleteGlobalRef(input->inputConnection);
    input->inputConnection = NULL;
  }
  GameTextInputState_destruct(&input->gameTextInputState);
}

void GameTextInput_setState(GameTextInput *input, const GameTextInputState *state) {
  if (input->inputConnection == nullptr) return;
  jobject jstate =
      GameTextInputState_ToJava(input->env, input->gameTextInputStateClass, state);
  input->env->CallVoidMethod(input->inputConnection,
                             input->inputConnectionSetStateMethod, jstate);
  input->env->DeleteLocalRef(jstate);
  GameTextInputState_set(&input->gameTextInputState, state);
}

const GameTextInputState *GameTextInput_getState(GameTextInput *input) {
  return &input->gameTextInputState;
}

void GameTextInput_setInputConnection(GameTextInput *input, jobject inputConnection) {
  if (input->inputConnection != NULL) {
    input->env->DeleteGlobalRef(input->inputConnection);
  }
  input->inputConnection = input->env->NewGlobalRef(inputConnection);
}

void GameTextInput_processEvent(GameTextInput *input, jobject textInputEvent) {
  GameTextInputState_destruct(&input->gameTextInputState);
  input->gameTextInputState = GameTextInputState_FromJava(
      input->env, input->gameTextInputStateClass, textInputEvent);
  if (input->eventCallback) {
    input->eventCallback(input->eventCallbackContext, &input->gameTextInputState);
  }
}

void GameTextInput_showIme(struct GameTextInput *input, int32_t flags) {
  if (input->inputConnection == nullptr) return;
  input->env->CallVoidMethod(input->inputConnection,
                             input->setSoftKeyboardActiveMethod, true, flags);
}

void GameTextInput_hideIme(struct GameTextInput *input, int32_t flags) {
  if (input->inputConnection == nullptr) return;
  input->env->CallVoidMethod(input->inputConnection,
                             input->setSoftKeyboardActiveMethod, false, flags);
}

void GameTextInput_setEventCallback(struct GameTextInput *input,
                                void (*callback)(void *,
                                                 const struct GameTextInputState *),
                                void *context) {
  input->eventCallback = callback;
  input->eventCallbackContext = context;
}
}