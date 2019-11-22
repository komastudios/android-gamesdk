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

#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <ios>
#include <unordered_map>
#include <sstream>
#include <regex>
#include <iterator>
#include <cstdio>

#include <condition_variable>
#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;

// Purpose: Measures compilation time of 1...2^N shaders compared to the
// equivalent time to load the program binaries from a blob stored to disk.

//==============================================================================

namespace {
    constexpr Log::Tag TAG{"ShaderBlobGLES3Stressor"};
}

//==============================================================================

namespace {
    struct configuration {
        bool debug_only = false;
        int trials = 1;
        int iterations = 1;

        void Validate() const {
            if (iterations < 1) {
                FatalError(TAG, "Configured number of 'iterations' "
                                "must be 1 or greater.");
            }

            if (iterations > 12) {
                FatalError(TAG, "Configured number of 'iterations' would generate "
                                "more shader variants than the test "
                                "supports (max 4096).");
            }

            if (trials < 1) {
                FatalError(TAG, "Configured number of 'trials' must be 1 or greater.");
            }
        }
    };

    JSON_CONVERTER(configuration) {
        JSON_OPTVAR(debug_only);
        JSON_OPTVAR(trials);
        JSON_OPTVAR(iterations);
    }

//------------------------------------------------------------------------------

    struct datum {
        int trial = 0;
        int shader_program_count = 0;
        float compile_time_milliseconds = 0.0F;
        float cache_load_time_milliseconds = 0.0F; };

    JSON_WRITER(datum) {
        JSON_REQVAR(trial);
        JSON_REQVAR(shader_program_count);
        JSON_REQVAR(compile_time_milliseconds);
        JSON_REQVAR(cache_load_time_milliseconds);
    }
}

//==============================================================================

namespace {

    // Returns the filename component of a file path.
    std::string GetFilename(const std::string& path) {
        const size_t pos = path.find_last_of("/");
        if (pos > path.size()) {
            return path;
        }

        return path.substr(pos + 1, path.size() - pos - 1);
    }

    // Returns the given path excluding the file extension and period.
    std::string GetPathRemovingExtension(const std::string& path) {
        const size_t pos = path.find_last_of(".");
        if (pos > path.size()) {
            return path;
        }

        return path.substr(0, pos);
    }

    // Returns the file extension from a file path.
    std::string GetFileExtension(const std::string& path) {
        const size_t pos = path.find_last_of(".");
        if (pos > path.size()) {
            return path;
        }

        return path.substr(pos + 1, path.size() - pos - 1);
    }

    // TODO (baxtermichael@google.com): Handle implication of regex input
    std::string StringReplace(
            const std::string& src,
            const std::string& original,
            const std::string& replacement)
    {
        std::string result;
        std::regex exp{"\\b" + original + "\\b"};
        std::regex_replace(std::back_inserter(result),
                src.begin(), src.end(), exp, replacement);
        return result;
    }

    std::string LoadTextFromFiles(const std::string& path) {
        std::ifstream ifs{path};
        if (!ifs.good()) {
            FatalError(TAG, "Failed to open file : " + path);
        }

        std::stringstream ss;
        ss << ifs.rdbuf();
        ifs.close();

        return ss.str();
    }

    bool SaveTextToFiles(const std::string& path, const std::string text) {
        std::ofstream ofs{path};
        if (!ofs.good()) {
            return false;
        }

        ofs << text;
        ofs.close();

        return true;
    }

    std::vector<std::string> GenerateShaderVariants(
            const std::string& src_path,
            const std::string& dst_dir,
            size_t num_variants,
            const std::string& identifier) {

        std::vector<std::string> paths;

        const std::string shader_src = LoadText(src_path.c_str());

        for (size_t i = 1; i < num_variants + 1; ++i) {

            std::string dst_path = dst_dir;
            dst_path += "/" + GetPathRemovingExtension(GetFilename(src_path));
            dst_path += std::to_string(i);
            dst_path += "." + GetFileExtension(src_path);

            const std::string modified = StringReplace(shader_src, identifier,
                    identifier + std::to_string(i));

            if (!SaveTextToFiles(dst_path, modified)) {
                FatalError(TAG, "Failed to create file to store"
                                "shader variant : " + dst_path);
            }

            paths.push_back(dst_path);
        }

        return paths;
    }
}

//==============================================================================

namespace {

    class TestIteration {
    public:

        TestIteration(const std::string& vert_path,
                const std::vector<std::string>& frag_paths) {

            const auto t1 = SteadyClock::now();
            CompilePrograms(vert_path, frag_paths);
            _compile_duration = SteadyClock::now() - t1;

            CachePrograms();

            const auto t2 = SteadyClock::now();
            LoadCachedPrograms();
            _cache_load_duration = SteadyClock::now() - t2;
        }

        // Prevent copies/moves that would prematurely
        // destroy the underlying program object
        TestIteration(const TestIteration&) = delete;
        TestIteration& operator=(const TestIteration&) = delete;
        TestIteration(const TestIteration&&) = delete;
        TestIteration& operator=(const TestIteration&&) = delete;

        ~TestIteration() {
            for (auto& program : _compiled_programs) {
                glDeleteProgram(program);
                program = 0;
            }

            for (auto& cached : _cached_programs) {
                glDeleteProgram(cached.name);
                cached.name = 0;
                cached.length = 0;
            }

            std::remove(_blob_path.c_str());
        }

        datum GetDatum() const {

            const auto compile_time = duration_cast<MillisecondsAs<float>>(
                    _compile_duration).count();

            const auto cache_load_time = duration_cast<MillisecondsAs<float>>(
                    _cache_load_duration).count();

            return datum{
                0,
                static_cast<int>(_compiled_programs.size()),
                compile_time,
                cache_load_time
            };
        }

        // GetCompiledPrograms can be called during development/debugging
        // to test or inspect any of the shader programs generated.
        std::vector<GLuint> GetCompiledPrograms() const {
            return _compiled_programs;
        }

        // GetCachedPrograms can be called during development/debugging
        // to test or inspect any of the shader programs generated.
        std::vector<GLuint> GetCachedPrograms() const {

            std::vector<GLuint> programs;

            for (const auto& p : _cached_programs) {
                programs.push_back(p.name);
            }

            return programs;
        }

    private:

        struct CachedProgram {
            GLuint name = 0;
            GLenum format;
            GLsizei length = 0;
        };

        void CompilePrograms(
                const std::string& vert_path,
                const std::vector<std::string>& frag_paths) {

            const std::string vert_source = LoadText(vert_path.c_str());

            for (const auto& frag_path : frag_paths) {
                const std::string frag_source = LoadTextFromFiles(frag_path.c_str());

                GLuint program = glh::CreateProgramSrc(
                        vert_source.c_str(),
                        frag_source.c_str());

                _compiled_programs.push_back(program);
            }
        }

        void CachePrograms() {

            const size_t buf_size = 32 * 1024 * 1024;
            void *buf = malloc(buf_size);
            size_t buf_offset = 0;

            for (auto program : _compiled_programs) {

                const auto cached = CacheProgram(program,
                        (char*)buf + buf_offset,
                        buf_size - buf_offset);

                buf_offset += static_cast<size_t>(cached.length);

                _cached_programs.push_back(cached);
            }

            _blob_size = buf_offset;
            SaveBlob(_blob_path, buf, _blob_size);

            free(buf);
        }

        CachedProgram CacheProgram(GLuint program, void *buf, size_t buf_size) {

            GLsizei length;
            GLenum format;

            glGetProgramBinary(program, static_cast<GLsizei>(buf_size),
                    &length, &format, buf);

            const GLenum err = glGetError();
            if (err) {
                FatalError(TAG, "Failed to create shader program binary");
            }

            CachedProgram cached_program{};
            cached_program.length = length;
            cached_program.format = format;

            return cached_program;
        }

        void SaveBlob(const std::string& path, void *data, size_t bytes) {

            std::ofstream f{ _blob_path, std::ios::binary };
            if (!f.good()) {
                FatalError(TAG, "Failed to create shader blob file");
            }

            f.write(reinterpret_cast<char*>(data), bytes);
        }

        void LoadCachedPrograms() {

            void *buf = malloc(_blob_size);
            size_t buf_offset = 0;

            LoadBlob(_blob_path, buf, _blob_size);

            for (auto& cached_program : _cached_programs) {
                LoadCachedProgram(cached_program, (char*)buf + buf_offset);
                buf_offset += static_cast<size_t>(cached_program.length);
            }

            free(buf);
        }

        void LoadCachedProgram(CachedProgram& cached_program, void *buf) {

            GLuint program = glCreateProgram();
            if (!program) {
                FatalError(TAG, "Failed to create shader program");
            }

            glProgramBinary(program, cached_program.format, buf,
                    cached_program.length);

            if (glGetError()) {
                FatalError(TAG, "Failed to create shader program from binary");
            }

            cached_program.name = program;
        }

        void LoadBlob(const std::string& path, void *buf, size_t bytes) {

            std::ifstream f{path, std::ios::binary};
            if (!f.good()) {
                FatalError(TAG, "Failed to open shader blob file");
            }

            f.read(reinterpret_cast<char*>(buf), bytes);
        }

    private:

        Duration _compile_duration;
        Duration _cache_load_duration;

        std::vector<GLuint> _compiled_programs;

        const std::string _blob_path = GetFilesDirectory() + "/shader_blob.bin";
        size_t _blob_size = 0;
        std::vector<CachedProgram> _cached_programs;
    };
}

// =============================================================================

class ShaderBlobGLES3Operation : public BaseGLES3Operation {
public:

    ShaderBlobGLES3Operation() = default;

    void OnGlContextReady(const GLContextConfig& ctx_config) override {
        Log::D(TAG, "GlContextReady");
        _configuration = GetConfiguration<configuration>();
        _configuration.Validate();

        SetHeartbeatPeriod(500ms);

        _egl_context = eglGetCurrentContext();
        if (_egl_context == EGL_NO_CONTEXT) {
            FatalError(TAG, "No EGL context available");
        }

        if (_configuration.debug_only) {
            SetupDrawableShader();
        } else {
            RunTest();
        }
    }

    void Draw(double delta_seconds) override {
        BaseGLES3Operation::Draw(delta_seconds);
        if (_configuration.debug_only) {
            DrawShader(delta_seconds);
        }
    }

    void OnHeartbeat(Duration elapsed) override {
    }

private:

    void RunTest() {

        const int iterations = _configuration.iterations;
        const int trials = _configuration.trials;

        // Generate shader variants:
        // We never want to use the same shader source twice.
        // Since our largest iteration runs 2^(iterations) shaders,
        // we will need twice that many shaders (e.g., as with mipmaps).
        const int shader_count = 2 * trials * static_cast<int>(pow(2, iterations));

        const auto frag_paths = GenerateShaderVariants(
                _frag_path,
                GetFilesDirectory(),
                shader_count, "uTime");

        // Run test iterations, 1, 2, 4 ... N
        int frag_paths_offset = 0;
        for (int trial = 0; trial < trials; ++trial) {

            for (int i = 0; i <= iterations; ++i) {

                std::vector<std::string> paths{
                        frag_paths.begin() + frag_paths_offset,
                        frag_paths.begin() + frag_paths_offset
                            + static_cast<int>(pow(2, i))
                };
                frag_paths_offset += static_cast<int>(paths.size());

                TestIteration test(_vert_path, paths);

                auto datum = test.GetDatum();
                datum.trial = trial;
                Report(datum);
            }
        }

        // Clean up shader variants
        for (const auto& frag_path : frag_paths) {
            std::remove(frag_path.c_str());
        }
    }

    //--------------------------------------------------------------------------
    // The performance test only involves compiling shaders, not drawing them.
    // Nevertheless, we want to make it easy to visually verify and debug
    // shaders used in the test.

    void SetupDrawableShader() {
        // Since our test generates a bunch of variants of the same fragment
        // shader and stores them in the user "files" directory, we only look
        // for fragment shaders in that directory, even if the vertex shader
        // is elsewhere. Thus, even when creating a shader for debug drawing,
        // we need to place it in the same files directory.

        const std::string src = LoadText(_frag_path.c_str());

        const std::string dst_path = GetFilesDirectory()
                + "/" + GetFilename(_frag_path);
        if (!SaveTextToFiles(dst_path, src)) {
            FatalError(TAG, "Failed to save drawable shader to files");
        }

        const std::vector<std::string> frag_paths = { dst_path };
        _test = std::make_unique<TestIteration>(_vert_path, frag_paths);

        const auto test_programs = _test->GetCachedPrograms();
        if (test_programs.empty()) {
            FatalError(TAG, "Test did not generate any valid shader programs");
        }

        _prog = test_programs[0];
        _screen_resolution_uniform_loc = glGetUniformLocation(_prog, "uResolution");
        _time_uniform_loc = glGetUniformLocation(_prog, "uTime");
    }

    void DrawShader(double delta_seconds) {
        // Color and depth clear are handled by GLSurfaceViewHostActivity

        const auto context_size = GetGlContextSize();
        _elapsed_time += delta_seconds;

        glUseProgram(_prog);

        glUniform2f(_screen_resolution_uniform_loc, context_size.x, context_size.y);
        glUniform1f(_time_uniform_loc, static_cast<float>(_elapsed_time));

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

private:

    configuration _configuration;
    EGLContext _egl_context = EGL_NO_CONTEXT;
    double _elapsed_time = 0.0;

    const std::string _vert_path = "Shaders/ShaderBlobGLES3Operation/"
                                   "implicit_fullscreen_triangle.vsh";
    const std::string _frag_path = "Shaders/ShaderBlobGLES3Operation/"
                                   "iq_quadratics.fsh";

    std::unique_ptr<TestIteration> _test;
    GLuint _prog = 0;
    GLint _screen_resolution_uniform_loc = 0;
    GLint _time_uniform_loc = 0;
};

EXPORT_ANCER_OPERATION(ShaderBlobGLES3Operation)