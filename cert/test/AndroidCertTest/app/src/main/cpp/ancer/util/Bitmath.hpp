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
    template <typename Rep1, typename Ratio1, typename Rep2, typename Ratio2>
    struct common_type<ancer::bitmath::Size<Rep1, Ratio1>,
                       ancer::bitmath::Size<Rep2, Ratio2>> {
        // TODO(tmillican@google.com): Ugly, but since we have a finite set of
        //  possible Ratios it does the job.
        using ratio_type = std::ratio<std::min(Ratio1::num, Ratio2::num),
                std::max(Ratio1::den, Ratio2::den)>;

        using type = ancer::bitmath::Size<std::common_type_t<Rep1, Rep2>,
                ratio_type>;
    };
}

//==============================================================================

namespace ancer::bitmath {
    template <typename Rep1, typename Ratio1, typename Rep2, typename Ratio2>
    static constexpr auto ToCommon(const Size<Rep1, Ratio1>& lhs,
                                   const Size<Rep2, Ratio2>& rhs) {
        using CommonType = std::common_type_t<Size<Rep1, Ratio1>, Size<Rep2, Ratio2>>;
        return std::pair{CommonType{lhs}, CommonType{rhs}};
    }

    // Heavily inspired by std::chrono::duration.
    template <typename Rep, typename Ratio>
    class Size {
    public:
        using rep_type = Rep;
        using ratio_type = Ratio;

        constexpr Size() = default;
        constexpr Size(const Size&) = default;
        constexpr Size(rep_type val) : _val{val} {}
        template <typename Rep2, typename Ratio2>
        constexpr Size(const Size<Rep2, Ratio2>& rhs);
        constexpr Size& operator = (const Size&) = default;

        static Size zero() { return {0}; }

        constexpr rep_type count() const { return _val; }

    private:
        rep_type _val = 0;

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------

// Preprocessor to make this managable.
// T is us, U is any other, V is common type. For each op, define:
//     T op T -> Actual code
//     T op U, U op T -> V op V
#define REPEAT_RHS_LHS(op) \
    template <typename Rep2, typename Ratio2> \
    friend constexpr bool operator op (const Size<Rep, Ratio>& lhs, \
                                       const Size<Rep2, Ratio2>& rhs) { \
        const auto [l,r] = ToCommon(lhs, rhs); return l op r; \
    } \
    template <typename Rep2, typename Ratio2> \
    friend constexpr bool operator op (const Size<Rep2, Ratio2>& lhs, \
                                       const Size<Rep, Ratio>& rhs) { \
        const auto [l,r] = ToCommon(lhs, rhs); return l op r; \
    }

// Comparison between two sizes
#define BITMATH_COMPARISON_OP(op) \
    friend constexpr bool operator op (const Size& lhs, const Size& rhs) \
    { return lhs.count() op rhs.count(); } \
    REPEAT_RHS_LHS(op)
// Binary op between two sizes
#define BITMATH_BINARY_OP(op) \
    friend constexpr auto operator op (const Size& lhs, const Size& rhs) \
    { return Size{lhs.count() op rhs.count()}; } \
    REPEAT_RHS_LHS(op)

// Binary op that returns a scalar.
#define BITMATH_BINARY_OP_SCALAR(op) \
    friend constexpr auto operator op (const Size& lhs, const Size& rhs) \
    { return lhs.count() op rhs.count(); } \
    REPEAT_RHS_LHS(op)

// Op with a size on the left and scalar on the right
#define BITMATH_RT_SCALAR_OP(op) \
    friend constexpr auto operator op (const Size& sz, Rep s) \
    { return Size{sz._val op s}; }
// Op with a size and a scalar
#define BITMATH_SCALAR_OP(op) \
    BITMATH_RT_SCALAR_OP(op) \
    friend constexpr auto operator op (Rep s, const Size& sz) \
    { return Size{sz._val op s}; }

//------------------------------------------------------------------------------

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
#undef REPEAT_RHS_LHS
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

    using DefaultStorage = int;
    using Bits      = BitsAs<DefaultStorage>;
    using Bytes     = BytesAs<DefaultStorage>;
    using Kilobytes = KilobytesAs<DefaultStorage>;
    using Megabytes = MegabytesAs<DefaultStorage>;
    using Gigabytes = GigabytesAs<DefaultStorage>;
}

//==============================================================================

#include "Bitmath.inl"
