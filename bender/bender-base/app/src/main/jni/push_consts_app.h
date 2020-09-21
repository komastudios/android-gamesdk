//
// Created by chingtangyu on 4/27/2020.
//

typedef glm::vec4 vec4;
typedef uint32_t uint;

#include "push_constants.h"
#include <assert.h>

static_assert(sizeof(FontPushConstants) <= 128, "The size of the push const exceeds minimum block size");
