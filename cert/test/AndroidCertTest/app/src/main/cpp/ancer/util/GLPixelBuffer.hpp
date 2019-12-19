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

#include <vector>
#include <string>

#include <GLES/gl.h>
#include <glm/glm.hpp>

namespace ancer {

/*
 * GLPixelBuffer is an abstraction on top of OpenGL's `glReadPixels` function
 * for the purpose of sampling individual pixels as RGBA values, as well as for
 * generating a PNG screenshot of the framebuffer.
 *
 * The intended usage pattern is to create a GLPixelBuffer once the OpenGL
 * context is ready (and once the viewport has been set), and then call
 * `CopyFromFramebuffer()` to capture a frame. The image can then either be
 * sampled with `ReadPixel` or converted to a PNG with `ToPNG()`.
 */
class GLPixelBuffer {
public:
    GLPixelBuffer();

    ~GLPixelBuffer();

    GLPixelBuffer(const GLPixelBuffer&) = delete;
    GLPixelBuffer& operator=(const GLPixelBuffer&) = delete;

    GLPixelBuffer(GLPixelBuffer&&) = delete;
    GLPixelBuffer& operator=(GLPixelBuffer&&) = delete;

    unsigned int Width() const;

    unsigned int Height() const;

    unsigned int Components() const;

    /*
     * Samples from the existing pixel buffer, does not for update on the buffer.
     * (0,0) is lower-left corner, (Width(),Height()) is upper-right corner.
     */
    glm::u8vec4 ReadPixel(int x, int y) const;

    /*
     * Copies all pixels from the currently bound framebuffer for later use.
     * Throws a FatalError if the framebuffer has different dimensions.
     */
    void CopyFromFramebuffer();

    /*
     * Returns the pixels of the current stored image. A pixel is an RGBA value
     * where each channel is one byte; thus, a pixel is four consecutive bytes.
     * The first (4 * Width()) bytes of the returned array corresponds with the
     * first row of the image, where the first row is either the bottom of the
     * image if flipped == false (matching OpenGL's coordinate system), or the
     * top of the image if flipped == true.
     *
     * (Note: the default value for flipped is false to match OpenGL's scheme,
     * but in the case of encoding the data as an image, it is almost always
     * desired to flip the result.)
     */
    std::vector<unsigned char> RGBABytes(bool flipped = false) const;

    std::vector<unsigned char> ToPNG() const;

private:
    GLubyte *_pixels = nullptr;
    GLenum _format = 0;
    GLenum _type = 0;
    unsigned int _components = 0;
    unsigned int _width = 0;
    unsigned int _height = 0;
};
} // end ancer namespace
