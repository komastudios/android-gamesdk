#include <jni.h>
#include <android/native_window_jni.h>
#include <string>
#include <pthread.h>

#include "cube.c"

extern "C" {

static struct android_app app;
static ANativeActivity activity;

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
Java_com_samples_cube_CubeActivity_nStartCube(JNIEnv *env, jobject clazz, jobject surface) {
    if (!surface) {
        __android_log_print(ANDROID_LOG_ERROR, APP_SHORT_NAME, "NULL surface passed");
        return;
    }
    app.window = ANativeWindow_fromSurface(env, surface);

    activity = ANativeActivity();
    activity.clazz = (jobject) env->NewGlobalRef(clazz);
    env->GetJavaVM(&activity.vm);
    app.activity = &activity;

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
Java_com_samples_cube_CubeActivity_nChangeNumCubes(JNIEnv *env, jobject /* clazz */, jint new_num_cubes) {
    update_cube_count(new_num_cubes);
}

} // extern "C"
