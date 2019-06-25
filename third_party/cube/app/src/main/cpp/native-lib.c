#include <jni.h>
#include <android/native_window_jni.h>
#include <pthread.h>

#include "cube.h"

static struct android_app_state state;
static ANativeActivity activity;
static pthread_t thread;

static void *startCubes(void *state_void_ptr)
{
    struct android_app_state* state = (struct android_app_state*)state_void_ptr;
    state->looper = ALooper_prepare(0);
    state->running = true;
    main_loop(state);
    state->running = false;
    state->destroyRequested = false;
    return NULL;
}


JNIEXPORT void JNICALL
Java_com_samples_cube_CubeActivity_nStartCube(JNIEnv *env, jobject clazz, jobject surface) {
    if (!surface) {
        return;
    }
    state.window = ANativeWindow_fromSurface(env, surface);
    state.clazz = (jobject) (*env)->NewGlobalRef(env, clazz);
    (*env)->GetJavaVM(env, &state.vm);

    pthread_create(&thread, NULL, startCubes, &state);
}

JNIEXPORT void JNICALL
Java_com_samples_cube_CubeActivity_nStopCube(JNIEnv *env, jobject clazz) {
    if (state.running) {
        state.destroyRequested = true;
        pthread_join(thread, NULL);
    }
}

JNIEXPORT void JNICALL
Java_com_samples_cube_CubeActivity_nUpdateGpuWorkload(JNIEnv *env, jobject clazz, jint new_workload) {
    update_gpu_workload(new_workload);
}
