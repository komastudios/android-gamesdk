//
// Created by theoking on 3/19/2020.
//

#ifndef BENDER_BASE_PACKED_TYPES_H
#define BENDER_BASE_PACKED_TYPES_H

#pragma pack(push, 0)

#include "../../../../../../../Users/theoking/AppData/Local/Android/Sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/cstdint"
#include "bender_helpers.h"
#include "../../external/glm/glm.hpp"
#include "../../external/glm/detail/type_quat.hpp"

using namespace benderhelpers;

struct vec4_snorm16 {

    int16_t x, y, z, w;

    const vec4_snorm16 &Set(glm::vec4 in) {
        x = NormFloatToSnorm16(in.x);
        y = NormFloatToSnorm16(in.y);
        z = NormFloatToSnorm16(in.z);
        w = NormFloatToSnorm16(in.w);
        return *this;
    }

    const vec4_snorm16 &Set(glm::quat in) {
            x = NormFloatToSnorm16(in.x);
            y = NormFloatToSnorm16(in.y);
            z = NormFloatToSnorm16(in.z);
            w = NormFloatToSnorm16(in.w);
            return *this;
    }

    const vec4_snorm16 &Set(glm::vec3 in, float in_w) {
            x = NormFloatToSnorm16(in.x);
            y = NormFloatToSnorm16(in.y);
            z = NormFloatToSnorm16(in.z);
            w = NormFloatToSnorm16(in_w);
            return *this;
    }
};

struct vec3_snorm16 {
    int16_t x, y, z;

    const vec3_snorm16 &Set(glm::vec3 in) {
            x = NormFloatToSnorm16(in.x);
            y = NormFloatToSnorm16(in.y);
            z = NormFloatToSnorm16(in.z);
            return *this;
    }
};

struct vec2_snorm16 {
    int16_t x, y;

    const vec2_snorm16 &Set(glm::vec2 in) {
            x = NormFloatToSnorm16(in.x);
            y = NormFloatToSnorm16(in.y);
            return *this;
    }
};

struct vec4_unorm16 {
    int16_t x, y, z, w;

    const vec4_unorm16 &Set(glm::vec4 in) {
            x = NormFloatToUnorm16(in.x);
            y = NormFloatToUnorm16(in.y);
            z = NormFloatToUnorm16(in.z);
            w = NormFloatToUnorm16(in.w);
            return *this;
    }
};

struct vec3_unorm16 {
    int16_t x, y, z;

    const vec3_unorm16 &Set(glm::vec3 in) {
            x = NormFloatToUnorm16(in.x);
            y = NormFloatToUnorm16(in.y);
            z = NormFloatToUnorm16(in.z);
            return *this;
    }
};

struct vec2_unorm16 {
    int16_t x, y;

    const vec2_unorm16 &Set(glm::vec2 in) {
            x = NormFloatToUnorm16(in.x);
            y = NormFloatToUnorm16(in.y);
            return *this;
    }
};

#pragma pack(pop)

struct packed_vertex {
    vec4_snorm16 position;
    vec4_snorm16 q_tangent;
    vec2_unorm16 tex_coord;
};

#endif //BENDER_BASE_PACKED_TYPES_H
