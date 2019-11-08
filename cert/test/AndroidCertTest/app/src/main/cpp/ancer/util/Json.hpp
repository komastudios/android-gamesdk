#pragma once

#include <nlohmann/json.hpp>


namespace ancer {
    using Json = nlohmann::json;
}

//==================================================================================================

// Defines a function that automatically converts a given type to & from JSON.
// If your logic requires it, the data being read/written to/from is simply named 'data'.
//
// namespace foo { // Our JSON lib uses ADL, so this actually needs to be in the Foo's namespace.
//   JSON_CONVERTER(Foo) { // Defines conversions to/from JSON for Foo.
//     JSON_REQVAR(bar);    // Foo.bar <-> json["bar"]; Fails if json["baz"] doesn't exist.
//     JSON_OPTVAR(baz); // Foo.baz <-> json["baz"]; Won't fail if json["baz"] doesn't exist.
//   }
// }
#define JSON_CONVERTER(Type) \
    INTERNAL_JSON_BOILERPLATE(Type) \
    INTERNAL_TO_JSON(Type) \
    INTERNAL_FROM_JSON(Type) \
    INTERNAL_CONVERT_BEGIN(Type)

// Like JSON_CONVERTER, but only reads JSON.
#define JSON_READER(Type) \
    INTERNAL_JSON_BOILERPLATE(Type) \
    INTERNAL_FROM_JSON(Type) \
    INTERNAL_CONVERT_BEGIN(Type)

// Like JSON_CONVERTER, but only writes to JSON.
#define JSON_WRITER(Type) \
    INTERNAL_JSON_BOILERPLATE(Type) \
    INTERNAL_TO_JSON(Type) \
    INTERNAL_CONVERT_BEGIN(Type)

//==============================================================================

// If you have logic that differs depending on if we're reading or writing.
// Note that these are constexpr.
#define IS_TO_JSON() Converter::is_to_json
#define IS_FROM_JSON() Converter::is_from_json

// Declares a required JSON variable that corresponds to a struct member.
#define JSON_REQVAR(name) _converter.template Convert< true>(#name, data.name)
// Declares an optional JSON variable that corresponds to a struct member. Will
// read nothing if the variable isn't found in JSON.
#define JSON_OPTVAR(name) _converter.template Convert<false>(#name, data.name)
// Declares a variable and reads/writes a given value.
#define JSON_SETVAR(name, value) \
    do { if constexpr ( IS_TO_JSON() ) { \
        _converter.template Convert<true>(#name, value); \
    } else { \
        data.name = value; \
    } } while(false)

#define JSON_REQENUM(name, enum_names) \
    _converter.template ConvertEnum< true, std::size(enum_names)>(#name, data.name, enum_names)
#define JSON_OPTENUM(name, enum_names) \
    _converter.template ConvertEnum<false, std::size(enum_names)>(#name, data.name, enum_names)

//==============================================================================

#include "Json.inl"
