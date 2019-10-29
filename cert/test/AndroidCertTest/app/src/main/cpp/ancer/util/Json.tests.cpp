#include "Json.hpp"
#include <gtest/gtest.h>

using namespace ancer;


//==================================================================================================

namespace {
    struct Foo {
        int req = 0;
        int opt = 0;
    };

    JSON_CONVERTER(Foo) {
        JSON_REQVAR(req);
        JSON_OPTVAR(opt);
    }

    struct Bar {
        int set = 0;
        int opt_but_used = 0;
        Foo req_foo;
        Foo opt_foo;
    };

    JSON_CONVERTER(Bar) {
        JSON_SETVAR(set, 1);
        JSON_OPTVAR(opt_but_used);
        JSON_REQVAR(req_foo);
        JSON_OPTVAR(opt_foo);
    }
}

//==================================================================================================

TEST(ancer_json, json_read) {
    const/*expr*/ auto base_json = R"({
            "set" : 1,
            "opt_but_used" : 1,
            "req_foo" : {
                "req" : 1
            }
        })"_json;

    constexpr auto base = [] {
        Bar bar;
        bar.set = 1;
        bar.opt_but_used = 1;
        bar.req_foo.req = 1;
        return bar;
    }();

    auto bar = base_json.get<Bar>();

    EXPECT_EQ(bar.set, base.set);
    EXPECT_EQ(bar.opt_but_used, base.opt_but_used);
    EXPECT_EQ(bar.req_foo.req, base.req_foo.req);
    EXPECT_EQ(bar.req_foo.opt, base.req_foo.opt);
    EXPECT_EQ(bar.opt_foo.req, base.opt_foo.req);
    EXPECT_EQ(bar.opt_foo.opt, base.opt_foo.opt);
}

//==================================================================================================

TEST(ancer_json, json_write) {
    const Bar base = {
            0, 0,
            {1, 0},
            {0, 1}
    };
    auto json = Json(base);

    // Pulling it out piecemeal for easier comparison.
    Bar out;
    json["set"].get_to(out.set);
    json["opt_but_used"].get_to(out.opt_but_used);
    json["req_foo"]["req"].get_to(out.req_foo.req);
    json["req_foo"]["opt"].get_to(out.req_foo.opt);
    json["opt_foo"]["req"].get_to(out.opt_foo.req);
    json["opt_foo"]["opt"].get_to(out.opt_foo.opt);

    EXPECT_NE(out.set, base.set); // This should ignore the original value.
    EXPECT_EQ(out.opt_but_used, base.opt_but_used);
    EXPECT_EQ(out.req_foo.req, base.req_foo.req);
    EXPECT_EQ(out.req_foo.opt, base.req_foo.opt);
    EXPECT_EQ(out.opt_foo.req, base.opt_foo.req);
    EXPECT_EQ(out.opt_foo.opt, base.opt_foo.opt);
}
