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

#include "System.hpp"

#include "util/Error.hpp"
#include "util/FpsCalculator.hpp"
#include "util/GLHelpers.hpp"
#include "util/JNIHelpers.hpp"
#include "util/Log.hpp"

using namespace ancer;

//==============================================================================

namespace {
Log::Tag TAG{"ancer::system"};
}

//==============================================================================

GLuint ancer::CreateProgram(const char *vtx_file_name, const char *frg_file_name) {
  std::string vtx_src = LoadText(vtx_file_name);
  std::string frag_src = LoadText(frg_file_name);
  if (!vtx_src.empty() && !frag_src.empty()) {
    return glh::CreateProgramSrc(vtx_src.c_str(), frag_src.c_str());
  }
  return 0;
}

//==============================================================================

GLuint ancer::LoadTexture(const char *file_name, int32_t *out_width, int32_t *out_height,
                          bool *has_alpha) {
  GLuint tex = 0;
  jni::SafeJNICall(
      [&tex, &out_width, &out_height, &has_alpha, file_name](jni::LocalJNIEnv *env) {
        jstring name = env->NewStringUTF(file_name);
        jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());
        jclass activity_class = env->GetObjectClass(activity);

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        jmethodID get_asset_helpers_mid = env->GetMethodID(
            activity_class,
            "getSystemHelpers",
            "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;"
        );
        jobject asset_helpers_instance = env->CallObjectMethod(activity, get_asset_helpers_mid);
        jclass asset_helpers_class = env->GetObjectClass(asset_helpers_instance);

        jmethodID load_texture_mid = env->GetMethodID(
            asset_helpers_class, "loadTexture",
            "(Ljava/lang/String;)Lcom/google/gamesdk/gamecert/operationrunner/"
            "util/SystemHelpers$TextureInformation;");

        jobject out = env->CallObjectMethod(asset_helpers_instance, load_texture_mid, name);

        // extract TextureInformation from out
        jclass java_cls = LoadClass(
            env, activity,
            "com/google/gamesdk/gamecert/operationrunner/util/"
            "SystemHelpers$TextureInformation");
        jfieldID fid_ret = env->GetFieldID(java_cls, "ret", "Z");
        jfieldID fid_has_alpha = env->GetFieldID(java_cls, "alphaChannel", "Z");
        jfieldID fid_width = env->GetFieldID(java_cls, "originalWidth", "I");
        jfieldID fid_height = env->GetFieldID(java_cls, "originalHeight", "I");
        bool ret = env->GetBooleanField(out, fid_ret);
        bool alpha = env->GetBooleanField(out, fid_has_alpha);
        int32_t width = env->GetIntField(out, fid_width);
        int32_t height = env->GetIntField(out, fid_height);

        if (!ret) {
          glDeleteTextures(1, &tex);
          *out_width = 0;
          *out_height = 0;
          if (has_alpha)
            *has_alpha = false;
          FatalError(TAG, "loadTexture - unable to load \"%s\"",
                     file_name);
        } else {
          *out_width = width;
          *out_height = height;
          if (has_alpha)
            *has_alpha = alpha;

          // Generate mipmap
          glGenerateMipmap(GL_TEXTURE_2D);
        }
      });

  return tex;
}

//==============================================================================

FpsCalculator &ancer::GetFpsCalculator() {
  static FpsCalculator fps_calc;
  return fps_calc;
}

//==============================================================================

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

//==============================================================================

std::vector<unsigned char> ancer::PNGEncodeRGBABytes(
    unsigned int width, unsigned int height,
    const std::vector<unsigned char> &bytes) {
  std::vector<unsigned char> data;

  jni::SafeJNICall(
      [&width, &height, &bytes, &data](jni::LocalJNIEnv *env) {
        // Bitmap config
        jobject argb8888_value =
            jni::GetEnumField(env, "android/graphics/Bitmap$Config", "ARGB_8888");
        if (argb8888_value==nullptr)
          return;

        // Bitmap
        jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
        if (bitmap_class==nullptr)
          return;

        jmethodID create_bitmap_method_id = env->GetStaticMethodID(
            bitmap_class, "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;Z)Landroid/graphics/Bitmap;");
        if (create_bitmap_method_id==nullptr)
          return;

        jobject bitmap_object =
            env->CallStaticObjectMethod(bitmap_class, create_bitmap_method_id,
                                        width, height, argb8888_value, true);
        if (bitmap_object==nullptr)
          return;

        // Set pixel data
        // (Note, we could try to use `setPixels` instead, but we would
        // still need to convert four bytes into a 32-bit jint.)
        jmethodID set_pixel_method_id = env->GetMethodID(bitmap_class, "setPixel", "(III)V");
        if (set_pixel_method_id==nullptr)
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
        if (png_format_value==nullptr)
          return;

        // Make output stream
        jclass byte_array_os_class = env->FindClass("java/io/ByteArrayOutputStream");
        if (byte_array_os_class==nullptr)
          return;

        jmethodID byte_array_os_init_id = env->GetMethodID(byte_array_os_class, "<init>", "()V");
        if (byte_array_os_init_id==nullptr)
          return;

        jobject byte_array_os = env->NewObject(byte_array_os_class, byte_array_os_init_id);
        if (byte_array_os==nullptr)
          return;

        // Compress as PNG
        jmethodID compress_method_id =
            env->GetMethodID(bitmap_class,
                             "compress",
                             "(Landroid/graphics/Bitmap$CompressFormat;"
                             "ILjava/io/OutputStream;)Z");
        if (compress_method_id==nullptr)
          return;

        const jboolean compress_result =
            env->CallBooleanMethod(bitmap_object, compress_method_id,
                                   png_format_value, 100, byte_array_os);
        if (!compress_result)
          return;

        // Get byte array data
        jmethodID to_byte_array_method_id = env->GetMethodID(
            byte_array_os_class, "toByteArray", "()[B");
        if (to_byte_array_method_id==nullptr)
          return;

        auto byte_result =
            (jbyteArray) env->CallObjectMethod(byte_array_os, to_byte_array_method_id);
        if (byte_result==nullptr)
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
        if (base64_class==nullptr)
          return;

        jmethodID encode_to_string_method_id = env->GetStaticMethodID(
            base64_class, "encodeToString", "([BIII)Ljava/lang/String;");
        if (encode_to_string_method_id==nullptr)
          return;

        jbyteArray bytes_array = env->NewByteArray(length);
        if (bytes_array==nullptr) {
          return;
        }

        env->SetByteArrayRegion(bytes_array, 0, length, reinterpret_cast<const jbyte *>(bytes));

        auto str = (jstring) env->CallStaticObjectMethod(base64_class, encode_to_string_method_id,
                                                         bytes_array, 0, length, 0);
        if (str!=nullptr) {
          const char *cstr = env->GetStringUTFChars(str, nullptr);
          if (cstr!=nullptr) {
            encoded = std::string(cstr);
            env->ReleaseStringUTFChars(str, cstr);
          }
        }
      });

  return encoded;
}
