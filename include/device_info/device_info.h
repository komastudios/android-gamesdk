#ifndef THIRD_PARTY_ANDROID_GAME_SDK__DEVICE_INFO_INCLUDE_DEVICE_INFO_H_
#define THIRD_PARTY_ANDROID_GAME_SDK__DEVICE_INFO_INCLUDE_DEVICE_INFO_H_

#include <cstdint>
#include <memory>

#include "nano/device_info.pb.h"

namespace androidgamesdk_deviceinfo {

// We are using nano proto C library, which does not take responsibility of
// dynamically allocated memory, such as strings. Therefore we use this data
// structure to keep dynamically allocated memory.
struct ProtoDataHolder {
  // Helper classes, instead of STL.
  // We do not use STL because using it goes over the file size limitations
  // of phonesky.

  // NULL terminated char array.
  struct String {
    std::unique_ptr<char[]> data;

    String() : data(nullptr) {}
    void copy(const char* from);

    // Forbid implicit copy
    String(const String&) = delete;
    void operator=(const String&) = delete;
  };

  // Vector of strings.
  // The allocated memory size is doubled whenever reallocated.
  struct StringVector {
    // Current number of elements.
    size_t size;

    // Maximum number of elements that can be held in the allocated memory.
    size_t sizeMax;

    // Pointer to dynamically allocated memory.
    std::unique_ptr<String[]> data;

    StringVector() : size(0), sizeMax(1), data(new String[sizeMax]()) {}

    void addCopy(const char* newString);
    bool has(const char* element);

    // Forbid implicit copy.
    StringVector(const StringVector&) = delete;
    void operator=(const StringVector&) = delete;
  };

  // Simple array with size info attached.
  template <typename T>
  struct Array {
    size_t size;
    std::unique_ptr<T[]> data;

    Array() : size(0), data(nullptr) {}

    void setSize(size_t newSize) {
      size = newSize;
      data.reset(new T[size]);
    }

    bool includes(const T& value) {
      for (size_t i = 0; i < size; ++i) {
        if (data[i] == value) {
          return true;
        }
      }

      return false;
    }

    // Forbid implicit copy
    Array(const Array&) = delete;
    void operator=(const Array&) = delete;
  };

  // Data fields. Those not listed here are directly
  // written in the proto.

  int cpuIndexMax;
  Array<int64_t> cpuFreqs;
  String cpu_present;
  String cpu_possible;
  StringVector cpu_extension;
  StringVector hardware;

  String ro_build_version_sdk;
  String ro_build_fingerprint;

  struct OpenGl {
    String renderer;
    String vendor;
    String version;
    String shading_language_version;

    StringVector extension;

    // OpenGL constants requiring minimum version 2.0
    Array<int32_t> compTexFormats;
    Array<int32_t> shaderBinFormats;

    Array<float> gl_aliased_line_width_range;
    Array<float> gl_aliased_point_size_range;
    Array<int32_t> gl_max_viewport_dims;

    // OpenGL constants requiring minimum version 3.0
    Array<int32_t> progBinFormats;

    // OpenGL constants requiring minimum version 3.2
    Array<float> gl_multisample_line_width_range;
  } ogl;

  StringVector errors;
};

struct RGBA {
  unsigned char r, g, b, a;
};
struct RGB {
  unsigned char r, g, b;
};

// Size of the OpenGL view used for testing EGL and compressed textures
// rendering.
constexpr int VIEW_WIDTH = 8;
constexpr int VIEW_HEIGHT = VIEW_WIDTH;

// returns number of errors
int createProto(finsky_dfe_GameSdkDeviceInfoWithErrors& proto,
                ProtoDataHolder& dataHolder);

}  // namespace androidgamesdk_deviceinfo

#endif  // THIRD_PARTY_ANDROID_GAME_SDK__DEVICE_INFO_INCLUDE_DEVICE_INFO_H_
