/*
 * Copyright 2020 The Android Open Source Project
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

#include "System.Gpu.hpp"

#include <memory>
#include <regex>

#include <ancer/System.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/FpsCalculator.hpp>
#include <ancer/util/GLHelpers.hpp>
#include <ancer/util/JNIHelpers.hpp>
#include <ancer/util/Log.hpp>
#include <ancer/util/Texture.hpp>
#include <ancer/util/texture/Jpg.hpp>
#include <ancer/util/texture/Png.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"SystemGpu"};
}

//--------------------------------------------------------------------------------------------------

GLuint ancer::CreateProgram(const std::string &vtx_file_name, const std::string &frg_file_name) {
  std::string vtx_src = LoadText(vtx_file_name);
  std::string frag_src = LoadText(frg_file_name);
  if (!vtx_src.empty() && !frag_src.empty()) {
    return glh::CreateProgramSrc(vtx_src.c_str(), frag_src.c_str());
  }
  return 0;
}

GLuint ancer::BindNewTexture2D() {
  GLuint texture_id = 0;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  return texture_id;
}

GLuint ancer::LoadTexture(const std::string &asset_path, int32_t *out_width,
                          int32_t *out_height, bool *has_alpha) {
  GLuint texture_id;

  static const std::regex ASSET_PATH_REGEX{R"(((.*)\/)?([^\/\.]*)\.([^.]*))"};
  std::smatch asset_path_parts;

  if (std::regex_match(asset_path, asset_path_parts, ASSET_PATH_REGEX)) {
    auto parts_count = asset_path_parts.size();
    auto extension = asset_path_parts[parts_count - 1].str();
    auto stem = asset_path_parts[parts_count - 2].str();
    auto relative_path = parts_count > 2 ? asset_path_parts[parts_count - 3].str() : std::string{};

    std::unique_ptr<ancer::Texture> p_texture;

    if (extension == std::string{"png"}) {
      p_texture = std::make_unique<ancer::PngTexture>(relative_path, stem);
    } else if (extension == std::string{"jpg"}) {
      p_texture = std::make_unique<ancer::JpgTexture>(relative_path, stem);
    } // else other formats to support in the future (ASTC, Gzipped ETC2, ...)

    if (p_texture->Load()) {
      *out_width = p_texture->GetWidth();
      *out_height = p_texture->GetHeight();
      if (has_alpha) {
        *has_alpha = p_texture->HasAlpha();
      }
      p_texture->ApplyBitmap(texture_id = ancer::BindNewTexture2D());
    }
  }

  if (texture_id == 0) {
    *out_width = 0;
    *out_height = 0;
    if (has_alpha) {
      *has_alpha = false;
    }
    FatalError(TAG, "LoadTexture: unable to load \"%s\"", asset_path.c_str());
  }

  return texture_id;
}

//--------------------------------------------------------------------------------------------------

FpsCalculator &ancer::GetFpsCalculator() {
  static FpsCalculator fps_calc;
  return fps_calc;
}

//--------------------------------------------------------------------------------------------------

void ancer::BridgeGLContextConfiguration(GLContextConfig src_config,
                                         jobject dst_config) {
  jni::SafeJNICall(
      [src_config, dst_config](jni::LocalJNIEnv *env) {
        jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());

        jclass java_cls = LoadClass(
            env, activity,
            "com/google/gamesdk/gamecert/operationrunner/hosts/"
            "BaseGLHostActivity$GLContextConfiguration");

        jfieldID red_bits = env->GetFieldID(java_cls, "redBits", "I");
        jfieldID green_bits = env->GetFieldID(java_cls, "greenBits", "I");
        jfieldID blue_bits = env->GetFieldID(java_cls, "blueBits", "I");
        jfieldID alpha_bits = env->GetFieldID(java_cls, "alphaBits", "I");
        jfieldID depth_bits = env->GetFieldID(java_cls, "depthBits", "I");
        jfieldID stencil_bits = env->GetFieldID(java_cls, "stencilBits", "I");

        env->SetIntField(dst_config, red_bits, src_config.red_bits);
        env->SetIntField(dst_config, green_bits, src_config.green_bits);
        env->SetIntField(dst_config, blue_bits, src_config.blue_bits);
        env->SetIntField(dst_config, alpha_bits, src_config.alpha_bits);
        env->SetIntField(dst_config, depth_bits, src_config.depth_bits);
        env->SetIntField(dst_config, stencil_bits, src_config.stencil_bits);
      });
}

GLContextConfig ancer::BridgeGLContextConfiguration(jobject src_config) {
  GLContextConfig dst_config;
  jni::SafeJNICall(
      [src_config, &dst_config](jni::LocalJNIEnv *env) {
        jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());

        jclass java_cls = LoadClass(
            env, activity,
            "com/google/gamesdk/gamecert/operationrunner/hosts/"
            "BaseGLHostActivity$GLContextConfiguration");

        jfieldID red_bits = env->GetFieldID(java_cls, "redBits", "I");
        jfieldID green_bits = env->GetFieldID(java_cls, "greenBits", "I");
        jfieldID blue_bits = env->GetFieldID(java_cls, "blueBits", "I");
        jfieldID alpha_bits = env->GetFieldID(java_cls, "alphaBits", "I");
        jfieldID depth_bits = env->GetFieldID(java_cls, "depthBits", "I");
        jfieldID stencil_bits = env->GetFieldID(java_cls, "stencilBits", "I");

        dst_config.red_bits = env->GetIntField(src_config, red_bits);
        dst_config.green_bits = env->GetIntField(src_config, green_bits);
        dst_config.blue_bits = env->GetIntField(src_config, blue_bits);
        dst_config.alpha_bits = env->GetIntField(src_config, alpha_bits);
        dst_config.depth_bits = env->GetIntField(src_config, depth_bits);
        dst_config.stencil_bits = env->GetIntField(src_config, stencil_bits);
      });

  return dst_config;
}

int32_t ancer::GetAppVsyncOffsetNanos() {
  int32_t offset = 0;

  jobject activity = jni::GetActivityWeakGlobalRef();
  if (activity == NULL) {
    return 0; // throw
  }

  jni::SafeJNICall([activity, &offset](jni::LocalJNIEnv *env) {
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jclass window_manager_class = env->FindClass("android/view/WindowManager");
    jclass display_class = env->FindClass("android/view/Display");

    jmethodID get_window_manager = env->GetMethodID(
        activity_class, "getWindowManager", "()Landroid/view/WindowManager;");
    jmethodID get_default_display = env->GetMethodID(
        window_manager_class, "getDefaultDisplay", "()Landroid/view/Display;");
    jobject wm = env->CallObjectMethod(activity, get_window_manager);
    jobject display = env->CallObjectMethod(wm, get_default_display);

    jmethodID get_vsync_offset_nanos = env->GetMethodID(display_class, "getAppVsyncOffsetNanos", "()J");
    if (get_vsync_offset_nanos == 0) {
      return; // throw
    }

    const long app_vsync_offset = env->CallLongMethod(display, get_vsync_offset_nanos);
    offset = static_cast<int32_t>(app_vsync_offset);
  });

  return offset;
}

//--------------------------------------------------------------------------------------------------

std::vector<unsigned char> ancer::PNGEncodeRGBABytes(
    unsigned int width, unsigned int height,
    const std::vector<unsigned char> &bytes) {
  std::vector<unsigned char> data;

  jni::SafeJNICall(
      [&width, &height, &bytes, &data](jni::LocalJNIEnv *env) {
        // Bitmap config
        jobject argb8888_value =
            jni::GetEnumField(env, "android/graphics/Bitmap$Config", "ARGB_8888");
        if (argb8888_value == nullptr)
          return;

        // Bitmap
        jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
        if (bitmap_class == nullptr)
          return;

        jmethodID create_bitmap_method_id = env->GetStaticMethodID(
            bitmap_class, "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;Z)Landroid/graphics/Bitmap;");
        if (create_bitmap_method_id == nullptr)
          return;

        jobject bitmap_object =
            env->CallStaticObjectMethod(bitmap_class, create_bitmap_method_id,
                                        width, height, argb8888_value, true);
        if (bitmap_object == nullptr)
          return;

        // Set pixel data
        // (Note, we could try to use `setPixels` instead, but we would
        // still need to convert four bytes into a 32-bit jint.)
        jmethodID set_pixel_method_id = env->GetMethodID(bitmap_class, "setPixel", "(III)V");
        if (set_pixel_method_id == nullptr)
          return;

        size_t offset = 0;
        for (int y = 0; y < height; ++y) {
          for (int x = 0; x < width; ++x) {
            const unsigned char r = bytes[offset++];
            const unsigned char g = bytes[offset++];
            const unsigned char b = bytes[offset++];
            const unsigned char a = bytes[offset++];
            const uint32_t c = static_cast<uint32_t>(a) << 24U
                | static_cast<uint32_t>(r) << 16U
                | static_cast<uint32_t>(g) << 8U
                | static_cast<uint32_t>(b);
            const jint color = static_cast<jint>(c);
            env->CallVoidMethod(bitmap_object, set_pixel_method_id, x, y, color);
          }
        }

        // PNG compression format
        jobject png_format_value =
            jni::GetEnumField(env, "android/graphics/Bitmap$CompressFormat", "PNG");
        if (png_format_value == nullptr)
          return;

        // Make output stream
        jclass byte_array_os_class = env->FindClass("java/io/ByteArrayOutputStream");
        if (byte_array_os_class == nullptr)
          return;

        jmethodID byte_array_os_init_id = env->GetMethodID(byte_array_os_class, "<init>", "()V");
        if (byte_array_os_init_id == nullptr)
          return;

        jobject byte_array_os = env->NewObject(byte_array_os_class, byte_array_os_init_id);
        if (byte_array_os == nullptr)
          return;

        // Compress as PNG
        jmethodID compress_method_id =
            env->GetMethodID(bitmap_class,
                             "compress",
                             "(Landroid/graphics/Bitmap$CompressFormat;"
                             "ILjava/io/OutputStream;)Z");
        if (compress_method_id == nullptr)
          return;

        const jboolean compress_result =
            env->CallBooleanMethod(bitmap_object, compress_method_id,
                                   png_format_value, 100, byte_array_os);
        if (!compress_result)
          return;

        // Get byte array data
        jmethodID to_byte_array_method_id = env->GetMethodID(
            byte_array_os_class, "toByteArray", "()[B");
        if (to_byte_array_method_id == nullptr)
          return;

        auto byte_result =
            (jbyteArray) env->CallObjectMethod(byte_array_os, to_byte_array_method_id);
        if (byte_result == nullptr)
          return;

        const jsize byte_result_length = env->GetArrayLength(byte_result);
        data.resize(static_cast<size_t>(byte_result_length));
        env->GetByteArrayRegion(byte_result, 0, byte_result_length,
                                reinterpret_cast<jbyte *>(data.data()));
      });

  return data;
}

std::string ancer::Base64EncodeBytes(const unsigned char *bytes, int length) {
  std::string encoded;

  jni::SafeJNICall(
      [&bytes, &length, &encoded](jni::LocalJNIEnv *env) {
        jclass base64_class = env->FindClass("android/util/Base64");
        if (base64_class == nullptr)
          return;

        jmethodID encode_to_string_method_id = env->GetStaticMethodID(
            base64_class, "encodeToString", "([BIII)Ljava/lang/String;");
        if (encode_to_string_method_id == nullptr)
          return;

        jbyteArray bytes_array = env->NewByteArray(length);
        if (bytes_array == nullptr) {
          return;
        }

        env->SetByteArrayRegion(bytes_array, 0, length, reinterpret_cast<const jbyte *>(bytes));

        auto str = (jstring) env->CallStaticObjectMethod(base64_class, encode_to_string_method_id,
                                                         bytes_array, 0, length, 0);
        if (str != nullptr) {
          const char *cstr = env->GetStringUTFChars(str, nullptr);
          if (cstr != nullptr) {
            encoded = std::string(cstr);
            env->ReleaseStringUTFChars(str, cstr);
          }
        }
      });

  return encoded;
}
