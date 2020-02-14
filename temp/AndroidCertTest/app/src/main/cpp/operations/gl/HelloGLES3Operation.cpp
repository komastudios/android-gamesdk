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
#include <thread>
#include <mutex>
#include <condition_variable>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"HelloGLES3Stressor"};
    constexpr float DefaultSaturation = 0.6F;
    constexpr float DefaultValue = 0.6F;
}

//==================================================================================================

namespace {
    struct configuration {
        float saturation = DefaultSaturation;
        float value = DefaultValue;
    };

    JSON_CONVERTER(configuration) {
        JSON_OPTVAR(saturation);
        JSON_OPTVAR(value);
    }

//--------------------------------------------------------------------------------------------------

    struct datum {
        int ticks;
    };

    JSON_WRITER(datum) {
        JSON_REQVAR(ticks);
    }
}

//==================================================================================================

class HelloGLES3Operation : public BaseGLES3Operation {
public:

    HelloGLES3Operation() = default;

    void OnGlContextReady(const GLContextConfig &ctx_config) override {
        Log::D(TAG, "GlContextReady");
        _configuration = GetConfiguration<configuration>();
        _tick = 0;

        SetHeartbeatPeriod(500ms);
    }

    void Draw(double delta_seconds) override {
        BaseGLES3Operation::Draw(delta_seconds);

        _tick++;

        float a = _tick * 1.0F;
        while ( a > 360 ) a -= 360;
        auto c = glh::color::hsv2rgb(
                glh::color::hsv{a, _configuration.saturation, _configuration.value});

        glClearColor(c.r, c.g, c.b, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OnHeartbeat(Duration elapsed) override {
        Report(datum{_tick});
    }

private:

    int _tick = 0;
    configuration _configuration;
};

EXPORT_ANCER_OPERATION(HelloGLES3Operation)