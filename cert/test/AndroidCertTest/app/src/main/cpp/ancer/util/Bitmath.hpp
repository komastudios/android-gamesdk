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

#include <algorithm>
#include <ratio>


//==============================================================================

namespace ancer::bitmath {
    template <typename, typename> class Size;
}

namespace std {
    template <typename Rep1, typename Period1, typename Rep2, typename Period2>
    struct common_type<ancer::bitmath::Size<Rep1, Period1>,
                       ancer::bitmath::Size<Rep2, Period2>> {
        // TODO(tmillican@google.com): Ugly, but since we have a finite set of
        //  possible periods it does the job.
        using ratio_type = std::ratio<std::min(Period1::num, Period2::num),
                std::max(Period1::den, Period2::den)>;

        using type = ancer::bitmath::Size<std::common_type_t<Rep1, Rep2>,
                ratio_type>;
    };
}

//==============================================================================

namespace ancer::bitmath {
    // Heavily inspired by std::chrono::duration.
    template <typename Rep, typename Ratio>
    class Size {
    public:
        using rep_type = Rep;
        using ratio_type = Ratio;

        constexpr Size() = default;
        constexpr Size(const Size&) = default;
        constexpr Size(rep_type val) : _val{val} {}
        template <typename Rep2, typename Period2>
        constexpr Size(const Size<Rep2, Period2>& rhs);
        constexpr Size& operator = (const Size&) = default;

        static Size zero() { return {0}; }

        constexpr rep_type count() const { return _val; }

    private:
        rep_type _val = 0;

        //----------------------------------------------------------------------
    public:
        explicit constexpr operator bool () const { return (bool)_val; }

        constexpr Size operator +() const { return *this; }
        constexpr Size operator -() const { return {-_val}; }

        constexpr Size& operator ++() { ++_val; return *this; }
        constexpr Size  operator ++(int) { return {_val++}; }
        constexpr Size& operator --() { --_val; return *this; }
        constexpr Size  operator --(int) { return {_val--}; }

        constexpr Size& operator += (const Size& s) { _val += s._val; return *this; }
        constexpr Size& operator -= (const Size& s) { _val -= s._val; return *this; }
        constexpr Size& operator %= (const Size& s) { _val %= s._val; return *this; }
        constexpr Size& operator *= (rep_type rhs) { _val *= rhs; return *this; }
        constexpr Size& operator /= (rep_type rhs) { _val /= rhs; return *this; }
        constexpr Size& operator %= (rep_type rhs) { _val %= rhs; return *this; }

        //----------------------------------------------------------------------

        template <typename Rep1, typename Period1, typename Rep2, typename Period2>
        static constexpr auto ToCommon(const Size<Rep1, Period1>& lhs,
                                       const Size<Rep2, Period2>& rhs) {
            using CommonType = std::common_type_t<Size<Rep1, Period1>, Size<Rep2, Period2>>;
            return std::pair{CommonType{lhs}, CommonType{rhs}};
        }

// Comparison between two sizes
#define BITMATH_COMPARISON_OP(op) \
    template <typename Rep1, typename Period1, typename Rep2, typename Period2> \
    friend constexpr bool operator op (const Size<Rep1, Period1>& lhs, \
                                const Size<Rep2, Period2>& rhs) { \
        const auto [l,r] = ToCommon(lhs, rhs); return l.count() op r.count(); \
    }
// Binary op between two sizes
#define BITMATH_BINARY_OP(op) \
    template <typename Rep1, typename Period1, typename Rep2, typename Period2> \
    friend constexpr auto operator op (const Size<Rep1, Period1>& lhs, \
                                       const Size<Rep2, Period2>& rhs) { \
        const auto [l,r] = ToCommon(lhs, rhs); \
        return decltype(l){l.count() op r.count()}; \
    }
#define BITMATH_BINARY_OP_SCALAR(op) \
    template <typename Rep1, typename Period1, typename Rep2, typename Period2> \
    friend constexpr auto operator op (const Size<Rep1, Period1>& lhs, \
                                       const Size<Rep2, Period2>& rhs) { \
        const auto [l,r] = ToCommon(lhs, rhs); \
        return l.count() op r.count(); \
    }
// Op with a size on the left and scalar on the right
#define BITMATH_RT_SCALAR_OP(op) \
    template <typename Rep1, typename Period1, typename FreeRep> \
    friend constexpr auto operator op (const Size<Rep1, Period1>& sz, FreeRep s) \
    { return Size<Rep1, Period1>{sz._val op (Rep1)s}; }
// Op with a size and a scalar
#define BITMATH_SCALAR_OP(op) \
    template <typename Rep1, typename Period1, typename FreeRep> \
    friend constexpr auto operator op (const Size<Rep1, Period1>& sz, FreeRep s) \
    { return Size<Rep1, Period1>{sz._val op s}; } \
    template <typename Rep1, typename Period1, typename FreeRep> \
    friend constexpr auto operator op (FreeRep s, const Size<Rep1, Period1>& sz) \
    { return Size<Rep1, Period1>{sz._val op s}; }

        BITMATH_COMPARISON_OP(==)
        BITMATH_COMPARISON_OP(!=)
        BITMATH_COMPARISON_OP(<)
        BITMATH_COMPARISON_OP(<=)
        BITMATH_COMPARISON_OP(>)
        BITMATH_COMPARISON_OP(>=)

        BITMATH_BINARY_OP(+)
        BITMATH_BINARY_OP(-)
        BITMATH_BINARY_OP(*)
        BITMATH_BINARY_OP_SCALAR(/)
        BITMATH_BINARY_OP_SCALAR(%)

        BITMATH_SCALAR_OP(*)
        BITMATH_RT_SCALAR_OP(/)
        BITMATH_RT_SCALAR_OP(%)

#undef BITMATH_COMPARISON_OP
#undef BITMATH_BINARY_OP
#undef BITMATH_RT_SCALAR_OP
#undef BITMATH_SCALAR_OP
    };
}

//==============================================================================

namespace ancer::bitmath {
    template <typename SizeType, typename SrcRep, typename SrcRatio>
    constexpr SizeType BitmathCast(const Size<SrcRep, SrcRatio>& s) {
        using DstRep = typename SizeType::rep_type;
        using DstRatio = typename SizeType::ratio_type;

        using CommonRep = std::common_type_t<DstRep, SrcRep>;
        CommonRep val = s.count();
        // TODO(tmillican@google.com): Kind of sloppy, and given the tight
        //  restrictions on potential ratios we can surely do better.
        val = val * ((double)DstRatio::den / (double)SrcRatio::den);
        val = val * ((double)SrcRatio::num / (double)DstRatio::num);
        return {static_cast<DstRep>(val)};
    }

//------------------------------------------------------------------------------

    using BitRatio      = std::ratio<1, 8>;
    using ByteRatio     = std::ratio<1>;
    using KilobyteRatio = std::ratio<1024>;
    using MegabyteRatio = std::ratio<1024*1024>;
    using GigabyteRatio = std::ratio<1024*1024*1024>;

    template <typename T> using BitsAs      = Size<T, BitRatio>;
    template <typename T> using BytesAs     = Size<T, ByteRatio>;
    template <typename T> using KilobytesAs = Size<T, KilobyteRatio>;
    template <typename T> using MegabytesAs = Size<T, MegabyteRatio>;
    template <typename T> using GigabytesAs = Size<T, GigabyteRatio>;

    using DefaultStorage = long;
    using Bits      = BitsAs<DefaultStorage>;
    using Bytes     = BytesAs<DefaultStorage>;
    using Kilobytes = KilobytesAs<DefaultStorage>;
    using Megabytes = MegabytesAs<DefaultStorage>;
    using Gigabytes = GigabytesAs<DefaultStorage>;
}

//==============================================================================

#include "Bitmath.inl"
