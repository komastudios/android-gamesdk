/*
 * Copyright 2019 The Android Open Source Project
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

#pragma once

#include <cstddef>
#include <string>

#include <GLES3/gl32.h>
#include <jni.h>
#include <sstream>

namespace ancer {
    class FpsCalculator;
}


namespace ancer {
    namespace internal {
        void SetJavaVM(JavaVM*);

        // init/deinit calls for the framework.
        void BindJNI(jobject activity);
        void UnbindJNI();
    }

    void SetFilesDirectory(std::string path);

    std::string GetFilesDirectory();

    /*
     * Load the text from a file in the application's assets/ folder
     */
    std::string LoadText(const char* file_name);

    /*
     * Create gl shader program given the contents of the two shader files in assets/
     * returns handle to program, or 0 if program failed to load or link
     */
    GLuint CreateProgram(const char* vtx_file_name, const char* frg_file_name);

    /*
     * Loads a texture from application's assets/ folder. If texture loads,
     * writes width into out_width, height into out_height, and if the texture
     * has an alpha channel writes true into has_alpha. Returns the GL
     * texture id.
     */
    GLuint LoadTexture(
            const char* file_name, int32_t* out_width = nullptr,
            int32_t* out_height = nullptr, bool* has_alpha = nullptr);

    struct MemoryInfo {
    public:
        int _oom_score = 0;                  // Current out-of-memory score (higher is worse).
        long native_heap_allocation_size = 0; // Current heap allocation size in bytes.
        long commit_limit = 0;              // Current commit limit in bytes.
        long total_memory;        // Total memory in bytes "accessible to kernel"
        long available_memory;    // Available memory in-system.
        long low_memory_threshold; // Point where the system would enter a low-memory condition.
        bool low_memory;          // True if we're currently in a low-memory condition.
    };

    /*
     * Get current memory information about the system. This is sourced
     * from the host activity, via BaseHostActivity.getMemoryHelper().getMemoryInfo()
     */
    MemoryInfo GetMemoryInfo();

    /*
     * ThermalStatus - direct mapping of the thermal constants defined here:
     * https://developer.android.com/reference/android/os/PowerManager.html#THERMAL_STATUS_NONE
     */
    enum class ThermalStatus {
        None = 0,
        Light = 1,
        Moderate = 2,
        Severe = 3,
        Critical = 4,
        Emergency = 5,
        Shutdown = 6,
        Unknown = -1 // added for unsupported API/devices
    };

    inline std::string to_string(ThermalStatus status) {
        switch ( status ) {
        case ThermalStatus::None: return "none";
        case ThermalStatus::Light: return "light";
        case ThermalStatus::Moderate: return "moderate";
        case ThermalStatus::Severe: return "severe";
        case ThermalStatus::Critical: return "critical";
        case ThermalStatus::Emergency: return "emergency";
        case ThermalStatus::Shutdown: return "shutdown";
        case ThermalStatus::Unknown: return "unknown";
        }
    }

    inline std::ostream& operator<<(std::ostream& os, ThermalStatus status) {
        os << to_string(status);
        return os;
    }

    /*
     * Get the current thermal status as reported by the android powermanager.
     * Note, this only returns valid data on android Q and up (and on pixel devices at present?)
     * Returns -1 for pre-q devices;
     * TODO(shamyl@google.com): Confirm this return value for non-pixel Q devices?
     */
    ThermalStatus GetThermalStatus();

    /*
     * Run the java vm garbage collector
     */
    void RunSystemGc();

    /*
     * Set cpu affinity for the calling thread
     */
    void SetThreadAffinity(int cpuIndex);

    static jclass RetrieveClass(JNIEnv* env, jobject activity, const char* className);

    FpsCalculator& GetFpsCalculator();

    /*
    * Configuration params for opengl contexts
    */
    struct GLContextConfig {
      int red_bits = 8;
      int green_bits = 8;
      int blue_bits = 8;
      int alpha_bits = 0;
      int depth_bits = 24;
      int stencil_bits = 0;

      static GLContextConfig Default() {
          return GLContextConfig {
              8,8,8,8,
              24,0
          };
      }
    };

    inline bool operator==(const GLContextConfig &lhs, const GLContextConfig &rhs) {
      return lhs.red_bits == rhs.red_bits &&
        lhs.green_bits == rhs.green_bits &&
        lhs.blue_bits == rhs.blue_bits &&
        lhs.alpha_bits == rhs.alpha_bits &&
        lhs.depth_bits == rhs.depth_bits &&
        lhs.stencil_bits == rhs.stencil_bits;
    }

    inline std::ostream& operator<<(std::ostream& os, GLContextConfig c) {
        os << "<GLContextConfig r" << c.red_bits << "g" << c.green_bits << "b" << c.blue_bits << "a" << c.alpha_bits << "d" << c.depth_bits << "s" << c.stencil_bits << ">";
        return os;
    }

    inline std::string to_string(const GLContextConfig &c) {
        std::stringstream ss;
        ss << c;
        return ss.str();
    }

    /*
     * Write the fields from config into the java-hosted destination
     */
    void BridgeGLContextConfiguration(GLContextConfig src_config,
                                      jobject dst_config);

    /*
     * Create native GLContextConfig from java-based GLContextConfiguration
     * instance
    */
    GLContextConfig BridgeGLContextConfiguration(jobject src_config);


} // namespace ancer