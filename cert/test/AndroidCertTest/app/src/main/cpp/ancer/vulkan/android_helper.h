#ifndef AGDC_ANCER_ANDROIDHELPER_H_
#define AGDC_ANCER_ANDROIDHELPER_H_

#define VK_USE_PLATFORM_ANDROID_KHR 1

#include <cstdio>

#include <vulkan/vulkan.h>

namespace ancer {
namespace vulkan {

struct AndroidHelper {
  static void Initialize();

  static ANativeWindow * Window();

  static void WindowSize(uint32_t & width, uint32_t & height);

  static FILE * Fopen(const char * fname, const char * mode);
};

}
}

#endif
