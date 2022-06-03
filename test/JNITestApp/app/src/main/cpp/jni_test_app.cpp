#include <jni.h>
#include <string>
#include <sstream>

#include "jni/jni_wrap.h"
#include "tuningfork_utils.h"

using namespace tuningfork::jni;

namespace {

bool operator==(const std::vector<char>& as, const std::vector<char>& bs) {
    if (as.size()!=bs.size()) return false;
    auto ib = bs.begin();
    for(char a: as) {
        if (a!=*ib++) return false;
    }
    return true;
}
std::ostream& operator<<(std::ostream& o, const std::vector<char>& as) {
    o << '[';
    for(char a: as) {
        o << (int)a << ',';
    }
    o << ']';
    return o;
}
class Test {
public:
    jobject ctx_;
    Helper jni_;
    static std::stringstream error_stream;

    Test(JNIEnv *env, jobject ctx) : jni_(env, ctx), ctx_(ctx) {}

    virtual void run() = 0;
    // Check for exceptions
    bool Check(const std::string &name) {
        std::string exception_msg;
        if (jni_.CheckForException(exception_msg)) {
            error_stream << "Exception in " << name << " : " << exception_msg << '\n';
            return false;
        }
        return true;
    }
    // Check for exceptions and result equality
    template<class T>
    bool Check(const std::string &name, const T &lhs, const T &rhs) {
        if (!Check(name)) return false;
        if (lhs == rhs) return true;
        error_stream << '\'' << name << "\' failed : ";
        error_stream << lhs << " != " << rhs << '\n';
        return false;
    }
    bool Check(const std::string &name, const char* lhs, const char* rhs) {
        return Check<std::string>(name, lhs, rhs);
    }
    bool Assert(const std::string msg, bool test) {
        if (!test) {
            error_stream << '\'' << msg << "\' failed\n";
            return false;
        }
        return true;
    }
    void Log(const std::string& s) {
        error_stream << s << '\n';
    }

    static std::string Errors() {
        return error_stream.str();
    }

};

std::stringstream Test::error_stream;

class ContextTest : public Test {
public:
    ContextTest(JNIEnv *env, jobject ctx) : Test(env, ctx) {}
    void run() {
        android::content::Context context(ctx_, jni_);
        Check("context.getPackageName()", context.getPackageName().C(),
            "com.google.jnitestapp");
        Check("context.getClass().getName()", context.getClass().getName().C(),
            "android.app.ContextImpl"); // NB this is the concrete class
    }
};

class PackageManagerTest : public Test {
    jbyteArray signature_bytes_;
public:
    PackageManagerTest(JNIEnv *env, jobject ctx, jbyteArray signature_bytes) : Test(env, ctx),
        signature_bytes_(signature_bytes) {}

    void run() {
        using namespace android::content::pm;
        android::content::Context context(ctx_, jni_);
        auto manager = context.getPackageManager();
        if (!Check("context.getPackageManager()")) return;
        auto info = manager.getPackageInfo(context.getPackageName().C(), 0);
        if (!Check("manager.getPackageInfo(..., 0)")) return;
        if (!Check("info.versionCode()", info.versionCode(), 7)) return;
        if (!Assert("info.signatures() is empty",
            info.signatures()==std::vector<PackageInfo::Signature>({}))) return;

        auto info_w_sigs = manager.getPackageInfo(context.getPackageName().C(),
            PackageManager::GET_SIGNATURES);
        if (!Check("manager.getPackageInfo(...,GET_SIGNATURES)")) return;
        auto sigs = info_w_sigs.signatures();
        if (!Assert("info.signatures() is not empty", !sigs.empty())) return;
        if (!Check("sigs[0] is expected signature",
            sigs[0], jni_.GetByteArrayBytes(signature_bytes_))) return;
    }
};

class MessageDigestTest : public Test {
    jbyteArray signature_bytes_;
    jstring signature_hex_;
public:
    MessageDigestTest(JNIEnv *env, jobject ctx, jbyteArray sig_bytes, jstring sig_hex)
      : Test(env, ctx),  signature_bytes_(sig_bytes), signature_hex_(sig_hex) {}
    void run() {
        using namespace java::security;
        MessageDigest md("SHA1", jni_);
        Check("MessageDigest constructor");
        auto d = md.digest(jni_.GetByteArrayBytes(signature_bytes_));
        Check("digest is expected value", tuningfork::Base16(d).c_str(), String(jni_.env(), signature_hex_).C());
    }
};

} // anonymous namespace

extern "C" JNIEXPORT jstring JNICALL
Java_com_google_jnitestapp_NativeTest_run(JNIEnv* env, jobject this_, jobject ctx, jbyteArray sig,
    jstring sig_hex) {

    {
        ContextTest t(env, ctx);
        t.run();
    }
    {
        PackageManagerTest t(env, ctx, sig);
        t.run();
    }
    {
        MessageDigestTest t(env, ctx, sig, sig_hex);
        t.run();
    }
    return env->NewStringUTF(Test::Errors().c_str());
}
