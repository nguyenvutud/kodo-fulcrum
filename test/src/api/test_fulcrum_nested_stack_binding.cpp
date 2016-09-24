// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/api/fulcrum_nested_stack_binding.hpp>

#include <gtest/gtest.h>
#include <stub/function.hpp>

/// @file test_fulcrum_nested_stack_binding.cpp Test for the
///       fulcrum_nested_stack_binding

namespace
{
struct dummy_stack
{
    stub::function<uint32_t()> nested_symbols;
    stub::function<uint32_t()> nested_symbol_size;
};

struct dummy
{
    stub::function<dummy_stack*()> stack;
};

using stack = kodo_fulcrum::api::fulcrum_nested_stack_binding<dummy>;
}

TEST(api_test_fulcrum_nested_stack_binding, nested_symbols)
{
    stack s;
    dummy_stack d;
    d.nested_symbols.set_return(0U, 42U);
    s.stack.set_return(&d);
    EXPECT_EQ(0U, s.nested_symbols());
    EXPECT_EQ(42U, s.nested_symbols());
}

TEST(api_test_fulcrum_nested_stack_binding, nested_symbol_size)
{
    stack s;
    dummy_stack d;
    d.nested_symbol_size.set_return(0U, 42U);
    s.stack.set_return(&d);
    EXPECT_EQ(0U, s.nested_symbol_size());
    EXPECT_EQ(42U, s.nested_symbol_size());
}
