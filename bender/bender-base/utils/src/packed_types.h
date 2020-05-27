//
// Created by theoking on 3/19/2020.
//

#ifndef BENDER_BASE_PACKED_TYPES_H
#define BENDER_BASE_PACKED_TYPES_H

#include <cstdint>
#include "bender_helpers.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/packing.hpp>

using namespace benderhelpers;
#pragma pack(push, 0)

struct vec4_snorm16 {

    int16_t x, y, z, w;

    vec4_snorm16() : x(0), y(0), z(0), w(0) { }

    vec4_snorm16(uint64_t packed_in) {
        *reinterpret_cast<uint64_t*>(&x) = packed_in;
    }

    const vec4_snorm16 &Set(glm::vec4 in) {
        uint64_t packed = glm::packSnorm4x16(in);
        x = packed & 0xFFFF;
        y = packed >> 16 & 0xFFFF;
        z = packed >> 32 & 0xFFFF;
        w = packed >> 48 & 0xFFFF;
        return *this;
    }

    const vec4_snorm16 &Set(glm::quat in) {
        uint64_t packed = glm::packSnorm4x16({in.x, in.y, in.z, in.w});
        x = packed & 0xFFFF;
        y = packed >> 16 & 0xFFFF;
        z = packed >> 32 & 0xFFFF;
        w = packed >> 48 & 0xFFFF;
        return *this;
    }

    const vec4_snorm16 &Set(glm::vec3 in, float in_w) {
        uint64_t packed = glm::packSnorm4x16({in.x, in.y, in.z, in_w});
        x = packed & 0xFFFF;
        y = packed >> 16 & 0xFFFF;
        z = packed >> 32 & 0xFFFF;
        w = packed >> 48 & 0xFFFF;
        return *this;
    }

    const vec4_snorm16 &Set(uint64_t packed_in) {
        //NOTE: When targeting a new platform, must verify that this is still valid.
        *reinterpret_cast<uint64_t*>(&x) = packed_in;
        return *this;
    }
};

struct vec2_snorm16 {
    int16_t x, y;

    vec2_snorm16() : x(0), y(0) { }

    vec2_snorm16(uint32_t packed_in) {
        *reinterpret_cast<uint32_t*>(&x) = packed_in;
    }

    const vec2_snorm16 &Set(glm::vec2 in) {
        uint packed = glm::packSnorm2x16(in);
        x = packed & 0xFFFF;
        y = packed >> 16 & 0xFFFF;
        return *this;
    }

    const vec2_snorm16 &Set(uint32_t packed_in) {
        //NOTE: When targeting a new platform, must verify that this is still valid.
        *reinterpret_cast<uint32_t*>(&x) = packed_in;
        return *this;
    }
};

struct vec4_unorm16 {
    int16_t x, y, z, w;

    vec4_unorm16() : x(0), y(0), z(0), w(0) { }

    vec4_unorm16(uint64_t packed_in) {
        *reinterpret_cast<uint64_t*>(&x) = packed_in;
    }

    const vec4_unorm16 &Set(glm::vec4 in) {
        uint64_t packed = glm::packUnorm4x16(in);
        x = packed & 0xFFFF;
        y = packed >> 16 & 0xFFFF;
        z = packed >> 32 & 0xFFFF;
        w = packed >> 48 & 0xFFFF;
        return *this;
    }

    const vec4_unorm16 &Set(uint64_t packed_in) {
        //NOTE: When targeting a new platform, must verify that this is still valid.
        *reinterpret_cast<uint64_t*>(&x) = packed_in;
        return *this;
    }
};

struct vec2_unorm16 {
    int16_t x, y;

    vec2_unorm16() : x(0), y(0) { }

    vec2_unorm16(uint32_t packed_in) {
        *reinterpret_cast<uint32_t *>(&x) = packed_in;
    }

    const vec2_unorm16 &Set(glm::vec2 in) {
        uint packed = glm::packUnorm2x16(in);
        x = packed & 0xFFFF;
        y = packed >> 16 & 0xFFFF;
        return *this;
    }

    const vec2_unorm16 &Set(uint32_t packed_in) {
        //NOTE: When targeting a new platform, must verify that this is still valid.
        *reinterpret_cast<uint32_t*>(&x) = packed_in;
        return *this;
    }
};


struct packed_vertex {
    vec4_snorm16 position;
    vec4_snorm16 q_tangent;
    vec2_unorm16 tex_coord;
};
#pragma pack(pop)

#endif //BENDER_BASE_PACKED_TYPES_H
