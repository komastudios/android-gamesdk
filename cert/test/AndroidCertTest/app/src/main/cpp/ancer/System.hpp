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

    /**
     * Load the text from a file in the application's assets/ folder
     * @param file_name the name of the file to load relative to assets/ folder
     * @return the text, or an empty string
     */
    std::string LoadText(const char* file_name);

    /**
     * Create gl shader program given the contents of the two shader files in assets/
     * @param vtx_file_name file name of vertex shader in assets
     * @param frg_file_name file name of fragment shader in assets
     * @return handle to program, or 0 if program failed to load or link
     */
    GLuint CreateProgram(const char* vtx_file_name, const char* frg_file_name);

    /**
     * Loads a texture from application's assets/ folder.
     * @param file_name the name of the texture to load from assets/
     * @param out_width the width of the loaded texture (or 0 if failed to load)
     * @param out_height the height of the loaded (or 0 if failed to load)
     * @param has_alpha if true the loaded texture has an alpha channel
     * @return the GL texture id. Check out_width && out_height > 0 to confirm load was successful.
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
    /**
     * Get current memory information about the system. This is sourced
     * from the host activity, via BaseHostActivity.getMemoryHelper().getMemoryInfo()
     * @return MemoryInfo, filled out
     */
    MemoryInfo GetMemoryInfo();

    /**
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

    /**
     * Get the current thermal status as reported by the android powermanager.
     * Note, this only returns valid data on android Q and up (and on pixel devices at present?)
     * TODO: What's the return value for non-pixel Q devices?
     * will return -1 for pre-q devices;
     * @return
     */
    ThermalStatus GetThermalStatus();

    /**
     * Run the java vm garbage collector
     */
    void RunSystemGc();

    /**
     * Set cpu affinity for the calling thread
     * @param cpuIndex the index of the cpu to set the calling thread's affinity to
     */
    void SetThreadAffinity(int cpuIndex);

    static jclass RetrieveClass(JNIEnv* env, jobject activity, const char* className);

    FpsCalculator& GetFpsCalculator();
}