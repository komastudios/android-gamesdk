#ifndef THIRD_PARTY_ANDROID_GAME_SDK_DEVICE_INFO_INCLUDE_TEXTURE_TEST_CASES_H_
#define THIRD_PARTY_ANDROID_GAME_SDK_DEVICE_INFO_INCLUDE_TEXTURE_TEST_CASES_H_
#include <GLES3/gl32.h>
#include "device_info/device_info.h"

namespace androidgamesdk_deviceinfo {

struct TextureTestCase {
  GLenum internalformat;
  GLint width;
  GLint height;
  unsigned long dataSize;
  const void* data;

  RGB expectedPixelsColor;
  RGB allowedPixelsDelta;
  const RGBA* expectedRenderedPixels;
};

TextureTestCase* getCompressedTextureTestCase(GLenum internalformat);

}


#endif  // THIRD_PARTY_ANDROID_GAME_SDK_DEVICE_INFO_INCLUDE_TEXTURE_TEST_CASES_H_
