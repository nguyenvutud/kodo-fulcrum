// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/api/fulcrum_config_binding.hpp>

#include <gtest/gtest.h>
#include <stub/call.hpp>

/// @file test_fulcrum_config_binding.cpp Test for the fulcrum_config_binding

namespace
{
    struct dummy_stack
    {
        stub::call<uint32_t()> max_expansion;
        stub::call<void(uint32_t)> set_expansion;
        stub::call<uint32_t()> max_inner_symbols;
    };

    struct dummy
    {
        stub::call<dummy_stack*()> stack;
    };

    using stack = kodo::fulcrum::api::fulcrum_config_binding<dummy>;
}

TEST(api_test_fulcrum_config_binding, max_expansion)
{
    stack s;
    dummy_stack d;
    d.max_expansion.set_return({0U, 42U});
    s.stack.set_return(&d);
    EXPECT_EQ(0U, s.max_expansion());
    EXPECT_EQ(42U, s.max_expansion());
}

TEST(api_test_fulcrum_config_binding, set_expansion)
{
    stack s;
    dummy_stack d;
    s.stack.set_return(&d);
    uint32_t expansion = 42U;
    s.set_expansion(expansion);
    EXPECT_TRUE(d.set_expansion.expect_calls().with(expansion).to_bool());
}

TEST(api_test_fulcrum_config_binding, max_inner_symbols)
{
    stack s;
    dummy_stack d;
    d.max_inner_symbols.set_return({0U, 42U});
    s.stack.set_return(&d);
    EXPECT_EQ(0U, s.max_inner_symbols());
    EXPECT_EQ(42U, s.max_inner_symbols());
}
