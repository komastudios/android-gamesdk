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

#pragma once

#include "jni_helper.h"

#include <sstream>

namespace tuningfork {

namespace jni {

namespace java {

class Class;

class Object {
  public:
    Helper::ClassAndObject obj_;
    Helper& jni_;
    Object(Helper& jni, const char* className, const char* ctorSig, ...) : jni_(jni) {
        va_list argptr;
        va_start(argptr, ctorSig);
        obj_ = jni_.NewObjectV(className, ctorSig, argptr);
        va_end(argptr);
    }
    Object(Helper::ClassAndObject o, Helper& jni) : obj_(o), jni_(jni) {}
    Object(jobject o, Helper& jni) : obj_({nullptr,o}), jni_(jni) {
        if (!jni_.RawExceptionCheck()) {
            if (o!=nullptr)
                obj_ = jni_.Cast(o);
        }
    }
    bool valid() const {
        return obj_.clz!=nullptr;
    }
    Class getClass() const;
    void CallVVMethod(const char* name) {
        jni_.CallVoidMethod(obj_, name, "()V");
    }
    void CallIVMethod(const char* name, int a) {
        jni_.CallVoidMethod(obj_, name, "(I)V", a);
    }
    int CallVIMethod(const char* name) {
        return jni_.CallIntMethod(obj_, name, "()I");
    }
    void CallZVMethod(const char* name, bool a) {
        jni_.CallVoidMethod(obj_, name, "(Z)V", a);
    }
    void CallSVMethod(const char* name, const char* a) {
        jni_.CallVoidMethod(obj_, name, "(Ljava/lang/String;)V",
                       jni_.NewString(a).J());
    }
    String CallVSMethod(const char* name) {
        return jni_.CallStringMethod(obj_, name, "()Ljava/lang/String;");
    }
    void CallSSVMethod(const char* name, const char* a, const char* b) {
        jni_.CallVoidMethod(obj_, name, "(Ljava/lang/String;Ljava/lang/String;)V",
                            jni_.NewString(a).J(), jni_.NewString(b).J());
    }
    Object CallVOMethod(const char* name, const char* returnClass) {
        std::stringstream str;
        str << "()L" << returnClass << ";";
        jobject o = jni_.CallObjectMethod(obj_, name, str.str().c_str());
        return Object(o, jni_);
    }
    Object CallSIOMethod(const char* name, const char* a, int b, const char* returnClass) {
        std::stringstream str;
        str << "(Ljava/lang/String;I)L" << returnClass << ";";
        jobject o = jni_.CallObjectMethod(obj_, name, str.str().c_str(), jni_.NewString(a).J(), b);
        return Object(o, jni_);
    }
};

class Class : public Object {
public:
    Class(jclass clz, Helper& jni) : Object({jni.FindClass("java/lang/Class"), clz}, jni) {}
    String getName() {
        return CallVSMethod("getName");
    }
};

namespace io {

class OutputStream : public Object {
  public:
    OutputStream(const Object& o) : Object(o) {}
    void close() {
        CallVVMethod("close");
    }
};

class OutputStreamWriter : public Object {
  public:
    OutputStreamWriter(OutputStream& o, const std::string& s)
      : Object(o.jni_, "java/io/OutputStreamWriter", "(Ljava/io/OutputStream;Ljava/lang/String;)V",
               o.obj_.obj,
               o.jni_.NewString(s.c_str()).J()) {}
};

class Writer : public Object {
  public:
    Writer(const Object& o) : Object(o) {}
    void write(const std::string& s) {
        CallSVMethod("write", s.c_str());
    }
    void flush() {
        CallVVMethod("flush");
    }
    void close() {
        CallVVMethod("close");
    }
};

class BufferedWriter : public Writer {
  public:
    BufferedWriter(const Writer& w)
            : Writer(Object(w.jni_, "java/io/BufferedWriter",
                            "(Ljava/io/Writer;)V", w.obj_.obj)) {}
};

class InputStream : public Object {
  public:
    InputStream(const Object& o) : Object(o) {}
    void close() {
        CallVVMethod("close");
    }
};

class Reader : public Object {
  public:
    Reader(const Object& o) : Object(o) {}
    jni::String readLine() {
        return CallVSMethod("readLine");
    }
    void close() {
        CallVVMethod("close");
    }
};

class InputStreamReader : public Reader {
  public:
    InputStreamReader(const InputStream& is, const std::string& s)
            : Reader(Object(is.jni_, "java/io/InputStreamReader",
                            "(Ljava/io/InputStream;Ljava/lang/String;)V",
                            is.obj_.obj,
                            is.jni_.NewString(s.c_str()).J())) {}
};

class BufferedReader : public Reader {
  public:
    BufferedReader(const Reader& r)
            : Reader(Object(r.jni_, "java/io/BufferedReader", "(Ljava/io/Reader;)V", r.obj_.obj))
    {}
};

} // namespace io

namespace net {

class URLConnection : public Object {
  public:
    URLConnection(const Object& o) : Object(o) {}
};

class HttpURLConnection : public URLConnection {
  public:
    HttpURLConnection(const URLConnection& u) : URLConnection(u) {
        obj_ = jni_.Cast(obj_.obj, "java/net/HttpURLConnection");
    }
    void setRequestMethod(const std::string& method) {
        CallSVMethod("setRequestMethod", method.c_str());
    }
    void setConnectTimeout(int t) {
        CallIVMethod("setConnectTimeout", t);
    }
    void setReadTimeout(int timeout) {
        CallIVMethod("setReadTimeout", timeout);
    }
    void setDoOutput(bool d) {
        CallZVMethod("setDoOutput", d);
    }
    void setDoInput(bool d) {
        CallZVMethod("setDoInput", d);
    }
    void setUseCaches(bool d) {
        CallZVMethod("setUseCaches", d);
    }
    void setRequestProperty(const std::string& name, const std::string& value) {
        CallSSVMethod("setRequestProperty", name.c_str(), value.c_str());
    }
    io::OutputStream getOutputStream() {
        return CallVOMethod("getOutputStream", "java/io/OutputStream");
    }
    void connect() {
        CallVVMethod("connect");
    }
    void disconnect() {
        CallVVMethod("disconnect");
    }
    int getResponseCode() {
        return CallVIMethod("getResponseCode");
    }
    jni::String getResponseMessage() {
        return CallVSMethod("getResponseMessage");
    }
    io::InputStream getInputStream() {
        return CallVOMethod("getInputStream", "java/io/InputStream");
    }
};

class URL : public Object {
  public:
    URL(jni::Helper::ClassAndObject o, jni::Helper& jni) : Object(o, jni) {}
    URL(const std::string& s, jni::Helper& jni)
            : Object(jni, "java/net/URL", "(Ljava/lang/String;)V", jni.NewString(s).J()) {}
    URLConnection openConnection() {
        return URLConnection(Object(jni_.CallObjectMethod(obj_, "openConnection",
                                                          "()Ljava/net/URLConnection;"), jni_));
    }
};

} // namespace net

namespace security {

class MessageDigest : public java::Object {
  public:
    MessageDigest(const std::string& instance, Helper& jni) : java::Object(nullptr, jni) {
        jclass clz = jni.FindClass("java/security/MessageDigest");
        obj_.clz = clz;
        obj_.obj = jni.CallStaticObjectMethod(obj_, "getInstance",
                                            "(Ljava/lang/String;)Ljava/security/MessageDigest;",
                                            jni.NewString(instance).J());
    }
    std::vector<char> digest(const std::vector<char>& bs) const {
        jbyteArray jbs = jni_.env()->NewByteArray(bs.size());
        jni_.env()->SetByteArrayRegion(jbs, 0, bs.size(),
                                       reinterpret_cast<const jbyte*>(bs.data()));
        jbyteArray out = reinterpret_cast<jbyteArray>(jni_.CallObjectMethod(obj_, "digest",
                                                                            "([B)[B", jbs));
        jni_.env()->DeleteLocalRef(jbs);
        return jni_.GetByteArrayBytes(out);
    }
};

} // namespace security

} // namespace java

namespace android {

namespace content {

namespace pm {

class PackageInfo : public java::Object {
  public:
    PackageInfo(const java::Object& o) : java::Object(o) {}
    typedef std::vector<char> Signature;
    std::vector<Signature> signatures() const {
        auto env = jni_.env();
        auto jsigs = jni_.GetObjectField(obj_, "signatures", "[Landroid/content/pm/Signature;");
        jobjectArray sigs = reinterpret_cast<jobjectArray>(jsigs.obj);
        if (sigs == nullptr)  return {};
        int n = env->GetArrayLength(sigs);
        if (n>0) {
            std::vector<std::vector<char>> ret;
            for (int i=0; i<n; ++i) {
                Object sig(env->GetObjectArrayElement(sigs, i), jni_);
                jbyteArray bytes = reinterpret_cast<jbyteArray>(jni_.CallObjectMethod(sig.obj_,
                                                                                      "toByteArray",
                                                                                      "()[B"));
                ret.push_back(jni_.GetByteArrayBytes(bytes));
            }
            return ret;
        }
        else
            return {};
    }
    int versionCode() const {
        return jni_.GetIntField(obj_, "versionCode");
    }
};

class PackageManager : public java::Object {
  public:
    PackageManager(const java::Object& o) : java::Object(o) {}
    static constexpr int GET_SIGNATURES = 0x0000040;
    PackageInfo getPackageInfo(const std::string& name, int flags) {
        return CallSIOMethod("getPackageInfo", name.c_str(), flags,
                             "android/content/pm/PackageInfo");
    }
};

} //namespace pm

class Context : public java::Object {
  public:
    Context(jobject o, Helper& h) : java::Object(o, h) {}
    pm::PackageManager getPackageManager() {
        return CallVOMethod("getPackageManager", "android/content/pm/PackageManager");
    }
    jni::String getPackageName() {
        return CallVSMethod("getPackageName");
    }
};

} // namespace content

} // namespace android

} // namespace jni

} // namespace tuningfork
