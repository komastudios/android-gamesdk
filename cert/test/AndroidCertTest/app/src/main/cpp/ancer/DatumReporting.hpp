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

#pragma once

#include <list>
#include <map>
#include <string_view>
#include <vector>

#include "Reporting.hpp"
#include "util/Bitmath.hpp"
#include "util/Time.hpp"


//==============================================================================
// To declare a type for use with the reporting system, first you'll need to
// declare WriteDatum():
//
//     void WriteDatum(ancer::report_writers::Struct w, const YourType& d)
//
// The report writer depends on the type; usually you'll be using Struct, but
// List and Basic may also come up for list-like and simple outputs. In any
// case, the writer should almost always be by-value so your function creates
// and destroys its own writer.
//
// For the signature of YourType, note that the datum at this point will likely
// be owned by an internal worker thread. If you want to avoid unnecessary
// copies (e.g., your datum owns a vector that you clear immediately after
// reporting), you'll need to call std::move() when calling Report().
//
//
// Once declared, the actual conversion can take place. For 90% of cases, this
// will be fairly straightforward: As long
//     void WriteDatum(report_writers::Struct w, const Datum& d) {
//         // Output: {"foo":1,"bar":{"bardata":3},"baz":true}
//         w.AddItem("foo", d.foo); // int : Standard type
//         w.AddItem("bar", d.bar); // Bar : Defined its own WriteDatum()
//         w.AddItem("baz", true);  // You can also write data on-the-fly
//
//         // To cut down on boilerplate/typos, we have the following helpers:
//         w.AddItem(DATUM_MEMBER(d, foo)); // These are both identical to
//         ADD_DATUM_MEMBER(w, d, foo);     // w.AddItem("foo": d.foo)
//     }
//
// If you have an array-like output, there's an AddItem for that too:
//     void WriteDatum(report_writers::List w, const Datum& d) {
//         // Output: [a, b, c]
//         w.AddItem(d.a);
//         w.AddItem(d.b);
//         w.AddItem(d.c);
//     }
//
// For cases where more flexibility is required, you can write directly to the
// output string. This will always work regardless of the writer type, but
// using the Basic writer is generally suggested.
//    void WriteDatum(report_writers::Basic w, const Datum& d) {
//        // Output: "Data(8):xxxxxxxx"
//        w.Print("Data(");      // Prints a string/character.
//        w.ToChars(d.buf_size); // Writes a number directly to the buffer.
//        w.Print("):");
//        w.PrintData(buf, buf_size); // Prints raw binary data.
//    }
//==============================================================================

namespace ancer::reporting::internal {
    template <typename Behavior>
    struct ScopedWriter {
        // Implicit conversions around ReportWriter so
        ScopedWriter(ReportWriter& w_) : w(w_) { b.Begin(w); }
        template <typename B>
        ScopedWriter(ScopedWriter<B>& w_) : w(w_.w) { b.Begin(w); }
        ScopedWriter(const ScopedWriter&) = delete;
        ~ScopedWriter() { b.End(w); }

        // Direct printing, mainly for numeric/basic types but may be used
        // to "hard-code" around special pseudo-members.
        template <typename T> void ToChars(T val) { w.ToChars(val); }
        void Print(char c) { w.Print(c); }
        void Print(std::string_view str) { w.Print(str); }
        void PrintData(const void* data, size_t sz) { w.PrintData(data, sz); }

        template <typename T>
        void AddItem(T&& value) {
            b.NewItem(w);
            WriteDatum(w, std::forward<T>(value));
        }
        template <typename T>
        void AddItem(std::string_view name, T&& value) {
            b.NewItem(w, name);
            WriteDatum(w, std::forward<T>(value));
        }

        ReportWriter& w;
        Behavior b;
    };

#define DATUM_MEMBER(value, name) #name, value.name
#define ADD_DATUM_MEMBER(writer, value, name) writer.AddItem(DATUM_MEMBER(value, name))

//------------------------------------------------------------------------------

    // Non-string values (ints & floats)
    struct NumericType {
        void Begin(ReportWriter&) {}
        void End(ReportWriter&) {}
    };

    // Simple value wrapped in quotes
    struct BasicType {
        void Begin(ReportWriter& w) { w.Print('"'); }
        void   End(ReportWriter& w) { w.Print('"'); }
    };

    // Multiple values wrapped in []
    struct ListType {
        void Begin(ReportWriter& w) { w.Print('['); }
        void   End(ReportWriter& w) { w.Print(']'); }

        bool has_value = false;
        void NewItem(ReportWriter& w) {
            if (has_value) { w.Print(", "); }
            has_value = true;
        }
    };

    // Multiple named values wrapped in {}
    struct StructType {
        void Begin(ReportWriter& w) { w.Print('{'); }
        void   End(ReportWriter& w) { w.Print('}'); }

        bool has_value = false;
        void NewItem(ReportWriter& w, std::string_view name) {
            if (has_value) { w.Print(", "); }
            has_value = true;
            w.Print('\"'); w.Print(name); w.Print("\":");
        }
    };
}

//------------------------------------------------------------------------------

namespace ancer::report_writers {
    using Generic = reporting::internal::ReportWriter&;
    using Numeric = reporting::internal::ScopedWriter<reporting::internal::NumericType>;
    using Basic   = reporting::internal::ScopedWriter<reporting::internal::BasicType>;
    using List    = reporting::internal::ScopedWriter<reporting::internal::ListType>;
    using Struct  = reporting::internal::ScopedWriter<reporting::internal::StructType>;
}

//==============================================================================
// Common types

// Note: Will be called correctly because of ADL on writer.
namespace ancer::reporting::internal {

//------------------------------------------------------------------------------
// Fundamental types
    template <typename T>
    inline void WriteDatum(report_writers::Numeric w, T v,
                           std::enable_if_t<std::is_integral_v<T>, void*> = nullptr) {
        // TODO(tmillican@google.com): Running into value_too_large?
        if constexpr (sizeof(T) >= sizeof(long)) {
            w.Print(std::to_string(v));
        } else {
            w.ToChars(v);
        }
    }
    template <typename T>
    inline void WriteDatum(report_writers::Numeric w, T v,
                           std::enable_if_t<std::is_floating_point_v<T>, void*> = nullptr) {
        // TODO(tmillican@google.com): Floating point versions of to_chars are missing?
        w.Print(std::to_string(v));
    }
    // Using SFINAE to avoid ambiguity with integral types if we don't have an exact match.
    template <typename T>
    inline void WriteDatum(report_writers::Generic w, T b,
                           std::enable_if_t<std::is_same_v<T, bool>, void*> = nullptr) {
        w.Print(b ? "true" : "false");
    }

    // TODO(tmillican@google.com): If/when we improve our support for enum-to-
    //  string, add that here if we're still outputting text.
    template <typename T>
    inline void WriteDatum(report_writers::Generic w, T v,
                           std::enable_if_t<std::is_enum_v<T>, void*> = nullptr) {
        // if constexpr ( StringConverterExists<T>() ) { ... } else
        // TODO(tmillican@google.com): WriteDatum() to get around ToChars issues.
        WriteDatum(w, static_cast<std::underlying_type_t<T>>(v));
    }

    inline void WriteDatum(report_writers::Basic w, std::string_view str) {
        w.Print(str);
    }

//------------------------------------------------------------------------------
// Size & Time
// TODO(tmillican@google.com): Integrate with JSON setup for these types.
// TODO(tmillican@google.com): WriteDatum() to get around ToChars issues.
#define SUFFIXED_REPORTER(Type, suffix) \
    template <typename Rep> \
    inline void WriteDatum(report_writers::Basic w, Type<Rep> v) { \
        WriteDatum(w, v.count()); w.Print(" " suffix); \
    }
    SUFFIXED_REPORTER(bitmath::BitsAs,      "bits")
    SUFFIXED_REPORTER(bitmath::BytesAs,     "bytes")
    SUFFIXED_REPORTER(bitmath::KilobytesAs, "kilobytes")
    SUFFIXED_REPORTER(bitmath::MegabytesAs, "megabytes")
    SUFFIXED_REPORTER(bitmath::GigabytesAs, "gigabytes")

    SUFFIXED_REPORTER(NanosecondsAs,  "nanoseconds")
    SUFFIXED_REPORTER(MillisecondsAs, "milliseconds")
    SUFFIXED_REPORTER(SecondsAs,      "seconds")
#undef SUFFIXED_REPORTER

    inline void WriteDatum(report_writers::Generic w, Timestamp t) {
        WriteDatum(w, (Timestamp::rep)t.time_since_epoch().count());
    }

//------------------------------------------------------------------------------
// Containers

#define LIST_REPORTER(Container) \
template <typename T> \
void WriteDatum(report_writers::List w, Container c) { \
    for (const auto& value : c) { w.AddItem(value); } \
}
    LIST_REPORTER(const std::vector<T>&)
    LIST_REPORTER(const std::list<T>&)
#undef LIST_REPORTER


#define MAP_REPORTER(...) \
template <typename T, typename U> \
void WriteDatum(report_writers::Struct w, __VA_ARGS__ c) { \
    for (auto&& [name, value] : c) { w.AddItem(name, value); } \
}
    MAP_REPORTER(const std::map<T,U>&)
#undef MAP_REPORTER
}
