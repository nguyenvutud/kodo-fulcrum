// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/api/fulcrum_binding.hpp>

#include <gtest/gtest.h>
#include <stub/call.hpp>

/// @file test_fulcrum_binding.cpp Test for the fulcrum_binding

namespace
{
    struct dummy_stack
    {
        stub::call<uint32_t()> expansion;
        stub::call<uint32_t()> inner_symbols;
    };

    struct dummy
    {
        stub::call<dummy_stack*()> stack;
    };

    using stack = kodo_fulcrum::api::fulcrum_binding<dummy>;
}

TEST(api_test_fulcrum_binding, expansion)
{
    stack s;
    dummy_stack d;
    d.expansion.set_return({0U, 42U});
    s.stack.set_return(&d);
    EXPECT_EQ(0U, s.expansion());
    EXPECT_EQ(42U, s.expansion());
}

TEST(api_test_fulcrum_binding, inner_symbols)
{
    stack s;
    dummy_stack d;
    d.inner_symbols.set_return({0U, 42U});
    s.stack.set_return(&d);
    EXPECT_EQ(0U, s.inner_symbols());
    EXPECT_EQ(42U, s.inner_symbols());
}
