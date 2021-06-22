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
#include <memory>

#define LOG_TAG "GameTextInput"

// Cache of field ids in the Java GameTextInputState class
struct StateClassInfo {
  jfieldID text;
  jfieldID selectionStart;
  jfieldID selectionEnd;
  jfieldID composingRegionStart;
  jfieldID composingRegionEnd;
};

// Main GameTextInput object.
struct GameTextInput {
 public:
  GameTextInput(JNIEnv *env);
  ~GameTextInput();
  void setState(const GameTextInputState& state);
  const GameTextInputState& getState() const { return gameTextInputState_;}
  void setInputConnection(jobject inputConnection);
  void processEvent(jobject textInputEvent);
  void showIme(uint32_t flags);
  void hideIme(uint32_t flags);
  void setEventCallback(GameTextInputEventCallback callback, void *context);
  jobject stateToJava(const GameTextInputState *state) const;
  GameTextInputState stateFromJava(jobject textInputEvent) const;
 private:
  JNIEnv *env_;
  // Needed because we can't find the class on a non-Java thread
  jclass gameTextInputStateClass_;
  // The latest text input update
  GameTextInputState gameTextInputState_;
  // An instance of gametextinput.InputConnection
  jclass inputConnectionClass_;
  jobject inputConnection_;
  jmethodID inputConnectionSetStateMethod_;
  jmethodID setSoftKeyboardActiveMethod_;
  void (*eventCallback_)(void *context, const struct GameTextInputState * state);
  void *eventCallbackContext_;
  StateClassInfo stateClassInfo_;
};

std::unique_ptr<GameTextInput> s_gameTextInput;

// Copy UTF-8 string.
// len is the byte length, not including the final null char.
static char *string_copy(const char *c, size_t len) {
  if (c == nullptr) return nullptr;
  char *ret = (char *)malloc(len + 1);  // Include null
  memcpy(ret, c, len);
  ret[len] = 0;  // Add a null at the end, rather than assuming that the input
                 // is null-termninated.
  return ret;
}

extern "C" {

///////////////////////////////////////////////////////////
/// GameTextInputState C Functions
///////////////////////////////////////////////////////////

void GameTextInputState_constructEmpty(GameTextInputState *s) { *s = {}; }

void GameTextInputState_construct(GameTextInputState *s, const char *t,
                                         int32_t l, int s0, int s1, int c0,
                                         int c1) {
  s->text_UTF8 = string_copy(t, l);
  s->text_length = l;
  s->selection = {s0, s1};
  s->composingRegion = {c0, c1};
  s->dealloc = free;
}
void GameTextInputState_destruct(GameTextInputState *s) {
  if (s->text_UTF8 && s->dealloc) s->dealloc((void *)s->text_UTF8);
}
void GameTextInputState_set(GameTextInputState *s,
                                   const GameTextInputState *rhs) {
  GameTextInputState_destruct(s);
  GameTextInputState_construct(
      s, rhs->text_UTF8, rhs->text_length, rhs->selection.start,
      rhs->selection.end, rhs->composingRegion.start, rhs->composingRegion.end);
}

// Convert to a Java structure.
jobject GameTextInputState_toJava(const GameTextInput* gameTextInput,
                                  const GameTextInputState *state) {
  return gameTextInput->stateToJava(state);
}

// Convert from Java structure.
GameTextInputState GameTextInputState_fromJava(const GameTextInput* gameTextInput,
                                               jobject textInputEvent) {
  return gameTextInput->stateFromJava(textInputEvent);
}

///////////////////////////////////////////////////////////
/// GameTextInput C Functions
///////////////////////////////////////////////////////////

struct GameTextInput *GameTextInput_init(JNIEnv *env) {
  if (s_gameTextInput.get()!=nullptr) {
      __android_log_print(ANDROID_LOG_WARN, LOG_TAG,
                 "Warning: called GameTextInput_init twice without calling GameTextInput_destroy");
      return s_gameTextInput.get();
  }
  s_gameTextInput = std::make_unique<GameTextInput>(env);
  return s_gameTextInput.get();
}

void GameTextInput_destroy(GameTextInput *input) {
  if (input == nullptr || s_gameTextInput.get()==nullptr) return;
  s_gameTextInput.reset();
}

void GameTextInput_setState(GameTextInput *input, const GameTextInputState *state) {
  if (state==nullptr) return;
  input->setState(*state);
}

const GameTextInputState *GameTextInput_getState(GameTextInput *input) {
  return &input->getState();
}

void GameTextInput_setInputConnection(GameTextInput *input, jobject inputConnection) {
  input->setInputConnection(inputConnection);
}

void GameTextInput_processEvent(GameTextInput *input, jobject textInputEvent) {
  input->processEvent(textInputEvent);
}

void GameTextInput_showIme(struct GameTextInput *input, uint32_t flags) {
  input->showIme(flags);
}

void GameTextInput_hideIme(struct GameTextInput *input, uint32_t flags) {
  input->hideIme(flags);
}

void GameTextInput_setEventCallback(struct GameTextInput *input,
                                    GameTextInputEventCallback callback, void *context) {
                     input->setEventCallback(callback, context);
}

} // extern "C"

///////////////////////////////////////////////////////////
/// GameTextInput C++ class Implementation
///////////////////////////////////////////////////////////

GameTextInput::GameTextInput(JNIEnv* env) : env_(env), gameTextInputState_ {} {
  gameTextInputStateClass_ = (jclass)env_->NewGlobalRef(
      env_->FindClass("com/google/androidgamesdk/gametextinput/State"));
  inputConnectionClass_ = (jclass)env_->NewGlobalRef(
      env_->FindClass("com/google/androidgamesdk/gametextinput/InputConnection"));
  inputConnectionSetStateMethod_ =
      env_->GetMethodID(inputConnectionClass_, "setState",
                       "(Lcom/google/androidgamesdk/gametextinput/State;)V");
  inputConnection_ = nullptr;
  eventCallback_ = nullptr;
  setSoftKeyboardActiveMethod_ = env_->GetMethodID(
      inputConnectionClass_, "setSoftKeyboardActive", "(ZI)V");

  stateClassInfo_.text =
      env_->GetFieldID(gameTextInputStateClass_, "text", "Ljava/lang/String;");
  stateClassInfo_.selectionStart =
      env_->GetFieldID(gameTextInputStateClass_, "selectionStart", "I");
  stateClassInfo_.selectionEnd =
      env_->GetFieldID(gameTextInputStateClass_, "selectionEnd", "I");
  stateClassInfo_.composingRegionStart =
      env_->GetFieldID(gameTextInputStateClass_, "composingRegionStart", "I");
  stateClassInfo_.composingRegionEnd =
      env_->GetFieldID(gameTextInputStateClass_, "composingRegionEnd", "I");

  return s_gameTextInput.get();
}

GameTextInput::~GameTextInput() {
  if (gameTextInputStateClass_ != NULL) {
    env_->DeleteGlobalRef(gameTextInputStateClass_);
    gameTextInputStateClass_ = NULL;
  }
  if (inputConnectionClass_ != NULL) {
    env_->DeleteGlobalRef(inputConnectionClass_);
    inputConnectionClass_ = NULL;
  }
  if (inputConnection_ != NULL) {
    env_->DeleteGlobalRef(inputConnection_);
    inputConnection_ = NULL;
  }
  GameTextInputState_destruct(&gameTextInputState_);
}

void GameTextInput::setState(const GameTextInputState& state) {
  if (inputConnection_ == nullptr) return;
  jobject jstate =
      GameTextInputState_toJava(this, &state);
  env_->CallVoidMethod(inputConnection_,
                             inputConnectionSetStateMethod_, jstate);
  env_->DeleteLocalRef(jstate);
  GameTextInputState_set(&gameTextInputState_, &state);
}

void GameTextInput::setInputConnection(jobject inputConnection) {
  if (inputConnection_ != NULL) {
    env_->DeleteGlobalRef(inputConnection_);
  }
  inputConnection_ = env_->NewGlobalRef(inputConnection);
}

void GameTextInput::processEvent(jobject textInputEvent) {
  GameTextInputState_destruct(&gameTextInputState_);
  gameTextInputState_ = GameTextInputState_fromJava(this, textInputEvent);
  if (eventCallback_) {
    eventCallback_(eventCallbackContext_, &gameTextInputState_);
  }
}

void GameTextInput::showIme(uint32_t flags) {
  if (inputConnection_ == nullptr) return;
  env_->CallVoidMethod(inputConnection_,
                             setSoftKeyboardActiveMethod_, true, flags);
}

void GameTextInput::setEventCallback(GameTextInputEventCallback callback, void *context) {
  eventCallback_ = callback;
  eventCallbackContext_ = context;
}

void GameTextInput::hideIme(uint32_t flags) {
  if (inputConnection_ == nullptr) return;
  env_->CallVoidMethod(inputConnection_,
                       setSoftKeyboardActiveMethod_, false, flags);
}

jobject GameTextInput::stateToJava(const GameTextInputState *state) const {
  static jmethodID constructor = nullptr;
  if (constructor == nullptr) {
    constructor =
        env_->GetMethodID(gameTextInputStateClass_, "<init>", "(Ljava/lang/String;IIII)V");
    if (constructor == nullptr) {
      __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
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
  jstring jtext = env_->NewStringUTF(text);
  jobject jobj =
      env_->NewObject(gameTextInputStateClass_, constructor, jtext, state->selection.start,
                     state->selection.end, state->composingRegion.start,
                     state->composingRegion.end);
  env_->DeleteLocalRef(jtext);
  return jobj;
}

GameTextInputState GameTextInput::stateFromJava(jobject textInputEvent) const {
  jstring text =
      (jstring)env_->GetObjectField(textInputEvent, stateClassInfo_.text);
  // Note this is 'modified' UTF-8, not true UTF-8. It has no NULLs in it,
  // except at the end. It's actually not specified whether the value returned
  // by GetStringUTFChars includes a null at the end, but it *seems to* on
  // Android.
  const char *text_chars = env_->GetStringUTFChars(text, NULL);
  int text_len = env_->GetStringUTFLength(
      text);  // Length in bytes, *not* including the null.
  int selectionStart =
      env_->GetIntField(textInputEvent, stateClassInfo_.selectionStart);
  int selectionEnd =
      env_->GetIntField(textInputEvent, stateClassInfo_.selectionEnd);
  int composingRegionStart =
      env_->GetIntField(textInputEvent, stateClassInfo_.composingRegionStart);
  int composingRegionEnd =
      env_->GetIntField(textInputEvent, stateClassInfo_.composingRegionEnd);
  GameTextInputState state;
  GameTextInputState_construct(&state, text_chars, text_len, selectionStart,
                           selectionEnd, composingRegionStart,
                           composingRegionEnd);
  env_->ReleaseStringUTFChars(text, text_chars);
  env_->DeleteLocalRef(text);

  return state;
}
