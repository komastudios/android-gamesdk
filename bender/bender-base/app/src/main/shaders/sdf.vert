#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"
layout(location = 0) out vec2 frag_tex_coord;

layout(push_constant)
uniform Push_consts {
    /*
        coordinates : (text_size_x, text_sie_y, posx, posy)
    */
    vec4 coordinates;
    uint string_data[28];
} push_consts;

layout(set = FONT_BINDING_SET, binding = FONT_VERTEX_MATRIX_BINDING)
uniform Orientation {
    mat4 transform;
} orientation;

layout(set = FONT_BINDING_SET, binding = FONT_VERTEX_UBO_BINDING)
uniform Font {
    /*
        position[0] : (pos_x, pos_y, width, height)
        position[1] : (x_offset, y_offset, x_advance, 0,0f)
        pos_x, pos_y : top-left coordinates of the character in texture space
        width, height : dimensions of the character in texture space
        x_offset, y_offset : for determining the vertex positions
    */
    vec4 positions[128][2];
} font;

vec2 GetTexCoord(uint current_char, uint current_position) {
    vec4 texture_data = font.positions[current_char][0];

    if(current_position == 0 || current_position == 5) {
        return vec2(texture_data[0] + texture_data[2], texture_data[1] + texture_data[3]);
    } else if (current_position == 2 || current_position == 3) {
        return vec2(texture_data[0], texture_data[1]);
    } else if (current_position == 1) {
        return vec2(texture_data[0], texture_data[1] + texture_data[3]);
    } else if (current_position == 4) {
        return vec2(texture_data[0] + texture_data[2], texture_data[1]);
    }

    // default, should not have happened
    return vec2(0.0f, 0.0f);
}

void main() {
    uint current_index = gl_VertexIndex / 6;
    uint current_position = gl_VertexIndex % 6;
    uint current_char = push_consts.string_data[current_index / 4] >> (8 * (3 - (current_index % 4))) & 0xFF;
    vec2 position = vec2(push_consts.coordinates[2], push_consts.coordinates[3]);

    for(int i = 0; i < current_index; ++i) {
        uint char = push_consts.string_data[i / 4] >> (8 * (3 - (i % 4))) & 0xFF;
        position[0] += font.positions[char][1][2] * push_consts.coordinates[0];
    }

    vec2 offset = vec2(0.0f, 0.0f);
    offset[0] += font.positions[current_char][1][0];
    offset[1] += font.positions[current_char][1][1];

    if(current_position == 0 || current_position == 5) {
        offset[0] += font.positions[current_char][0][2];
        offset[1] += font.positions[current_char][0][3];
    } else if (current_position == 1) {
        offset[1] += font.positions[current_char][0][3];
    } else if (current_position == 4) {
        offset[0] += font.positions[current_char][0][2];
    }

    position[0] += offset[0] * push_consts.coordinates[0];
    position[1] += offset[1] * push_consts.coordinates[1];

    gl_Position = orientation.transform * vec4(position, 0.0, 1.0);
    frag_tex_coord = GetTexCoord(current_char, current_position);
}