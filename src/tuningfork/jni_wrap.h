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

class Object {
  public:
    LocalObject obj_;
    Object(const char* className, const char* ctorSig, ...) {
        va_list argptr;
        va_start(argptr, ctorSig);
        obj_ = NewObjectV(className, ctorSig, argptr);
        va_end(argptr);
    }
    Object(LocalObject&& o) : obj_(std::move(o)) {}
    Object(jobject o) : obj_(o) {
        if (!RawExceptionCheck()) {
            obj_.Cast(); // Cast to the object's own class.
        }
    }
    bool valid() const {
        return !obj_.ClassIsNull();
    }
    void CallVVMethod(const char* name) {
        obj_.CallVoidMethod(name, "()V");
    }
    void CallIVMethod(const char* name, int a) {
        obj_.CallVoidMethod(name, "(I)V", a);
    }
    int CallVIMethod(const char* name) {
        return obj_.CallIntMethod(name, "()I");
    }
    void CallZVMethod(const char* name, bool a) {
        obj_.CallVoidMethod(name, "(Z)V", a);
    }
    void CallSVMethod(const char* name, const char* a) {
        obj_.CallVoidMethod(name, "(Ljava/lang/String;)V",
                       String(a).J());
    }
    String CallVSMethod(const char* name) {
        return obj_.CallStringMethod(name, "()Ljava/lang/String;");
    }
    void CallSSVMethod(const char* name, const char* a, const char* b) {
        obj_.CallVoidMethod(name, "(Ljava/lang/String;Ljava/lang/String;)V",
                            String(a).J(), String(b).J());
    }
    Object CallAOMethod(const char* name, const char* returnClass) {
        std::stringstream str;
        str << "()[L" << returnClass << ";";
        jobject o = obj_.CallObjectMethod(name, str.str().c_str());
        return Object(o);
    }
    Object CallVOMethod(const char* name, const char* returnClass) {
        std::stringstream str;
        str << "()L" << returnClass << ";";
        jobject o = obj_.CallObjectMethod(name, str.str().c_str());
        return Object(o);
    }
    Object CallSIOMethod(const char* name, const char* a, int b, const char* returnClass) {
        std::stringstream str;
        str << "(Ljava/lang/String;I)L" << returnClass << ";";
        jobject o = obj_.CallObjectMethod(name, str.str().c_str(), String(a).J(), b);
        return Object(o);
    }
};

namespace io {

class OutputStream : public Object {
  public:
    OutputStream(Object&& o) : Object(std::move(o)) {}
    void close() {
        CallVVMethod("close");
    }
};

class OutputStreamWriter : public Object {
  public:
    OutputStreamWriter(OutputStream& o, const std::string& s)
      : Object("java/io/OutputStreamWriter", "(Ljava/io/OutputStream;Ljava/lang/String;)V",
               (jobject)o.obj_,
               String(s.c_str()).J()) {}
};

class Writer : public Object {
  public:
    Writer(Object&& o) : Object(std::move(o)) {}
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
            : Writer(Object("java/io/BufferedWriter",
                            "(Ljava/io/Writer;)V", (jobject)w.obj_)) {}
};

class InputStream : public Object {
  public:
    InputStream(Object&& o) : Object(std::move(o)) {}
    void close() {
        CallVVMethod("close");
    }
};

class Reader : public Object {
  public:
    Reader(Object&& o) : Object(std::move(o)) {}
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
            : Reader(Object("java/io/InputStreamReader",
                            "(Ljava/io/InputStream;Ljava/lang/String;)V",
                            (jobject)is.obj_,
                            String(s.c_str()).J())) {}
};

class BufferedReader : public Reader {
  public:
    BufferedReader(const Reader& r)
            : Reader(Object("java/io/BufferedReader", "(Ljava/io/Reader;)V", (jobject)r.obj_))
    {}
};

class File: public Object {
  public:
    File(Object&& o) : Object(std::move(o)) {}
    jni::String getPath() {
        return CallVSMethod("getPath");
    }
};

} // namespace io

namespace util {

class UUID : public Object {
  public:
    UUID(LocalObject&& o) : Object(std::move(o)) {}
    static UUID randomUUID() {
        LocalObject obj(nullptr, Env()->FindClass("java/util/UUID"));
        auto o = obj.CallStaticObjectMethod("randomUUID", "()Ljava/util/UUID;");
        obj.SetObj(o);
        return obj;
    }
    jni::String toString() {
        return CallVSMethod("toString");
    }
};

} // namespace util

namespace net {

class URLConnection : public Object {
  public:
    URLConnection(Object&& o) : Object(std::move(o)) {}
};

class HttpURLConnection : public URLConnection {
  public:
    HttpURLConnection(URLConnection&& u) : URLConnection(std::move(u)) {
        obj_.Cast("java/net/HttpURLConnection");
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
    URL(LocalObject o) : Object(o) {}
    URL(const std::string& s)
            : Object("java/net/URL", "(Ljava/lang/String;)V", String(s.c_str()).J()) {}
    URLConnection openConnection() {
        return URLConnection(Object(obj_.CallObjectMethod("openConnection",
                                                          "()Ljava/net/URLConnection;")));
    }
};

} // namespace net

namespace security {

class MessageDigest : public java::Object {
  public:
    MessageDigest(const std::string& instance) : java::Object(nullptr) {
        LocalObject temp(nullptr, FindClass("java/security/MessageDigest"));
        auto o = temp.CallStaticObjectMethod("getInstance",
                                             "(Ljava/lang/String;)Ljava/security/MessageDigest;",
                                             String(instance.c_str()).J());
        temp.SetObj(o);
        obj_ = std::move(temp);
    }
    std::vector<char> digest(const std::vector<char>& bs) const {
        auto env = Env();
        jbyteArray jbs = env->NewByteArray(bs.size());
        env->SetByteArrayRegion(jbs, 0, bs.size(),
                                       reinterpret_cast<const jbyte*>(bs.data()));
        jbyteArray out = reinterpret_cast<jbyteArray>(obj_.CallObjectMethod("digest",
                                                                            "([B)[B", jbs));
        env->DeleteLocalRef(jbs);
        return GetByteArrayBytesAndDeleteRef(out);
    }
};

} // namespace security

} // namespace java

namespace android {

namespace content {

namespace pm {

class FeatureInfo : public java::Object {
  public:
    FeatureInfo(java::Object&& o) : java::Object(std::move(o)) {
        jni::String jname((jstring)obj_.GetObjectField("name", "Ljava/lang/String;").ObjNewRef());
        if(jname.J() != nullptr) name = jname.C();
        reqGlEsVersion = obj_.GetIntField("reqGlEsVersion");
    }
    static constexpr int GL_ES_VERSION_UNDEFINED = 0x0000000;

    std::string name;
    int reqGlEsVersion;
};

class ApplicationInfo : public java::Object {
  public:
    ApplicationInfo(java::Object&& o) : java::Object(std::move(o)) {}
    int flags() const {
        return obj_.GetIntField("flags");
    }
    static const int FLAG_DEBUGGABLE = 2;
};

class PackageInfo : public java::Object {
  public:
    PackageInfo(java::Object&& o) : java::Object(std::move(o)) {}
    typedef std::vector<char> Signature;
    std::vector<Signature> signatures() const {
        auto env = Env();
        auto jsigs = obj_.GetObjectField("signatures", "[Landroid/content/pm/Signature;");
        jobjectArray sigs = jsigs.AsObjectArray();
        if (sigs == nullptr)  return {};
        int n = env->GetArrayLength(sigs);
        if (n>0) {
            std::vector<std::vector<char>> ret;
            for (int i=0; i<n; ++i) {
                Object sig(env->GetObjectArrayElement(sigs, i));
                jbyteArray bytes = reinterpret_cast<jbyteArray>(
                    sig.obj_.CallObjectMethod("toByteArray", "()[B"));
                ret.push_back(GetByteArrayBytesAndDeleteRef(bytes));
            }
            return ret;
        }
        else
            return {};
    }
    int versionCode() const {
        return obj_.GetIntField("versionCode");
    }
    ApplicationInfo applicationInfo() const {
        auto appInfo = obj_.GetObjectField("applicationInfo",
                                           "[Landroid/content/pm/ApplicationInfo;");
        return ApplicationInfo(std::move(appInfo));
    }
};

class PackageManager : public java::Object {
  public:
    PackageManager(java::Object&& o) : java::Object(std::move(o)) {}
    static constexpr int GET_SIGNATURES = 0x0000040;
    PackageInfo getPackageInfo(const std::string& name, int flags) {
        return CallSIOMethod("getPackageInfo", name.c_str(), flags,
                             "android/content/pm/PackageInfo");
    }
    std::vector<FeatureInfo> getSystemAvailableFeatures() {
        auto env = Env();
        auto jfeatures = CallAOMethod("getSystemAvailableFeatures",
                                                "android/content/pm/FeatureInfo");
        if (jfeatures.obj_.IsNull())  return {};
        jobjectArray features = jfeatures.obj_.AsObjectArray();
        int n = env->GetArrayLength(features);
        std::vector<FeatureInfo> ret;
        if (n>0) {
            for (int i=0; i<n; ++i) {
                FeatureInfo f(Object(env->GetObjectArrayElement(features, i)));
                ret.push_back(std::move(f));
            }
        }
        return ret;
    }
};

} //namespace pm

namespace res {

class AssetManager : public java::Object {
  public:
    AssetManager(java::Object&& o) : java::Object(std::move(o)) {}
};

} // namespace res

class Context : public java::Object {
  public:
    Context(jobject o) : java::Object(o) {}
    pm::PackageManager getPackageManager() {
        return CallVOMethod("getPackageManager", "android/content/pm/PackageManager");
    }
    jni::String getPackageName() {
        return CallVSMethod("getPackageName");
    }
    res::AssetManager getAssets() {
        return CallVOMethod("getAssets", "android/content/res/AssetManager");
    }
    java::io::File getCacheDir() {
        return CallVOMethod("getCacheDir", "java/io/File");
    }
};

} // namespace content

} // namespace android

// A local jni reference to the app context
android::content::Context AppContext();

} // namespace jni

} // namespace tuningfork
