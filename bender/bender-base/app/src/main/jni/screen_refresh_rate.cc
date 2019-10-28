//
// Created by chingtangyu on 10/23/2019.
//
#include <android/log.h>
#include <android/looper.h>
#include <unistd.h>
#include <string>
#include <thread>
#include "screen_refresh_rate.h"
#include "trace.h"

#define  LOG_TAG    "RefreshRate"

const char *window_service = "window";

class NativeHandler {
public:
    static constexpr auto TAG = "NativeHandler";

    static NativeHandler *forCurrentThread() {
        return new NativeHandler;
    }

    template<typename FUNC, typename... ARGS>
    bool post(FUNC &&func, ARGS &&... args) {
        auto callable = new Callable(func, std::forward<ARGS>(args)...);
        if (_looper != nullptr) {
            write(_pipeFDS[1], &callable, sizeof(decltype(callable)));
        } else {
            callable->call();
            delete callable;
        }
        return true;
    }

    NativeHandler(const NativeHandler &) = delete;

    NativeHandler(NativeHandler &&) = delete;

    NativeHandler &operator=(const NativeHandler &) = delete;

    NativeHandler &operator=(NativeHandler &&) = delete;

    virtual ~NativeHandler() {
        ALooper_removeFd(_looper, _pipeFDS[0]);
        ALooper_release(_looper);
        close(_pipeFDS[0]);
        close(_pipeFDS[1]);
    }

private:
    class Callable {
    public:
        void call() {
            if (_function) _function();
        }

        template<typename FUNC, typename... ARGS>
        Callable(FUNC func, ARGS... args) : _function(std::bind(func, args...)) {}

        Callable() = delete;

        Callable(const Callable &) = delete;

        Callable(Callable &&) = delete;

        Callable operator=(const Callable &) = delete;

        Callable operator=(Callable &&) = delete;

        virtual ~Callable() {}

    private:
        std::function<void()> _function;
    };

    NativeHandler() {
        if (pipe(_pipeFDS) != 0) {
            _looper = nullptr;
            return;
        }
        _looper = ALooper_forThread();
        ALooper_acquire(_looper);
        if (ALooper_addFd(_looper, _pipeFDS[0], ALOOPER_POLL_CALLBACK,
                          ALOOPER_EVENT_INPUT, _looperCallback, nullptr) == -1) {
            _looper = nullptr;
            return;
        }
    };
    ALooper *_looper;
    int _pipeFDS[2];

    static int _looperCallback(int fd, int events, void *data) {
        char buf[sizeof(Callable *)];
        read(fd, buf, sizeof(Callable *));
        Callable *callable = *((Callable **) buf);
        callable->call();
        delete callable;
        return 1;
    }
};

std::string getClassName(JNIEnv *env, jobject obj) {
    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
    jobject clsObj = env->CallObjectMethod(obj, mid);
    cls = env->GetObjectClass(clsObj);
    mid = env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
    jstring strObj = (jstring) env->CallObjectMethod(clsObj, mid);
    const char *str = env->GetStringUTFChars(strObj, NULL);
    return std::string(str);
}

jobject getSystemService(JNIEnv *env, jobject obj, const char *service) {
    jstring jstr = env->NewStringUTF("window");
    jclass cls_act = env->GetObjectClass(obj);
    jmethodID GetSystemService = env->GetMethodID(cls_act, "getSystemService",
                                                  "(Ljava/lang/String;)Ljava/lang/Object;");
    return env->CallObjectMethod(obj, GetSystemService, jstr);
}

jobject getDisplay(JNIEnv *env, jobject activity) {
    jobject windowManager = getSystemService(env, activity, window_service);
    jmethodID GetDefaultDisplay = env->GetMethodID(env->GetObjectClass(windowManager),
                                                   "getDefaultDisplay",
                                                   "()Landroid/view/Display;");
    return env->CallObjectMethod(windowManager, GetDefaultDisplay);
}

void getSupportedModes(JNIEnv *env, jobject activity, std::vector<DisplayMode> &displayModes) {
    displayModes.clear();
    jobject display = getDisplay(env, activity);
    jmethodID GetSupportedModes = env->GetMethodID(env->GetObjectClass(display),
                                                   "getSupportedModes",
                                                   "()[Landroid/view/Display$Mode;");
    jobjectArray modes = (jobjectArray) env->CallObjectMethod(display, GetSupportedModes);
    for (int i = 0; i < env->GetArrayLength(modes); ++i) {
        jobject mode = env->GetObjectArrayElement(modes, i);
        jmethodID GetModeId = env->GetMethodID(env->GetObjectClass(mode), "getModeId", "()I");
        jmethodID GetWidth = env->GetMethodID(env->GetObjectClass(mode), "getPhysicalWidth",
                                              "()I");
        jmethodID GetHeight = env->GetMethodID(env->GetObjectClass(mode), "getPhysicalHeight",
                                               "()I");
        jmethodID GetRefreshRate = env->GetMethodID(env->GetObjectClass(mode), "getRefreshRate",
                                                    "()F");
        DisplayMode displayMode;
        displayMode.id = env->CallIntMethod(mode, GetModeId);
        displayMode.width = env->CallIntMethod(mode, GetWidth);
        displayMode.height = env->CallIntMethod(mode, GetHeight);
        displayMode.screenRefreshRate = static_cast<int>(env->CallFloatMethod(mode,
                                                                              GetRefreshRate));
        displayModes.push_back(displayMode);
    }
}

void setScreenRefreshRateImpl(JNIEnv *env, jobject activity, int refreshRate) {
    jobject display = getDisplay(env, activity);
    jmethodID GetMode = env->GetMethodID(env->GetObjectClass(display), "getMode",
                                         "()Landroid/view/Display$Mode;");
    jobject mode = (jobjectArray) env->CallObjectMethod(display, GetMode);
    jmethodID GetWidth = env->GetMethodID(env->GetObjectClass(mode), "getPhysicalWidth",
                                          "()I");
    jmethodID GetHeight = env->GetMethodID(env->GetObjectClass(mode), "getPhysicalHeight",
                                           "()I");
    jmethodID GetRefreshRate = env->GetMethodID(env->GetObjectClass(mode), "getRefreshRate",
                                                "()F");
    int displayWidth = env->CallIntMethod(mode, GetWidth);
    int displayHeight = env->CallIntMethod(mode, GetHeight);
    int displayRefreshRate = static_cast<int>(env->CallFloatMethod(mode, GetRefreshRate));
//    ALOGI("Current Mode: %d %d %d", displayWidth, displayHeight,
//          displayRefreshRate);
    if (displayRefreshRate == refreshRate)
        return;
    std::vector<DisplayMode> modes;
    getSupportedModes(env, activity, modes);
    for (auto mode : modes) {
//        ALOGI("   Considering: %d %d %d", mode.width, mode.height,
//              mode.screenRefreshRate);
        if (mode.width == displayWidth && mode.height == displayHeight &&
            mode.screenRefreshRate == refreshRate) {
            jmethodID GetWindow = env->GetMethodID(env->GetObjectClass(activity),
                                                   "getWindow", "()Landroid/view/Window;");
            jobject window = env->CallObjectMethod(activity, GetWindow);
            jmethodID GetAttributes = env->GetMethodID(env->GetObjectClass(window),
                                                       "getAttributes",
                                                       "()Landroid/view/WindowManager$LayoutParams;");
            jobject layoutParameters = env->CallObjectMethod(window, GetAttributes);
            env->SetIntField(layoutParameters,
                             env->GetFieldID(env->GetObjectClass(layoutParameters),
                                             "preferredDisplayModeId", "I"), mode.id);
            jmethodID SetAttributes = env->GetMethodID(env->GetObjectClass(window),
                                                       "setAttributes",
                                                       "(Landroid/view/WindowManager$LayoutParams;)V");
            env->CallVoidMethod(window, SetAttributes, layoutParameters);
//            ALOGI("Preferred refresh rate set to %dhz", refreshRate);
        }
    }
}

int getScreenRefreshRate(JNIEnv *env, jobject activity) {
    jobject display = getDisplay(env, activity);
    jmethodID GetRefreshRate = env->GetMethodID(env->GetObjectClass(display), "getRefreshRate",
                                                "()F");
    return static_cast<int>(env->CallFloatMethod(display, GetRefreshRate));
}

void setScreenRefreshRate(JNIEnv *env, jobject activity, int refreshRate) {
    jobject globalActivity = env->NewGlobalRef(activity);
    auto handler = NativeHandler::forCurrentThread();
    handler->post([](JNIEnv *env, jobject activity, int refreshRate) {
        setScreenRefreshRateImpl(env, activity, refreshRate);
        env->DeleteGlobalRef(activity);
    }, env, globalActivity, refreshRate);
}

void setScreenRefreshRateMainThread(JNIEnv *env, jobject activity, int refreshRate) {
    setScreenRefreshRateImpl(env, activity, refreshRate);
}

