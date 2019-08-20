#include "ge_backend.h"

#define LOG_TAG "TuningFork.GE"
#include "Log.h"

#include "jni_helper.h"

namespace tuningfork {

static bool CheckException(JNIEnv *env) {
    if(env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }
    return false;
}

TFErrorCode GEBackend::Init(JNIEnv* env, jobject context) {

    // TODO(willosborn): Initialize Java JobScheduler or our own C++ mechanism

    return TFERROR_OK;
}

GEBackend::~GEBackend() {}

TFErrorCode GEBackend::Process(const std::string &evt_ser) {

    ALOGI("GEBackend::Process %s",evt_ser.c_str());

    // TODO(willosborn): Add upload of event

    return TFERROR_OK;
}

} //namespace tuningfork {
