#include "Bitmath.hpp"

#include <charconv>
#include <string_view>

#include <nlohmann/json.hpp>

#include "Error.hpp"
#include "Log.hpp"

using namespace ancer;
using namespace ancer::bitmath;


//==============================================================================

namespace {
    Log::Tag TAG{"ancer::bitmath"};
}

//==============================================================================

namespace {
    // Acceptable names for byte types.
    // The first entry is used for output; the rest are also acceptable as inputs.
    template <typename Size> constexpr std::string_view time_names[] = {};
    template <> constexpr std::string_view time_names<Bits>[] = {"bits"};
    template <> constexpr std::string_view time_names<Bytes>[] = {"bytes", "b"};
    template <> constexpr std::string_view time_names<Kilobytes>[] = {"kilobytes", "kb"};
    template <> constexpr std::string_view time_names<Megabytes>[] = {"megabytes", "mb"};
    template <> constexpr std::string_view time_names<Gigabytes>[] = {"gigabytes", "gb"};


    // TODO(tmillican@google.com): Everything from here down is largely common
    //  with Time.cpp; need to refactor.
    template <typename CastTo, typename Rep, typename Ratio>
    std::optional<CastTo> TryGetNumberAndCast(const std::string_view str) {
        if constexpr ( std::is_integral_v<Rep> ) {
            Rep num;
            if ( auto[p, ec] = std::from_chars(str.begin(), str.end(), num); ec == std::errc{} ) {
                auto duration = bitmath::Size<Rep, Ratio>{num};
                return BitmathCast<CastTo>(duration);
            } else {
                return std::nullopt;
            }
        } else {
            // Needs to be null terminated for stold
            std::string local_str{str.begin(), str.end()};
            size_t pos;
            Rep num = stold(local_str, &pos);
            if ( pos != 0 && pos == str.size()) {
                auto duration = bitmath::Size<Rep, Ratio>{num};
                return BitmathCast<CastTo>(duration);
            } else {
                return std::nullopt;
            }
        }
    }

    // Gets a time type from a string, but the suffix needs to match the type.
    template <typename OutputType, typename CheckType>
    std::optional<OutputType> TryGetBitStringAndCast(const std::string_view str) {
        const auto name_pos = str.find_first_not_of("0123456789. ");
        const bool match_found = [name_str = str.substr(name_pos)] {
            for ( const auto name : time_names<CheckType> ) {
                if ( name == name_str )
                    return true;
            }
            return false;
        }();
        if ( !match_found ) {
            return std::nullopt;
        }

        const auto numstr = str.substr(0, str.find_first_not_of("0123456789."));
        if ( numstr.find('.') != std::string_view::npos ) {
            return TryGetNumberAndCast<OutputType, double, typename CheckType::ratio_type>(numstr);
        } else {
            return TryGetNumberAndCast<OutputType, long, typename CheckType::ratio_type>(numstr);
        }
    }

    template <typename T>
    T TimeFromString(const std::string& str) {
        if (auto from_bi = TryGetBitStringAndCast<T, Bits>(str)) return *from_bi;
        if (auto from_by = TryGetBitStringAndCast<T, Bytes>(str)) return *from_by;
        if (auto from_kb = TryGetBitStringAndCast<T, Kilobytes>(str)) return *from_kb;
        if (auto from_mb = TryGetBitStringAndCast<T, Megabytes>(str)) return *from_mb;
        if (auto from_gb = TryGetBitStringAndCast<T, Gigabytes>(str)) return *from_gb;

        FatalError(TAG, "Failed to parse a valid time type from string: '%s'", str.c_str());
    }
}

namespace nlohmann {
#define BITMATH_SERIALIZER(Type) \
    static_assert(std::size(time_names<Type>) > 0, "Cannot serialize a type with no names."); \
    void adl_serializer<Type>::to_json(json& j, const Type& t) \
    { j = std::to_string(t.count()).append(" ").append(time_names<Type>[0]); } \
    void adl_serializer<Type>::from_json(const json& j, Type& t) \
    { t = TimeFromString<Type>(j.get<std::string>()); }

    BITMATH_SERIALIZER(Bits)
    BITMATH_SERIALIZER(Bytes)
    BITMATH_SERIALIZER(Kilobytes)
    BITMATH_SERIALIZER(Megabytes)
    BITMATH_SERIALIZER(Gigabytes)
#undef BITMATH_SERIALIZER
} // namespace nlohmann
