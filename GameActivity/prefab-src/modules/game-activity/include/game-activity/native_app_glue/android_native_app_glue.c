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

#include "android_native_app_glue.h"

#include <android/log.h>
#include <errno.h>
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, "threaded_app", __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, "threaded_app", __VA_ARGS__))

/* For debug builds, always enable the debug traces in this library */
#ifndef NDEBUG
#define LOGV(...) \
    ((void)__android_log_print(ANDROID_LOG_VERBOSE, "threaded_app", \
                               __VA_ARGS__))
#else
#define LOGV(...) ((void)0)
#endif

static void free_saved_state(struct android_app* android_app) {
  pthread_mutex_lock(&android_app->mutex);
  if (android_app->savedState != NULL) {
    free(android_app->savedState);
    android_app->savedState = NULL;
    android_app->savedStateSize = 0;
  }
  pthread_mutex_unlock(&android_app->mutex);
}

int8_t android_app_read_cmd(struct android_app* android_app) {
  int8_t cmd;
  if (read(android_app->msgread, &cmd, sizeof(cmd)) != sizeof(cmd)) {
    LOGE("No data on command pipe!");
    return -1;
  }
  if (cmd == APP_CMD_SAVE_STATE) free_saved_state(android_app);
  return cmd;
}

static void print_cur_config(struct android_app* android_app) {
  char lang[2], country[2];
  AConfiguration_getLanguage(android_app->config, lang);
  AConfiguration_getCountry(android_app->config, country);

  LOGV(
      "Config: mcc=%d mnc=%d lang=%c%c cnt=%c%c orien=%d touch=%d dens=%d "
      "keys=%d nav=%d keysHid=%d navHid=%d sdk=%d size=%d long=%d "
      "modetype=%d modenight=%d",
      AConfiguration_getMcc(android_app->config),
        AConfiguration_getMnc(android_app->config), lang[0], lang[1],
        country[0], country[1],
        AConfiguration_getOrientation(android_app->config),
      AConfiguration_getTouchscreen(android_app->config),
      AConfiguration_getDensity(android_app->config),
      AConfiguration_getKeyboard(android_app->config),
      AConfiguration_getNavigation(android_app->config),
      AConfiguration_getKeysHidden(android_app->config),
      AConfiguration_getNavHidden(android_app->config),
      AConfiguration_getSdkVersion(android_app->config),
      AConfiguration_getScreenSize(android_app->config),
      AConfiguration_getScreenLong(android_app->config),
      AConfiguration_getUiModeType(android_app->config),
      AConfiguration_getUiModeNight(android_app->config));
}

void android_app_pre_exec_cmd(struct android_app* android_app, int8_t cmd) {
  switch (cmd) {
    case UNUSED_APP_CMD_INPUT_CHANGED:
      LOGV("UNUSED_APP_CMD_INPUT_CHANGED");
      // Do nothing. This can be used in the future to handle AInputQueue
      // natively, like done in NativeActivity.
      break;

    case APP_CMD_INIT_WINDOW:
      LOGV("APP_CMD_INIT_WINDOW");
      pthread_mutex_lock(&android_app->mutex);
      android_app->window = android_app->pendingWindow;
      pthread_cond_broadcast(&android_app->cond);
      pthread_mutex_unlock(&android_app->mutex);
      break;

    case APP_CMD_TERM_WINDOW:
      LOGV("APP_CMD_TERM_WINDOW");
      pthread_cond_broadcast(&android_app->cond);
      break;

    case APP_CMD_RESUME:
    case APP_CMD_START:
    case APP_CMD_PAUSE:
    case APP_CMD_STOP:
      LOGV("activityState=%d", cmd);
      pthread_mutex_lock(&android_app->mutex);
      android_app->activityState = cmd;
      pthread_cond_broadcast(&android_app->cond);
      pthread_mutex_unlock(&android_app->mutex);
      break;

    case APP_CMD_CONFIG_CHANGED:
      LOGV("APP_CMD_CONFIG_CHANGED");
            AConfiguration_fromAssetManager(
                android_app->config, android_app->activity->assetManager);
      print_cur_config(android_app);
      break;

    case APP_CMD_DESTROY:
      LOGV("APP_CMD_DESTROY");
      android_app->destroyRequested = 1;
      break;
  }
}

void android_app_post_exec_cmd(struct android_app* android_app, int8_t cmd) {
  switch (cmd) {
    case APP_CMD_TERM_WINDOW:
      LOGV("APP_CMD_TERM_WINDOW");
      pthread_mutex_lock(&android_app->mutex);
      android_app->window = NULL;
      pthread_cond_broadcast(&android_app->cond);
      pthread_mutex_unlock(&android_app->mutex);
      break;

    case APP_CMD_SAVE_STATE:
      LOGV("APP_CMD_SAVE_STATE");
      pthread_mutex_lock(&android_app->mutex);
      android_app->stateSaved = 1;
      pthread_cond_broadcast(&android_app->cond);
      pthread_mutex_unlock(&android_app->mutex);
      break;

    case APP_CMD_RESUME:
      free_saved_state(android_app);
      break;
  }
}

void app_dummy() {}

static void android_app_destroy(struct android_app* android_app) {
  LOGV("android_app_destroy!");
  free_saved_state(android_app);
  pthread_mutex_lock(&android_app->mutex);

  AConfiguration_delete(android_app->config);
  android_app->destroyed = 1;
  pthread_cond_broadcast(&android_app->cond);
  pthread_mutex_unlock(&android_app->mutex);
  // Can't touch android_app object after this.
}

static void process_cmd(struct android_app* app,
                        struct android_poll_source* source) {
  int8_t cmd = android_app_read_cmd(app);
  android_app_pre_exec_cmd(app, cmd);
  if (app->onAppCmd != NULL) app->onAppCmd(app, cmd);
  android_app_post_exec_cmd(app, cmd);
}

// This is run on a separate thread (i.e: not the main thread).
static void* android_app_entry(void* param) {
  struct android_app* android_app = (struct android_app*)param;

  LOGV("android_app_entry called");
  android_app->config = AConfiguration_new();
  LOGV("android_app = %p", android_app);
  LOGV("config = %p", android_app->config);
  LOGV("activity = %p", android_app->activity);
  LOGV("assetmanager = %p", android_app->activity->assetManager);
  AConfiguration_fromAssetManager(android_app->config,
                                  android_app->activity->assetManager);

  print_cur_config(android_app);

  android_app->cmdPollSource.id = LOOPER_ID_MAIN;
  android_app->cmdPollSource.app = android_app;
  android_app->cmdPollSource.process = process_cmd;

  ALooper* looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
  ALooper_addFd(looper, android_app->msgread, LOOPER_ID_MAIN,
                ALOOPER_EVENT_INPUT, NULL, &android_app->cmdPollSource);
  android_app->looper = looper;

  pthread_mutex_lock(&android_app->mutex);
  android_app->running = 1;
  pthread_cond_broadcast(&android_app->cond);
  pthread_mutex_unlock(&android_app->mutex);

  android_main(android_app);

  android_app_destroy(android_app);
  return NULL;
}

// --------------------------------------------------------------------
// Native activity interaction (called from main thread)
// --------------------------------------------------------------------

static struct android_app* android_app_create(GameActivity* activity,
                                              void* savedState,
                                              size_t savedStateSize) {
//  struct android_app* android_app = calloc(1, sizeof(struct android_app));
  struct android_app* android_app =
      (struct android_app*)malloc(sizeof(struct android_app));
  memset(android_app, 0, sizeof(struct android_app));
    android_app->activity = activity;

  pthread_mutex_init(&android_app->mutex, NULL);
  pthread_cond_init(&android_app->cond, NULL);

  if (savedState != NULL) {
    android_app->savedState = malloc(savedStateSize);
    android_app->savedStateSize = savedStateSize;
    memcpy(android_app->savedState, savedState, savedStateSize);
  }

  int msgpipe[2];
  if (pipe(msgpipe)) {
    LOGE("could not create pipe: %s", strerror(errno));
    return NULL;
  }
  android_app->msgread = msgpipe[0];
  android_app->msgwrite = msgpipe[1];

  LOGV("Launching android_app_entry in a thread");
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&android_app->thread, &attr, android_app_entry, android_app);

  // Wait for thread to start.
  pthread_mutex_lock(&android_app->mutex);
  while (!android_app->running) {
    pthread_cond_wait(&android_app->cond, &android_app->mutex);
  }
  pthread_mutex_unlock(&android_app->mutex);

  return android_app;
}

static void android_app_write_cmd(struct android_app* android_app, int8_t cmd) {
  if (write(android_app->msgwrite, &cmd, sizeof(cmd)) != sizeof(cmd)) {
    LOGE("Failure writing android_app cmd: %s", strerror(errno));
  }
}

static void android_app_set_window(struct android_app* android_app,
                                   ANativeWindow* window) {
  LOGV("android_app_set_window called");
  pthread_mutex_lock(&android_app->mutex);
  if (android_app->pendingWindow != NULL) {
    android_app_write_cmd(android_app, APP_CMD_TERM_WINDOW);
  }
  android_app->pendingWindow = window;
  if (window != NULL) {
    android_app_write_cmd(android_app, APP_CMD_INIT_WINDOW);
  }
  while (android_app->window != android_app->pendingWindow) {
    pthread_cond_wait(&android_app->cond, &android_app->mutex);
  }
  pthread_mutex_unlock(&android_app->mutex);
}

static void android_app_set_activity_state(struct android_app* android_app,
                                           int8_t cmd) {
  pthread_mutex_lock(&android_app->mutex);
  android_app_write_cmd(android_app, cmd);
  while (android_app->activityState != cmd) {
    pthread_cond_wait(&android_app->cond, &android_app->mutex);
  }
  pthread_mutex_unlock(&android_app->mutex);
}

static void android_app_free(struct android_app* android_app) {
  pthread_mutex_lock(&android_app->mutex);
  android_app_write_cmd(android_app, APP_CMD_DESTROY);
  while (!android_app->destroyed) {
    pthread_cond_wait(&android_app->cond, &android_app->mutex);
  }
  pthread_mutex_unlock(&android_app->mutex);

  close(android_app->msgread);
  close(android_app->msgwrite);
  pthread_cond_destroy(&android_app->cond);
  pthread_mutex_destroy(&android_app->mutex);
  free(android_app);
}

static inline struct android_app* ToApp(GameActivity* activity) {
    return (struct android_app*) activity->instance;
}

static void onDestroy(GameActivity* activity) {
  LOGV("Destroy: %p", activity);
  android_app_free(ToApp(activity));
}

static void onStart(GameActivity* activity) {
  LOGV("Start: %p", activity);
    android_app_set_activity_state(ToApp(activity), APP_CMD_START);
}

static void onResume(GameActivity* activity) {
  LOGV("Resume: %p", activity);
    android_app_set_activity_state(ToApp(activity), APP_CMD_RESUME);
}

static void onSaveInstanceState(GameActivity* activity,
                                SaveInstanceStateRecallback recallback,
                                void* context) {
  LOGV("SaveInstanceState: %p", activity);

  struct android_app* android_app = ToApp(activity);
  void* savedState = NULL;
  pthread_mutex_lock(&android_app->mutex);
  android_app->stateSaved = 0;
  android_app_write_cmd(android_app, APP_CMD_SAVE_STATE);
  while (!android_app->stateSaved) {
    pthread_cond_wait(&android_app->cond, &android_app->mutex);
  }

  if (android_app->savedState != NULL) {
    // Tell the Java side about our state.
        recallback((const char*)android_app->savedState,
                   android_app->savedStateSize, context);
    // Now we can free it.
    free(android_app->savedState);
    android_app->savedState = NULL;
    android_app->savedStateSize = 0;
  }

  pthread_mutex_unlock(&android_app->mutex);
}

static void onPause(GameActivity* activity) {
  LOGV("Pause: %p", activity);
  android_app_set_activity_state(ToApp(activity), APP_CMD_PAUSE);
}

static void onStop(GameActivity* activity) {
  LOGV("Stop: %p", activity);
  android_app_set_activity_state(ToApp(activity), APP_CMD_STOP);
}

static void onConfigurationChanged(GameActivity* activity) {
  LOGV("ConfigurationChanged: %p", activity);
  android_app_write_cmd(ToApp(activity), APP_CMD_CONFIG_CHANGED);
}

static void onContentRectChanged(GameActivity* activity, const ARect* r) {
    LOGV("ContentRectChanged: l=%d,t=%d,r=%d,b=%d", r->left, r->top, r->right,
         r->bottom);
    struct android_app* android_app = ToApp(activity);
    pthread_mutex_lock(&android_app->mutex);
    android_app->contentRect = *r;
    pthread_mutex_unlock(&android_app->mutex);
    android_app_write_cmd(ToApp(activity), APP_CMD_CONTENT_RECT_CHANGED);
}

static void onTrimMemory(GameActivity* activity, int level) {
  LOGV("TrimMemory: %p %d", activity, level);
  android_app_write_cmd(ToApp(activity), APP_CMD_LOW_MEMORY);
}

static void onWindowFocusChanged(GameActivity* activity, bool focused) {
  LOGV("WindowFocusChanged: %p -- %d", activity, focused);
    android_app_write_cmd(ToApp(activity),
                          focused ? APP_CMD_GAINED_FOCUS : APP_CMD_LOST_FOCUS);
}

static void onNativeWindowCreated(GameActivity* activity,
                                  ANativeWindow* window) {
  LOGV("NativeWindowCreated: %p -- %p", activity, window);
  android_app_set_window(ToApp(activity), window);
}

static void onNativeWindowDestroyed(GameActivity* activity,
                                    ANativeWindow* window) {
  LOGV("NativeWindowDestroyed: %p -- %p", activity, window);
  android_app_set_window(ToApp(activity), NULL);
}

static void onNativeWindowRedrawNeeded(GameActivity* activity,
                                       ANativeWindow* window) {
    LOGV("NativeWindowRedrawNeeded: %p -- %p", activity, window);
    android_app_write_cmd(ToApp(activity), APP_CMD_WINDOW_REDRAW_NEEDED);
}

static void onNativeWindowResized(GameActivity* activity,
                                  ANativeWindow* window) {
    LOGV("NativeWindowResized: %p -- %p", activity, window);
    android_app_write_cmd(ToApp(activity), APP_CMD_WINDOW_RESIZED);
}

static void onTouchEvent(GameActivity* activity,
                         const GameActivityMotionEvent* event) {
  struct android_app* android_app = ToApp(activity);
  pthread_mutex_lock(&android_app->mutex);

  struct android_input_buffer* inputBuffer = &android_app->inputBuffers[android_app->currentInputBuffer];

  // Add to the list of active motion events
  if (inputBuffer->motionEventsCount < NATIVE_APP_GLUE_MAX_NUM_MOTION_EVENTS) {
    int new_ix = inputBuffer->motionEventsCount;
    memcpy(&inputBuffer->motionEvents[new_ix], event, sizeof(GameActivityMotionEvent));
    ++inputBuffer->motionEventsCount;
  }
  pthread_mutex_unlock(&android_app->mutex);
}

struct android_input_buffer* android_app_swap_input_buffers(struct android_app* android_app) {
  pthread_mutex_lock(&android_app->mutex);

  struct android_input_buffer* inputBuffer = &android_app->inputBuffers[android_app->currentInputBuffer];

  if (inputBuffer->motionEventsCount == 0 && inputBuffer->keyEventsCount == 0) {
    inputBuffer = NULL;
  } else {
    android_app->currentInputBuffer = (android_app->currentInputBuffer + 1) % NATIVE_APP_GLUE_MAX_INPUT_BUFFERS;
  }

  pthread_mutex_unlock(&android_app->mutex);

  return inputBuffer;
}

void android_app_clear_motion_events(struct android_input_buffer* inputBuffer) {
  inputBuffer->motionEventsCount = 0;
}

static void onKey(GameActivity* activity, const GameActivityKeyEvent* event) {
  struct android_app* android_app = ToApp(activity);
  pthread_mutex_lock(&android_app->mutex);

  struct android_input_buffer* inputBuffer = &android_app->inputBuffers[android_app->currentInputBuffer];

  // Add to the list of active key down events
  if (inputBuffer->keyEventsCount < NATIVE_APP_GLUE_MAX_NUM_KEY_EVENTS) {
    int new_ix = inputBuffer->keyEventsCount;
    memcpy(&inputBuffer->keyEvents[new_ix], event, sizeof(GameActivityKeyEvent));
    ++inputBuffer->keyEventsCount;
  }

  pthread_mutex_unlock(&android_app->mutex);
}

void android_app_clear_key_events(struct android_input_buffer* inputBuffer) {
  inputBuffer->keyEventsCount = 0;
}

static void onTextInputEvent(GameActivity* activity,
                             const GameTextInputState* state) {
  struct android_app* android_app = ToApp(activity);
  pthread_mutex_lock(&android_app->mutex);

  android_app->textInputState = 1;
  pthread_mutex_unlock(&android_app->mutex);
}

static void onWindowInsetsChanged(GameActivity* activity) {
    LOGV("WindowInsetsChanged: %p", activity);
    android_app_write_cmd(ToApp(activity), APP_CMD_WINDOW_INSETS_CHANGED);
}

JNIEXPORT
void GameActivity_onCreate(GameActivity* activity, void* savedState,
                           size_t savedStateSize) {
  LOGV("Creating: %p", activity);
  activity->callbacks->onDestroy = onDestroy;
  activity->callbacks->onStart = onStart;
  activity->callbacks->onResume = onResume;
  activity->callbacks->onSaveInstanceState = onSaveInstanceState;
  activity->callbacks->onPause = onPause;
  activity->callbacks->onStop = onStop;
  activity->callbacks->onTouchEvent = onTouchEvent;
  activity->callbacks->onKeyDown = onKey;
  activity->callbacks->onKeyUp = onKey;
  activity->callbacks->onTextInputEvent = onTextInputEvent;
  activity->callbacks->onConfigurationChanged = onConfigurationChanged;
  activity->callbacks->onTrimMemory = onTrimMemory;
  activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
  activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
  activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onNativeWindowRedrawNeeded =
        onNativeWindowRedrawNeeded;
  activity->callbacks->onNativeWindowResized = onNativeWindowResized;
  activity->callbacks->onContentRectChanged = onContentRectChanged;
    activity->callbacks->onWindowInsetsChanged = onWindowInsetsChanged;
  LOGV("Callbacks set: %p", activity->callbacks);

    activity->instance =
        android_app_create(activity, savedState, savedStateSize);
}
