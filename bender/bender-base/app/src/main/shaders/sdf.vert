#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"
#include "push_constants.h"

layout(location = 0) out vec2 frag_tex_coord;
layout(location = 1) out vec4 frag_color;

layout(push_constant)
uniform TextData {
    FontPushConstants text;
} text_push_consts;

layout(set = FONT_BINDING_SET, binding = FONT_VERTEX_MATRIX_BINDING)
uniform Orientation {
    mat4 transform;
} orientation;

layout(set = FONT_BINDING_SET, binding = FONT_VERTEX_UBO_BINDING)
uniform FontData {
    /*
        position[0] : (pos_x, pos_y, width, height)
        position[1] : (x_offset, y_offset, x_advance, 0,0f)
        pos_x, pos_y : top-left coordinates of the character in texture space
        width, height : dimensions of the character in texture space
        x_offset, y_offset : for determining the vertex positions
    */
    vec4 positions[128][2];
} font_data;

vec2 GetTexCoord(uint current_char, uint index_mod) {
    vec4 texture_data = font_data.positions[current_char][0];

    if(index_mod == 0 || index_mod == 5) {
        return vec2(texture_data[0] + texture_data[2], texture_data[1] + texture_data[3]);
    } else if (index_mod == 2 || index_mod == 3) {
        return vec2(texture_data[0], texture_data[1]);
    } else if (index_mod == 1) {
        return vec2(texture_data[0], texture_data[1] + texture_data[3]);
    } else if (index_mod == 4) {
        return vec2(texture_data[0] + texture_data[2], texture_data[1]);
    }

    // default, should not have happened
    return vec2(0.0f, 0.0f);
}

void main() {
    uint current_char_index = gl_VertexIndex / 6;
    uint index_mod = gl_VertexIndex % 6;
    uint current_char = text_push_consts.text.text_data[current_char_index / 4] >> (8 * (3 - (current_char_index % 4))) & 0xFF;
    vec2 position = vec2(text_push_consts.text.coordinates[2], text_push_consts.text.coordinates[3]);

    for(int i = 0; i < current_char_index; ++i) {
        uint char = text_push_consts.text.text_data[i / 4] >> (8 * (3 - (i % 4))) & 0xFF;
        position[0] += font_data.positions[char][1][2] * text_push_consts.text.coordinates[0];
    }

    vec2 offset = vec2(0.0f, 0.0f);
    offset[0] += font_data.positions[current_char][1][0];
    offset[1] += font_data.positions[current_char][1][1];

    if(index_mod == 0 || index_mod == 5) {
        offset[0] += font_data.positions[current_char][0][2];
        offset[1] += font_data.positions[current_char][0][3];
    } else if (index_mod == 1) {
        offset[1] += font_data.positions[current_char][0][3];
    } else if (index_mod == 4) {
        offset[0] += font_data.positions[current_char][0][2];
    }

    position[0] += offset[0] * text_push_consts.text.coordinates[0];
    position[1] += offset[1] * text_push_consts.text.coordinates[1];

    gl_Position = orientation.transform * vec4(position, 0.0, 1.0);
    frag_tex_coord = GetTexCoord(current_char, index_mod);
    frag_color = text_push_consts.text.colors;
}