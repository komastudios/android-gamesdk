#include <jni.h>
#include <android/native_window_jni.h>
#include <string>
#include <pthread.h>

#include "cube.c"

extern "C" {

struct android_app app;

static void *startCubes(void *app_void_ptr)
{
    android_app* app = (android_app*)app_void_ptr;
    app->looper = ALooper_prepare(0);
    app->running = true;
    android_main2(app);
    app->running = false;
    app->destroyRequested = false;
    return NULL;
}

JNIEXPORT void JNICALL
Java_com_samples_cube_CubeActivity_nStartCube(JNIEnv *env, jobject /* this */, jobject surface) {

    if (!surface) {
        __android_log_print(ANDROID_LOG_ERROR, APP_SHORT_NAME, "cube: NULL surface passed");
        return;
    }
    app.window = ANativeWindow_fromSurface(env, surface);
    pthread_create(&app.thread, NULL, startCubes, &app);
}

JNIEXPORT void JNICALL
Java_com_samples_cube_CubeActivity_nStopCube(JNIEnv *env, jobject /* this */) {
    if (app.running) {
        app.destroyRequested = true;
        pthread_join(app.thread, NULL);
    }
}

JNIEXPORT void JNICALL
Java_com_samples_cube_CubeActivity_nChangeNumCubes(JNIEnv *env, jobject /* this */, jint new_num_cubes) {
    update_cube_count(new_num_cubes);
}



struct busyThread {
    _Atomic bool stop = false;
    pthread_t thread;
};

static void * startThread(void* this_busy_thread) {
    struct busyThread* this_thread = (struct busyThread*)this_busy_thread;

    while(!this_thread->stop) {
        volatile int busyWork = 0;
        for(int i = 0; i < 500; ++i) {
            ++busyWork;
        }
    }

    return NULL;
}

static const size_t MAX_THREADS = 200;
static size_t thread_count = 0;
static busyThread thread_array[MAX_THREADS];

JNIEXPORT void JNICALL
Java_com_samples_cube_CubeActivity_nChangeNumBusyThreads(JNIEnv *env, jobject /* this */, jint new_num_threads) {
    if (thread_count == new_num_threads || new_num_threads > MAX_THREADS || new_num_threads < 0) {
        return;
    }

    if (thread_count > new_num_threads) {
        for(int i = thread_count - 1; i >= new_num_threads; --i) {
            thread_array[i].stop = true;
        }
        size_t original_thread_count = thread_count;
        while (thread_count > new_num_threads) {
            pthread_join(thread_array[thread_count-1].thread, NULL);
            --thread_count;
        }
        __android_log_print(ANDROID_LOG_INFO, APP_SHORT_NAME, "Joined %zu threads. Current thread count: %zu", original_thread_count - thread_count, thread_count);
    } else {
        size_t original_thread_count = thread_count;
        while (thread_count < new_num_threads) {
            thread_array[thread_count].stop = false;
            pthread_create(&thread_array[thread_count].thread, NULL, startThread, &thread_array[thread_count]);
            ++thread_count;
        }
        __android_log_print(ANDROID_LOG_INFO, APP_SHORT_NAME, "Created %zu threads. Current thread count: %zu", thread_count - original_thread_count, thread_count);
    }
}

} // extern "C"
