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

#ifndef BENDER_BASE_ASTC_H
#define BENDER_BASE_ASTC_H

#define ASTC_MAGIC 0x5CA1AB13

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

VkFormat GetASTCFormat(uint8_t block_dim_x, uint8_t block_dim_y) {
  if (block_dim_x == 4 && block_dim_y == 4) {
    return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
  } else if (block_dim_x == 5 && block_dim_y == 4) {
    return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
  } else if (block_dim_x == 6 && block_dim_y == 5) {
    return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
  } else if (block_dim_x == 6 && block_dim_y == 6) {
    return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
  } else if (block_dim_x == 8 && block_dim_y == 5) {
    return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
  } else if (block_dim_x == 8 && block_dim_y == 6) {
    return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
  } else if (block_dim_x == 8 && block_dim_y == 8) {
    return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
  } else if (block_dim_x == 10 && block_dim_y == 5) {
    return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
  } else if (block_dim_x == 10 && block_dim_y == 6) {
    return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
  } else if (block_dim_x == 10 && block_dim_y == 8) {
    return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
  } else if (block_dim_x == 10 && block_dim_y == 10) {
    return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
  } else if (block_dim_x == 12 && block_dim_y == 10) {
    return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
  } else if (block_dim_x == 12 && block_dim_y == 12) {
    return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
  } else {
    return VK_FORMAT_UNDEFINED;
  }
}

bool ASTCHeaderIsValid(ASTCHeader header) {
  uint32_t magic = header.magic[0] | (uint32_t(header.magic[1]) << 8) |
                   (uint32_t(header.magic[2]) << 16) | (uint32_t(header.magic[3]) << 24);

  return magic == ASTC_MAGIC && header.block_dim_z == 1;
}

}

#endif //BENDER_BASE_ASTC_H
