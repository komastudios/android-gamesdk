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

#include <sstream>

#define LOG_TAG "TuningFork:Web"
#include "Log.h"

namespace tuningfork {

#define CHECK_FOR_EXCEPTION if (jni.CheckForException(exception_msg)) { \
      ALOGW("%s", exception_msg.c_str()); return TFERROR_JNI_EXCEPTION; }

WebRequest::WebRequest(JNIEnv* env, jobject context, const std::string& uri,
                       const std::string& api_key, int timeout_ms) :
        orig_env_(env), thread_env_(nullptr), context_(context), uri_(uri),
        api_key_(api_key), timeout_ms_(timeout_ms) {
    orig_env_->GetJavaVM(&vm_);
    context_ = orig_env_->NewGlobalRef(context_);
}
WebRequest::WebRequest(const WebRequest& rq) :  vm_(rq.vm_), orig_env_(rq.orig_env_),
                                                thread_env_(rq.thread_env_), uri_(rq.uri_),
                                                api_key_(rq.api_key_), timeout_ms_(rq.timeout_ms_){
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
    JNIHelper jni(env, context_);
    std::string exception_msg;
    // url = new URL(uri)
    jni::String urlStr = jni.NewString(uri_);
    auto url = jni.NewObject("java/net/URL", "(Ljava/lang/String;)V", urlStr.J());
    CHECK_FOR_EXCEPTION; // Malformed URL

    // Open connection and set properties
    // connection = url.openConnection()
    jobject connectionObj = jni.CallObjectMethod(url, "openConnection",
                                                 "()Ljava/net/URLConnection;");
    CHECK_FOR_EXCEPTION;// IOException
    auto connection = jni.Cast(connectionObj, "java/net/HttpURLConnection");
    // connection.setRequestMethod("POST")
    jni.CallVoidMethod(connection, "setRequestMethod", "(Ljava/lang/String;)V",
                       jni.NewString("POST").J());
    // connection.setConnectionTimeout(timeout)
    jni.CallVoidMethod(connection, "setConnectTimeout", "(I)V", timeout_ms_);
    // connection.setReadTimeout(timeout)
    jni.CallVoidMethod(connection, "setReadTimeout", "(I)V", timeout_ms_);
    // connection.setDoOutput(true)
    jni.CallVoidMethod(connection, "setDoOutput", "(Z)V", true);
    // connection.setDoInput(true)
    jni.CallVoidMethod(connection, "setDoInput", "(Z)V", true);
    // connection.setUseCaches(false)
    jni.CallVoidMethod(connection, "setUseCaches", "(Z)V", false);
    // connection.setRequestProperty( name, value)
    if (!api_key_.empty()) {
        jni.CallVoidMethod(connection, "setRequestProperty",
                           "(Ljava/lang/String;Ljava/lang/String;)V",
                           jni.NewString("X-Goog-Api-Key").J(), jni.NewString(api_key_).J());
    }
    jni.CallVoidMethod(connection, "setRequestProperty", "(Ljava/lang/String;Ljava/lang/String;)V",
                       jni.NewString("Content-Type").J(), jni.NewString("application/json").J());

    // Write json request body
    // os = connection.getOutputStream()
    jobject os = jni.CallObjectMethod(connection, "getOutputStream", "()Ljava/io/OutputStream;");
    CHECK_FOR_EXCEPTION; // IOException
    // writer = new BufferedWriter(new OutputStreamWriter(os, "UTF-8"))
    auto osw = jni.NewObject("java/io/OutputStreamWriter",
                             "(Ljava/io/OutputStream;Ljava/lang/String;)V",
                             os, jni.NewString("UTF-8").J());
    auto writer = jni.NewObject("java/io/BufferedWriter", "(Ljava/io/Writer;)V", osw.second);
    // writer.write(json)
    jni.CallVoidMethod(writer, "write", "(Ljava/lang/String;)V",
                       jni.NewString(request_json).J());
    CHECK_FOR_EXCEPTION;// IOException
    // writer.flush()
    jni.CallVoidMethod(writer, "flush", "()V");
    CHECK_FOR_EXCEPTION;// IOException
    // writer.close()
    jni.CallVoidMethod(writer, "close", "()V");
    CHECK_FOR_EXCEPTION;// IOException
    // os.close()
    jni.CallVoidMethod(jni.Cast(os), "close", "()V");
    CHECK_FOR_EXCEPTION;// IOException

    // connection.connect()
    jni.CallVoidMethod(connection, "connect", "()V");
    CHECK_FOR_EXCEPTION;// IOException

    // connection.getResponseCode()
    response_code = jni.CallIntMethod(connection, "getResponseCode", "()I");
    ALOGI("Response code: %d", response_code);
    CHECK_FOR_EXCEPTION;// IOException

    // connection.getResponseMessage()
    auto resp = jni.CallStringMethod(connection, "getResponseMessage",
                                                  "()Ljava/lang/String;");
    ALOGI("Response message: %s", resp.C());
    CHECK_FOR_EXCEPTION;// IOException

    // Read body from input stream
    jobject is = jni.CallObjectMethod(connection, "getInputStream", "()Ljava/io/InputStream;");
    CHECK_FOR_EXCEPTION;// IOException
    auto isr = jni.NewObject("java/io/InputStreamReader",
                             "(Ljava/io/InputStream;Ljava/lang/String;)V",
                             is, jni.NewString("UTF-8").J());
    auto reader = jni.NewObject("java/io/BufferedReader", "(Ljava/io/Reader;)V", isr.second);
    std::stringstream body;
    while (true) {
        auto line = jni.CallStringMethod(reader, "readLine", "()Ljava/lang/String;");
        if (line.J()==nullptr) break;
        body << line.C() << "\n";
    }

    // reader.close()
    jni.CallVoidMethod(reader, "close", "()V");
    // is.close()
    jni.CallVoidMethod(jni.Cast(is), "close", "()V");

    // connection.disconnect()
    jni.CallVoidMethod(connection, "disconnect", "()V");

    response_body = body.str();

    return TFERROR_OK;
}

} // namespace tuningfork
