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

#include "web.h"

#include "jni_wrap.h"

#include <sstream>

#define LOG_TAG "TuningFork:Web"
#include "Log.h"

#include "tuningfork_utils.h"

namespace tuningfork {

WebRequest::WebRequest(JNIEnv* env, jobject context, const std::string& uri,
                       const std::string& api_key, Duration timeout) :
        orig_env_(env), thread_env_(nullptr), context_(context), uri_(uri),
        api_key_(api_key), timeout_(timeout) {
    orig_env_->GetJavaVM(&vm_);
    context_ = orig_env_->NewGlobalRef(context_);
}
WebRequest::WebRequest(const WebRequest& rq) :  vm_(rq.vm_), orig_env_(rq.orig_env_),
                                                thread_env_(rq.thread_env_), uri_(rq.uri_),
                                                api_key_(rq.api_key_), timeout_(rq.timeout_){
    context_ = orig_env_->NewGlobalRef(rq.context_);
}
WebRequest::~WebRequest() {
    orig_env_->DeleteGlobalRef(context_);
}
void WebRequest::AttachToThread() {
    JNIEnv* env;
    vm_->AttachCurrentThread(&env, NULL);
    thread_env_ = env;
}
void WebRequest::DetachFromThread() {
    vm_->DetachCurrentThread();
    thread_env_ = nullptr;
}

TFErrorCode WebRequest::Send(const std::string& request_json,
                 int& response_code, std::string& response_body) {
    ALOGI("Connecting to: %s", uri_.c_str());
    JNIEnv* env = thread_env_ ? thread_env_ : orig_env_;

    using namespace jni;

    Helper jni(env, context_);

    // url = new URL(uri)
    auto url = java::net::URL(uri_, jni);
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION); // Malformed URL

    // Open connection and set properties
    java::net::HttpURLConnection connection(url.openConnection());
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException
    connection.setRequestMethod("POST");
    auto timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout_).count();
    connection.setConnectTimeout(timeout_ms);
    connection.setReadTimeout(timeout_ms);
    connection.setDoOutput(true);
    connection.setDoInput(true);
    connection.setUseCaches(false);
    if (!api_key_.empty()) {
        connection.setRequestProperty( "X-Goog-Api-Key", api_key_);
    }
    connection.setRequestProperty( "Content-Type", "application/json");

    std::string package_name;
    apk_utils::GetVersionCode(env, context_, &package_name);
    if (!package_name.empty())
      connection.setRequestProperty( "X-Android-Package", package_name);
    auto signature = apk_utils::GetSignature(env, context_);
    if (!signature.empty())
      connection.setRequestProperty( "X-Android-Cert", signature);

    // Write json request body
    auto os = connection.getOutputStream();
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION); // IOException
    auto writer = java::io::BufferedWriter(java::io::OutputStreamWriter(os, "UTF-8"));
    writer.write(request_json);
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException
    writer.flush();
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException
    writer.close();
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException
    os.close();
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException

    // Connect and get response
    connection.connect();
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException

    response_code = connection.getResponseCode();
    ALOGI("Response code: %d", response_code);
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException

    auto resp = connection.getResponseMessage();
    ALOGI("Response message: %s", resp.C());
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException

    // Read body from input stream
    auto is = connection.getInputStream();
    CHECK_FOR_JNI_EXCEPTION_AND_RETURN(TFERROR_JNI_EXCEPTION);// IOException
    auto reader = java::io::BufferedReader(java::io::InputStreamReader(is, "UTF-8"));
    std::stringstream body;
    while (true) {
        auto line = reader.readLine();
        if (line.J()==nullptr) break;
        body << line.C() << "\n";
    }

    reader.close();
    is.close();
    connection.disconnect();

    response_body = body.str();

    return TFERROR_OK;
}

} // namespace tuningfork
