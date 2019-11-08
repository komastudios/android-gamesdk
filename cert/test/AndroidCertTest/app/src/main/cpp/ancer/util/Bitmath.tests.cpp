#include "Bitmath.hpp"
#include <gtest/gtest.h>

using namespace ancer;
using namespace ancer::bitmath;

// Change to EXPECT_EQ if something is failing and you want to debug at runtime
#define STATIC_EXPECT_EQ(a, b) static_assert(a == b)


//==============================================================================

TEST(ancer_bytes, simple_conversions) {
    {
        constexpr Bits bits{16};
        constexpr auto bytes = BitmathCast<Bytes>(bits);
        STATIC_EXPECT_EQ(bytes.count(), 2);
    }
    {
        constexpr Kilobytes kilo{1};
        constexpr auto bytes = BitmathCast<Bytes>(kilo);
        STATIC_EXPECT_EQ(bytes.count(), 1024);
    }
    {
        constexpr BytesAs<char> char_bytes{128};
        constexpr auto int_bytes = BitmathCast<Bytes>(char_bytes);
        STATIC_EXPECT_EQ(int_bytes.count(), 128);
    }
}
