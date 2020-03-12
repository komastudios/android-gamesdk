#include "android_helper.h"

#include <cassert>

#include <errno.h>

#include <android_native_app_glue.h>

namespace ancer {
namespace vulkan {

static ANativeWindow *_window;
static ANativeActivity *_activity;

void AndroidHelper::Initialize(ANativeWindow *window, ANativeActivity *activity) {
    _window = window;
    _activity = activity;
}

ANativeWindow * AndroidHelper::Window() {
  assert(_window != nullptr);
  return _window;
}

void AndroidHelper::WindowSize(uint32_t & width, uint32_t & height) {
    assert(_window != nullptr);
    width = ANativeWindow_getWidth(_window);
    height = ANativeWindow_getHeight(_window);
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

    assert(_activity != nullptr);
    AAsset * asset = AAssetManager_open(
                                   _activity->assetManager,
                                   fname, 0);
    if(!asset)
        return nullptr;

    return funopen(asset, AndroidRead, AndroidWrite, AndroidSeek,
                   AndroidClose);
}

}
}
