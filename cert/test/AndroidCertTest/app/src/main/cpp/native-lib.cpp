/*
 * Copyright 2019 The Android Open Source Project
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

#include <cmath>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <android/native_window_jni.h>
#include <jni.h>

#ifdef BUILD_UNIT_TESTS
#include <gtest/gtest.h>
#endif

#include "swappy/swappyGL.h"
#include "swappy/swappyGL_extra.h"

#include "ancer/BaseOperation.hpp"
#include "ancer/SwappyRenderer.hpp"
#include "ancer/Suite.hpp"
#include "ancer/System.hpp"
#include "ancer/util/Error.hpp"
#include "ancer/util/Log.hpp"
#include "ancer/util/FpsCalculator.hpp"
#include "ancer/util/TestResultPrinter.hpp"
#include "ancer/util/Time.hpp"

using namespace ancer;


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"NativeInvoker"};

    // TODO: Move to System?
    // Renderer in general should probably be more generic too...
    std::unique_ptr<swappy::Renderer> _swappy_renderer;

    std::string to_string(jstring jstr, JNIEnv* env) {
        const char* utf = env->GetStringUTFChars(jstr, nullptr);
        std::string str(utf);
        env->ReleaseStringUTFChars(jstr, utf);
        return str;
    }
}

//==================================================================================================

extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_runUnitTests(JNIEnv *env, jclass instance) {
    // TODO: Maybe support a standalone version? There are a lot of tools that support Google Test's
    //  normal output if it's built as an executable, including Visual Studio.
#ifdef BUILD_UNIT_TESTS
    testing::InitGoogleTest();
    testing::UnitTest::GetInstance()->listeners().Append(new ResultPrinter{env});
    return RUN_ALL_TESTS() == 0;
#else
    Log::E(TAG, "Attempting to run unit tests in a build where they aren't being generated.");
    return false;
#endif
}

//==================================================================================================

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_initializeSuite(
        JNIEnv* env, jclass instance, jobject activity) {
    internal::BindJNI(activity);
    internal::InitializeSuite();
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_shutdownSuite(JNIEnv* env, jclass instance) {
    internal::ShutdownSuite();
    internal::UnbindJNI();
}

//==================================================================================================

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    internal::SetJavaVM(vm);
    return JNI_VERSION_1_6;
}

///////////////////////////////////////////////////////////////////////
// Operation management

extern "C" JNIEXPORT jint JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_createOperation(
        JNIEnv* env, jclass instance,
        jstring j_suite_id,
        jstring j_operation_id,
        jint jMode) {
    auto suite_id = to_string(j_suite_id, env);
    auto operation_id = to_string(j_operation_id, env);
    auto mode = jMode == 0
                ? BaseOperation::Mode::DataGatherer
                : BaseOperation::Mode::Stressor;

    return internal::CreateOperation(suite_id, operation_id, mode);
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_startOperation(
        JNIEnv* env, jclass instance,
        jint id,
        jlong duration_millis,
        jstring j_configuration) {
    auto duration = duration_cast<Duration>(Milliseconds{duration_millis});
    auto configuration = to_string(j_configuration, env);
    internal::StartOperation(id, duration, configuration);
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_stopOperation(
        JNIEnv* env, jclass instance,
        jint id) {
    internal::StopOperation(id);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_isOperationStopped(
        JNIEnv* env, jclass instance,
        jint id) {

    return static_cast<jboolean>(internal::OperationIsStopped(id));
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_waitForOperation(
        JNIEnv* env, jclass instance,
        jint id) {
    internal::WaitForOperation(id);
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_onTrimMemory(
        JNIEnv* env, jclass instance,
        jint level) {
    internal::ForEachOperation([level](BaseOperation& op) { op.OnTrimMemory(level); });
    reporting::FlushReportLogQueue();
}

/////////////////////////////////////////////////////////////////////////
//// GLSurfaceViewHostActivity
// TODO: Move into own file

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_glSurfaceViewHost_1ContextReady(
        JNIEnv* env,
        jclass instance) {
    ancer::GetFpsCalculator().Reset();
    internal::ForEachOperation([](BaseOperation& op) { op.OnGlContextReady(); });
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_glSurfaceViewHost_1Draw(
        JNIEnv* env,
        jclass instance) {

    double delta_seconds = ancer::GetFpsCalculator().UpdateFps();

    // clear, and draw all our gl operations
    glClearColor(0, 0, 0, 1);
    glClearDepthf(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    internal::ForEachOperation([delta_seconds](BaseOperation& op) { op.Draw(delta_seconds); });
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_glSurfaceViewHost_1Resize(
        JNIEnv* env,
        jclass instance,
        jint width,
        jint height) {
    glViewport(0, 0, width, height);
    internal::ForEachOperation(
            [width, height](BaseOperation& op) { op.OnGlContextResized(width, height); });
}

/////////////////////////////////////////////////////////////////////////
//// SwappyGLHostActivity
// TODO: Move into swappy renderer

namespace {
    void StartFrameCallback(void*, int, long) {
        ancer::GetFpsCalculator().UpdateFps();
    }

    void SwapIntervalChangedCallback(void*) {
        uint64_t swap_ns = SwappyGL_getSwapIntervalNS();
        Log::I(TAG, "Swappy changed swap interval to %.2fms", swap_ns / 1e6f);
    }
}


extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_swappyGLHost_1Init(
        JNIEnv* env, jclass instance,
        jobject activity) {
    // Get the Renderer instance to create it
    _swappy_renderer = swappy::Renderer::Create();

    SwappyGL_init(env, activity);

    SwappyTracer tracers;
    tracers.preWait = nullptr;
    tracers.postWait = nullptr;
    tracers.preSwapBuffers = nullptr;
    tracers.postSwapBuffers = nullptr;
    tracers.startFrame = StartFrameCallback;
    tracers.userData = nullptr;
    tracers.swapIntervalChanged = SwapIntervalChangedCallback;
    SwappyGL_injectTracer(&tracers);

    // Init is called at end of SwappyGLHostActivity::OnCreate, and all operations have already been created
    internal::SetSwappyRenderer(_swappy_renderer.get());
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_swappyGLHost_1SetSurface(
        JNIEnv* env,
        jclass instance,
        jobject surface,
        jint width,
        jint height) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    _swappy_renderer->SetWindow(window, static_cast<int32_t>(width), static_cast<int32_t>(height));
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_swappyGLHost_1StartRenderer(
        JNIEnv* env,
        jclass instance) {
    ancer::GetFpsCalculator().Reset();
    // start the render thread
    _swappy_renderer->Start();
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_swappyGLHost_1StopRenderer(
        JNIEnv* env,
        jclass instance) {
    // stop the render thread
    _swappy_renderer->Stop();
    internal::SetSwappyRenderer(nullptr);
    _swappy_renderer = nullptr;
    SwappyGL_destroy();
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_swappyGLHost_1ClearSurface(
        JNIEnv* env,
        jclass instance) {
    _swappy_renderer->SetWindow(nullptr, 0, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_swappyGLHost_1SetAutoSwapInterval(
        JNIEnv* env,
        jclass instance,
        jboolean enabled) {
    SwappyGL_setAutoSwapInterval(enabled);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_swappyGLHost_1GetSwappyStats(
        JNIEnv* env,
        jclass instance,
        jint stat,
        jint bin) {
    static bool enabled = false;
    if ( !enabled ) {
        SwappyGL_enableStats(true);
        enabled = true;
    }

    // stats are read one by one, query once per stat
    static SwappyStats stats;
    static int stat_idx = -1;

    if ( stat_idx != stat ) {
        SwappyGL_getStats(&stats);
        stat_idx = stat;
    }

    int64_t value = 0;

    if ( stats.totalFrames ) {
        switch ( stat ) {
        case 0:value = stats.idleFrames[bin];
            break;
        case 1:value = stats.lateFrames[bin];
            break;
        case 2:value = stats.offsetFromPreviousFrame[bin];
            break;
        case 3:value = stats.latencyFrames[bin];
            break;
        default:return static_cast<jint>(stats.totalFrames);
        }

        value = static_cast<int64_t>(std::round(value * 100.0F / stats.totalFrames));
    }

    return static_cast<jint>(value);
}

///////////////////////////////////////////////////////////////////////
// Helper functions

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_openReportFile(
        JNIEnv* env, jclass instance,
        jstring j_report_file) {
    reporting::OpenReportLog(to_string(j_report_file, env));
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_closeReportFile(JNIEnv* env, jclass instance) {
    reporting::CloseReportLog();
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_writeToReportFile(
        JNIEnv* env, jclass instance,
        jstring j_report_str) {
    reporting::WriteToReportLog(to_string(j_report_str, env));
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_getSteadyClockTimeNanos(JNIEnv *env, jclass instance) {
    auto n = ancer::SteadyClock::now();
    return static_cast<jlong>(n.time_since_epoch().count());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_getCpuId(JNIEnv* env, jclass instance) {
    return sched_getcpu();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_getThreadId(JNIEnv* env, jclass instance) {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return env->NewStringUTF(ss.str().c_str());
}

extern "C" JNIEXPORT jdouble JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_getCurrentFps(JNIEnv* env, jclass instance) {
    return ancer::GetFpsCalculator().GetAverageFps();
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_getCurrentMinFrameTimeNs(JNIEnv *env,
                                                                            jclass instance) {
    return duration_cast<Nanoseconds>(ancer::GetFpsCalculator().GetMinFrameTime()).count();
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_google_gamesdk_gamecert_operationrunner_util_NativeInvoker_getCurrentMaxFrameTimeNs(JNIEnv *env,
                                                                            jclass instance) {
    return duration_cast<Nanoseconds>(ancer::GetFpsCalculator().GetMaxFrameTime()).count();
}
