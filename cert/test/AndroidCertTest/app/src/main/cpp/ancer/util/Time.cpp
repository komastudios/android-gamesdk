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

#include "Time.hpp"

#include <charconv>
#include <string_view>

#include <nlohmann/json.hpp>

#include "Error.hpp"
#include "Log.hpp"

using namespace ancer;


//==================================================================================================

namespace {
    Log::Tag TAG{"ancer::time"};
}

//==================================================================================================

namespace {
    // Acceptable names for support time types.
    // The first entry should be some sort of full name since that's what we use for output.
    template <typename Period> constexpr std::string_view time_names[] = {};
    template <> constexpr std::string_view time_names<Seconds>[] = {"seconds", "s", "sec"};
    template <> constexpr std::string_view time_names<Milliseconds>[] = {"milliseconds", "ms"};
    template <> constexpr std::string_view time_names<Nanoseconds>[] = {"nanoseconds", "ns"};


    template <typename CastTo, typename Rep, typename Period>
    std::optional<CastTo> TryGetNumberAndCast(const std::string_view str) {
        // HACK: from_chars only implemented for integral values right now.
        if constexpr ( std::is_integral_v<Rep> ) {
            Rep num;
            if ( auto[p, ec] = std::from_chars(str.begin(), str.end(), num); ec == std::errc{} ) {
                auto duration = std::chrono::duration<Rep, Period>{num};
                return std::chrono::duration_cast<CastTo>(duration);
            } else {
                return std::nullopt;
            }
        } else {
            std::string local_str{str.begin(), str.end()}; // Needs to be null terminated.
            size_t pos;
            Rep num = stold(local_str, &pos);
            if ( pos != 0 && pos == str.size()) {
                auto duration = std::chrono::duration<Rep, Period>{num};
                return std::chrono::duration_cast<CastTo>(duration);
            } else {
                return std::nullopt;
            }
        }
    }


    // Gets a time type from a string, but the suffix needs to match the type.
    template <typename OutputType, typename CheckType>
    std::optional<OutputType> TryGetTimeStringAndCast(const std::string_view str) {
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
            return TryGetNumberAndCast<OutputType, double, typename CheckType::period>(numstr);
        } else {
            return TryGetNumberAndCast<OutputType, long, typename CheckType::period>(numstr);
        }
    }


    template <typename T>
    T TimeFromString(const std::string& str) {
        if ( auto from_sec = TryGetTimeStringAndCast<T, Seconds>(str)) return *from_sec;
        if ( auto from_ms = TryGetTimeStringAndCast<T, Milliseconds>(str)) return *from_ms;
        if ( auto from_ns = TryGetTimeStringAndCast<T, Nanoseconds>(str)) return *from_ns;

        FatalError(TAG, "Failed to parse a valid time type from string: '%s'", str.c_str());
    }
} // anonymous namespace


namespace nlohmann {
    void adl_serializer<Timestamp>::to_json(json& j, const Timestamp& t) {
        j = t.time_since_epoch().count();
    }


#define TIME_SERIALIZER(Type) \
    static_assert(std::size(time_names<Type>) > 0, "Cannot serialize a type with no names."); \
    void adl_serializer<Type>::to_json(json& j, const Type& t) \
    { j = std::to_string(t.count()).append(" ").append(time_names<Type>[0]); } \
    void adl_serializer<Type>::from_json(const json& j, Type& t) \
    { t = TimeFromString<Type>(j.get<std::string>()); }

    TIME_SERIALIZER(Seconds)

    TIME_SERIALIZER(Milliseconds)

    TIME_SERIALIZER(Nanoseconds)
} // namespace nlohmann
