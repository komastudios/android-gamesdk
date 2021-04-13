// Native entry points for Test Vulkan Renderer.
#include <jni.h>

#include "test-vulkan-renderer.h"

namespace {
istresser_testvulkanrenderer::TestVulkanRenderer *test_vulkan_renderer;
}  // namespace

extern "C" JNIEXPORT jlong JNICALL
Java_net_jimblackler_istresser_MainActivity_vkAlloc(JNIEnv *env, jclass clazz,
                                                    jlong size) {
  if (test_vulkan_renderer == NULL) {
    test_vulkan_renderer =
        new istresser_testvulkanrenderer::TestVulkanRenderer();
  }
  return test_vulkan_renderer->Allocate(size);
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_vkRelease(JNIEnv *env,
                                                      jclass clazz) {
  if (test_vulkan_renderer != NULL) {
    test_vulkan_renderer->Release();
  }
}