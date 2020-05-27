// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BENDER_BASE_UTILS_SRC_ASTC_H
#define BENDER_BASE_UTILS_SRC_ASTC_H

#include <stdint.h>
#include "vulkan_wrapper.h"

namespace astc {

struct ASTCHeader {
    uint8_t magic[4];
    uint8_t block_dim_x;
    uint8_t block_dim_y;
    uint8_t block_dim_z;
    uint8_t x_size[3];
    uint8_t y_size[3];
    uint8_t z_size[3];
};
static_assert(sizeof(ASTCHeader) == 16, "Packed ASTC header struct is not 16 bytes.");

VkFormat GetASTCFormat(VkFormat template_format, uint8_t block_dim_x, uint8_t block_dim_y);

bool ASTCHeaderIsValid(ASTCHeader header);

}

#endif //BENDER_BASE_ASTC_H
