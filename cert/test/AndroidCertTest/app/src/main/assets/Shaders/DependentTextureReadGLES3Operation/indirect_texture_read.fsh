#version 300 es

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

precision mediump float;

uniform sampler2D uTex;

// Random multiplier (same across all fragments for the same frame; but a different one each frame).
uniform float uMultiplier;  // ranges between 0.0 and 1.0.

// Number of indirect texture readings to perform (if 0, no indirections).
uniform int uIndirections;

in vec2 vTexCoord;

out vec4 outColor;

void main() {
    vec2 lastTextCoord = vTexCoord;
    outColor = texture(uTex, lastTextCoord);  // so far, the original texture read directly.

    for (int i = 0; i < uIndirections; ++i) {
        // outColor[red] acts as "delta x", outcolor[green] as "delta y".
        vec2 deltaXY = vec2(outColor) * uMultiplier;
        // Each delta component ranges between 0.0 and 1.0
        lastTextCoord += deltaXY;

        // If applying dx busted the rightmost bound, it cycles back from the left.
        if (lastTextCoord.x > 1.0) {
            lastTextCoord.x -= 1.0;
        }
        // Same with the y-axis.
        if (lastTextCoord.y > 1.0) {
            lastTextCoord.y -= 1.0;
        }

        // Each iteration ends with an indirect reading.
        outColor = texture(uTex, lastTextCoord);
    }
}
