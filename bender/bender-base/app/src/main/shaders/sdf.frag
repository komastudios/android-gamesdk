#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "shader_bindings.h"

layout(set = FONT_BINDING_SET, binding = FONT_FRAG_SAMPLER_BINDING)
uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragTexColor;


layout(location = 0) out vec4 outColor;

void main() {
    /*
     * Distance: ranging [0.0f, 1.0f], representing the distance away from the nearest glyph stroke
     *           0.0f means outside of the glyph stroke, 0.5f is on the edge of it
     *           and 1.0f states in the middle of the stroke.
     *           By sampling this value we obtain the information of which pixels should be revealed
     *           according to the opacity.
     *
     * smoothWidth: To identify the edges of glyphs, we need the partial derivates of
     *              the value *distance* w.r.t to x and y screen dimensions (0.5f and 0.0f)
     *              which means, in pixels, the difference of between its neighboring pixels.
     *              The function fwidth() does the exact operation for us.
     *
     * alpha: After getting the edges we need to do some anti-aliasing and clamping the value inside
     *        the glyph. smoothstep() is a sigmoid-like function to help us to do so.
     */
    float distance = texture(texSampler, fragTexCoord).a;
    float smoothWidth = fwidth(distance);
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);

    outColor = vec4(fragTexColor.rgb, alpha * fragTexColor.a);
}
