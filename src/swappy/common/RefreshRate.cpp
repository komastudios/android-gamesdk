#include <android/log.h>
#include <android/looper.h>

#include <unistd.h>

#include <string>

#include "RefreshRate.h"

#include "Trace.h"
#include "Log.h"

#define  LOG_TAG    "RefreshRate"

namespace swappy {

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

    namespace {
        jobject getSystemService(JNIEnv *env, jobject obj, char service[]) {
            jstring jstr = env->NewStringUTF("window");

            jclass cls_act = env->GetObjectClass(obj);
            jmethodID GetSystemService = env->GetMethodID(cls_act, "getSystemService",
                                                          "(Ljava/lang/String;)Ljava/lang/Object;");

            return env->CallObjectMethod(obj, GetSystemService, jstr);
        }

        jobject getDisplay(JNIEnv *env, jobject activity) {
            jobject windowManager = getSystemService(env, activity, (char *) "window");

            jmethodID GetDefaultDisplay = env->GetMethodID(env->GetObjectClass(windowManager),
                                                           "getDefaultDisplay",
                                                           "()Landroid/view/Display;");
            return env->CallObjectMethod(windowManager, GetDefaultDisplay);
        }

        void getSupportedModes(JNIEnv *env, jobject activity, std::vector <DisplayMode> &displayModes) {
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
                displayMode.refreshRate = static_cast<int>(env->CallFloatMethod(mode, GetRefreshRate));

                displayModes.push_back(displayMode);
            }
        }

        void setRefreshRate(JNIEnv *env, jobject activity, int refreshRate) {
            static int currentPreferredRefreshRate = 0;

            if (currentPreferredRefreshRate == refreshRate) {
                return;
            }

            currentPreferredRefreshRate = refreshRate;

            jobject display = getDisplay(env, activity);
            jmethodID GetMode = env->GetMethodID(env->GetObjectClass(display), "getMode",
                                                 "()Landroid/view/Display$Mode;");
            jobject mode = (jobjectArray) env->CallObjectMethod(display, GetMode);

            jmethodID GetWidth = env->GetMethodID(env->GetObjectClass(mode), "getPhysicalWidth",
                                                  "()I");
            jmethodID GetHeight = env->GetMethodID(env->GetObjectClass(mode), "getPhysicalHeight",
                                                   "()I");

            int displayWidth = env->CallIntMethod(mode, GetWidth);
            int displayHeight = env->CallIntMethod(mode, GetHeight);

            std::vector <DisplayMode> modes;
            getSupportedModes(env, activity, modes);

            for (auto mode : modes) {
                ALOGI("   Considering: %d %d %d", mode.width, mode.height,
                     mode.refreshRate);
                if (mode.width == displayWidth && mode.height == displayHeight &&
                    mode.refreshRate == refreshRate) {
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

                    ALOGI("Preferred refresh rate set to %dhz", refreshRate);
                }
            }
        }
    }

    int getRefreshRate(JNIEnv *env, jobject activity) {
        jobject display = getDisplay(env, activity);
        jmethodID GetRefreshRate = env->GetMethodID(env->GetObjectClass(display), "getRefreshRate",
                                                    "()F");
        return static_cast<int>(env->CallFloatMethod(display, GetRefreshRate));
    }
}