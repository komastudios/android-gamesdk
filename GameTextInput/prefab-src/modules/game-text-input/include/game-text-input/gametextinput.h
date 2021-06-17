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
 * @defgroup game_text_input Game Text Input
 * The interface to use GameTextInput.
 * @{
 */

#pragma once

#include <jni.h>

/**
 * This struct holds a span within a region of text from start (inclusive) to
 * end (exclusive). An empty span or cursor position is specified with
 * start==end. An undefined span is specified with start = end = SPAN_UNDEFINED.
 */
typedef struct GameTextInputSpan {
  /** The start of the region (inclusive). */
  int start;
  /** The end of the region (exclusive). */
  int end;
} GameTextInputSpan;

/**
 * Values with special meaning in a GameTextInputSpan.
 */
enum { SPAN_UNDEFINED = -1 };

/**
 * This struct holds the state of an editable section of text.
 * The text can have a selection and a composing region defined on it.
 * A composing region is used by IMEs that allow input using multiple steps to
 * compose a glyph or word. Use functions GameTextInput_getState and
 * GameTextInput_setState to read and modify the state that an IME is editing.
 */
typedef struct GameTextInputState {
  /**
   * Text owned by the state, as a modified UTF-8 string. Null-terminated.
   * https://en.wikipedia.org/wiki/UTF-8#Modified_UTF-8
   */
  const char *text_UTF8;
  /**
   * Length in bytes of text_UTF8, *not* including the null at end.
   */
  int64_t text_length;
  /**
   * A selection defined on the text.
   */
  GameTextInputSpan selection;
  /**
   * A composing region defined on the text.
   */
  GameTextInputSpan composingRegion;
  /**
   * Deallocator for text_UTF8 string
   */
  void (*dealloc)(void *);
} GameTextInputState;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fill in the state with empty values.
 * @param state A struct owned by the caller.
 */
void GameTextInputState_constructEmpty(GameTextInputState *state);

/**
 * Fill in the state with the values passed-in. Ownership of the text is
 * maintained by the caller.
 * @param state A struct owned by the caller.
 * @param text_UTF8 A modified UTF8 character array owned by the caller.
 * @param text_length The length of text_UTF8.
 * @param selection_start
 * @param selection_end
 * @param composing_region_start
 * @param composing_region_end
 */
void GameTextInputState_construct(GameTextInputState *state, const char *text_UTF8,
                              int64_t text_length, int selection_start,
                              int selection_end, int composing_region_start,
                              int composing_region_end);

/**
 * Free any memory owned by the state.
 * @param state A struct owned by the caller.
 */
void GameTextInputState_destruct(GameTextInputState *state);

/**
 * Copy from rhs to state. Ownership of rhs and its internals is maintained by
 * the caller.
 * @param state
 * @param rhs
 */
void GameTextInputState_set(GameTextInputState *state, const GameTextInputState *rhs);

// Convert to a Java object. Don't forget to delete the local ref when you're
// done.
/**
 * Convert a GameTextInputState struct to a Java gametextinput.State object.
 * @param env A JNI env valid on the calling thread.
 * @param stateClass A Java class obtained using
 * FindClass("com/google/androidgamesdk/gametextinput/State") or equivalent.
 * @param state Input state to convert.
 * @return A Java object of class gametextinput.State.
 */
jobject GameTextInputState_ToJava(JNIEnv *env, jclass stateClass,
                              const GameTextInputState *state);

/**
 * Convert from a Java gametextinput.State object into a C GameTextInputState struct.
 * You are responsible for calling GameTextInputState_destruct on the returned
 * value.
 * @param env A JNI env valid on the calling thread.
 * @param stateClass A Java class obtained using
 * FindClass("com/google/androidgamesdk/gametextinput/State") or equivalent.
 * @param state A Java gametextinput.State object.
 * @return The converted GameTextInputState struct, now owned by the caller.
 */
GameTextInputState GameTextInputState_FromJava(JNIEnv *env, jclass stateClass,
                                       jobject state);

/**
 * Opaque handle to the GameTextInput API.
 */
typedef struct GameTextInput GameTextInput;

/**
 * Initialize the GameTextInput library.
 * @param env A JNI env valid on the calling thread.
 * @return A handle to the library.
 */
GameTextInput *GameTextInput_init(JNIEnv *env);

/**
 * Free any resources owned by the GameTextInput library.
 * @param input A valid GameTextInput library handle.
 */
void GameTextInput_destroy(GameTextInput *input);

/**
 * Show the IME. Calls InputMethodManager.showSoftInput().
 * @param input A valid GameTextInput library handle.
 * @param flags See
 * https://developer.android.com/reference/android/view/inputmethod/InputMethodManager
 */
void GameTextInput_showIme(GameTextInput *input, int32_t flags);
/**
 * Show the IME. Calls InputMethodManager.hideSoftInputFromWindow().
 * @param input A valid GameTextInput library handle.
 * @param flags See
 * https://developer.android.com/reference/android/view/inputmethod/InputMethodManager
 */
void GameTextInput_hideIme(GameTextInput *input, int32_t flags);

/**
 * Get the current GameTextInput state. The state is modified by changes in the IME
 * and calls to GameTextInput_setState.
 * @param input A valid GameTextInput library handle.
 * @return The current maintained GameTextInputState or NULL if there is none.
 */
const GameTextInputState *GameTextInput_getState(GameTextInput *input);

/**
 * Set the current GameTextInput state. This state is reflected to any active IME.
 * @param input A valid GameTextInput library handle.
 * @param state The state to set. Ownership is maintained by the caller.
 */
void GameTextInput_setState(GameTextInput *input, const GameTextInputState *state);

/**
 * You must call this to tell the C interface about your Java
 * gametextinput.InputConnection, unless you are using GameActivity.
 * @param input A valid GameTextInput library handle.
 * @param inputConnection A gametextinput.InputConnection object.
 */
void GameTextInput_setInputConnection(GameTextInput *input, jobject inputConnection);

/**
 * Optionally set a callback to be called whenever the IME changes the state.
 * Not necessary if you are using GameActivity.
 * @param input A valid GameTextInput library handle.
 * @param callback The callback to call.
 * @param context Context passed as first argument to the callback.
 */
void GameTextInput_setEventCallback(GameTextInput *input,
                                void (*callback)(void *,
                                                 const GameTextInputState *),
                                void *context);

/**
 * Call this function from your Java gametextinput.Listener.stateChanged method to
 * automatically convert eventState and trigger the callback defined above. Not
 * necessary if you are using GameActivity.
 * @param input A valid GameTextInput library handle.
 * @param eventState A Java gametextinput.State object.
 */
void GameTextInput_processEvent(GameTextInput *input, jobject eventState);

#ifdef __cplusplus
}
#endif

/** @} */
