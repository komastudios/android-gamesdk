#pragma once

#include <string>

namespace apk_utils {

class NativeAsset {
    AAsset* asset;

   public:
    NativeAsset(const char* name);
    NativeAsset(NativeAsset&& a);
    NativeAsset& operator=(NativeAsset&& a);

    NativeAsset(const NativeAsset& a) = delete;
    NativeAsset& operator=(const NativeAsset& a) = delete;

    ~NativeAsset();
    bool IsValid();
    operator AAsset*();
};

}  // namespace apk_utils