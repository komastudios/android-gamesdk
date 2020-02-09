#include "android_helper.h"

#include <cassert>

#include <errno.h>

#include <android_native_app_glue.h>

namespace ancer {
namespace vulkan {

static android_app *android_application = nullptr;

void AndroidHelper::Initialize() {
}

ANativeWindow * AndroidHelper::Window() {
  assert(android_application != nullptr);
  return android_application->window;
}

void AndroidHelper::WindowSize(uint32_t & width, uint32_t & height) {
    assert(android_application != nullptr);
    width = ANativeWindow_getWidth(android_application->window);
    height = ANativeWindow_getHeight(android_application->window);
}

// Android fopen stub described at
// http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/#comment-1850768990
static int AndroidRead(void * cookie, char * buf, int size) {
    return AAsset_read((AAsset *)cookie, buf, static_cast<size_t>(size));
}

static int AndroidWrite(void * cookie, const char * buf, int size) {
    return EACCES;  // can't provide write access to the apk
}

static fpos_t AndroidSeek(void * cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset *)cookie, offset, whence);
}

static int AndroidClose(void *cookie) {
    AAsset_close((AAsset *)cookie);
    return 0;
}

FILE * AndroidHelper::Fopen(const char * fname, const char * mode) {
    if(mode[0] == 'w')
        return nullptr;

    assert(android_application != nullptr);
    AAsset * asset = AAssetManager_open(
                                   android_application->activity->assetManager,
                                   fname, 0);
    if(!asset)
        return nullptr;

    return funopen(asset, AndroidRead, AndroidWrite, AndroidSeek,
                   AndroidClose);
}

}
}
