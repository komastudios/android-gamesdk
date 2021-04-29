/*
 * Copyright (C) Google Inc. 2021
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
 * @file gameinput.h
 */

#pragma once

#include <jni.h>

/**
 * This struct holds a span within a region of text from start (inclusive) to end (exclusive).
 * An empty span or cursor position is specified with start==end.
 * An undefined span is specified with start = end = SPAN_UNDEFINED.
 */
typedef struct GameInputSpan {
    int start;
    int end;
} GameInputSpan;

/**
 * Values with special meaning in a GameInputSpan.
 */
enum {
    SPAN_UNDEFINED = -1
};

/**
 * This struct holds the state of an editable section of text.
 * The text can have a selection and a composing region defined on it.
 * A composing region is used by IMEs that allow input using multiple steps to compose a glyph
 * or word.
 * Use functions GameInput_getState and GameInput_setState to read and modify the state that
 * an IME is editing.
 */
typedef struct GameInputState {
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
    GameInputSpan selection;
    /**
     * A composing region defined on the text.
     */
    GameInputSpan composingRegion;
    /**
     * Deallocator for text_UTF8 string
     */
    void (*dealloc)(void *);
} GameInputState;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fill in the state with empty values.
 * @param state A struct owned by the caller.
 */
void GameInputState_constructEmpty(GameInputState *state);

 /**
  * Fill in the state with the values passed-in. Ownership of the text is maintained by the caller.
  * @param state A struct owned by the caller.
  * @param text_UTF8 A modified UTF8 character array owned by the caller.
  * @param text_length The length of text_UTF8.
  * @param selection_start
  * @param selection_end
  * @param composing_region_start
  * @param composing_region_end
  */
void GameInputState_construct(GameInputState *state, const char *text_UTF8, int64_t text_length, int selection_start, int selection_end, int composing_region_start, int composing_region_end);

/**
 * Free any memory owned by the state.
 * @param state A struct owned by the caller.
 */
void GameInputState_destruct(GameInputState *state);

/**
 * Copy from rhs to state. Ownership of rhs and its internals is maintained by the caller.
 * @param state
 * @param rhs
 */
void GameInputState_set(GameInputState *state, const GameInputState *rhs);

// Convert to a Java object. Don't forget to delete the local ref when you're done.
/**
 * Convert a GameInputState struct to a Java gameinput.State object.
 * @param env A JNI env valid on the calling thread.
 * @param stateClass A Java class obtained using FindClass("com/google/androidgamesdk/gameinput/State") or equivalent.
 * @param state Input state to convert.
 * @return A Java object of class gameinput.State.
 */
jobject GameInputState_ToJava(JNIEnv *env, jclass stateClass, const GameInputState *state);

/**
 * Convert from a Java gameinput.State object into a C GameInputState struct.
 * You are responsible for calling GameInputState_destruct on the returned value.
 * @param env A JNI env valid on the calling thread.
 * @param stateClass A Java class obtained using FindClass("com/google/androidgamesdk/gameinput/State") or equivalent.
 * @param state A Java gameinput.State object.
 * @return The converted GameInputState struct, now owned by the caller.
 */
GameInputState GameInputState_FromJava(JNIEnv *env, jclass stateClass, jobject state);

/**
 * Opaque handle to the GameInput API.
 */
typedef struct GameInput GameInput;

/**
 * Initialize the GameInput library.
 * @param env A JNI env valid on the calling thread.
 * @return A handle to the library.
 */
GameInput *GameInput_init(JNIEnv *env);

/**
 * Free any resources owned by the GameInput library.
 * @param input A valid GameInput library handle.
 */
void GameInput_destroy(GameInput *input);

/**
 * Show the IME. Calls InputMethodManager.showSoftInput().
 * @param input A valid GameInput library handle.
 * @param flags See https://developer.android.com/reference/android/view/inputmethod/InputMethodManager
 */
void GameInput_showIme(GameInput *input, int32_t flags);
/**
 * Show the IME. Calls InputMethodManager.hideSoftInputFromWindow().
 * @param input A valid GameInput library handle.
 * @param flags See https://developer.android.com/reference/android/view/inputmethod/InputMethodManager
 */
void GameInput_hideIme(GameInput *input, int32_t flags);

/**
 * Get the current GameInput state. The state is modified by changes in the IME and calls to
 * GameInput_setState.
 * @param input A valid GameInput library handle.
 * @return The current maintained GameInputState or NULL if there is none.
 */
const GameInputState *GameInput_getState(GameInput *input);

/**
 * Set the current GameInput state. This state is reflected to any active IME.
 * @param input A valid GameInput library handle.
 * @param state The state to set. Ownership is maintained by the caller.
 */
void GameInput_setState(GameInput *input, const GameInputState *state);

/**
 * You must call this to tell the C interface about your Java gameinput.InputConnection,
 * unless you are using GameActivity.
 * @param input A valid GameInput library handle.
 * @param inputConnection A gameinput.InputConnection object.
 */
void GameInput_setInputConnection(GameInput *input, jobject inputConnection);

/**
 * Optionally set a callback to be called whenever the IME changes the state.
 * Not necessary if you are using GameActivity.
 * @param input A valid GameInput library handle.
 * @param callback The callback to call.
 * @param context Context passed as first argument to the callback.
 */
void GameInput_setEventCallback(GameInput *input,
                                void (*callback)(void *, const GameInputState *),
                                void *context);

// TODO (willosborn): can we avoid this by registering a native method with InputConnection directly?
/**
 * Call this function from your Java gameinput.Listener.stateChanged method to automatically convert
 * eventState and trigger the callback defined above.
 * Not necessary if you are using GameActivity.
 * @param input A valid GameInput library handle.
 * @param eventState A Java gameinput.State object.
 */
void GameInput_processEvent(GameInput *input, jobject eventState);

#ifdef __cplusplus
}
#endif
