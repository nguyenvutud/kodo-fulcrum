// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/api/fulcrum_nested_stack_interface.hpp>
#include <kodo_core/api/final_interface.hpp>

#include <kodo_fulcrum/api/nested_symbols.hpp>
#include <kodo_fulcrum/api/nested_symbol_size.hpp>

#include <gtest/gtest.h>
#include <stub/function.hpp>

/// @file test_fulcrum_nested_stack_interface.cpp Test for the
///       fulcrum_nested_stack_interface

namespace
{
    struct dummy : kodo_core::api::final_interface,
        kodo_fulcrum::api::fulcrum_nested_stack_interface
    {
        uint32_t nested_symbols() const
        {
            return m_nested_symbols();
        }

        uint32_t nested_symbol_size() const
        {
            return m_nested_symbol_size();
        }

        stub::function<uint32_t()> m_nested_symbols;
        stub::function<uint32_t()> m_nested_symbol_size;
    };
}

TEST(api_test_fulcrum_interface, nested_symbols)
{
    dummy d;
    d.m_nested_symbols.set_return(0U, 42U);

    EXPECT_EQ(0U, kodo_fulcrum::api::nested_symbols(&d));
    EXPECT_EQ(42U, kodo_fulcrum::api::nested_symbols(&d));
}

TEST(api_test_fulcrum_interface, nested_symbol_size)
{
    dummy d;
    d.m_nested_symbol_size.set_return(0U, 42U);

    EXPECT_EQ(0U, kodo_fulcrum::api::nested_symbol_size(&d));
    EXPECT_EQ(42U, kodo_fulcrum::api::nested_symbol_size(&d));
}
