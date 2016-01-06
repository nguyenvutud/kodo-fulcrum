// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/api/fulcrum_interface.hpp>

#include <gtest/gtest.h>
#include <stub/call.hpp>

#include <kodo_core/api/final_interface.hpp>

#include <kodo_fulcrum/api/expansion.hpp>
#include <kodo_fulcrum/api/inner_symbols.hpp>

/// @file test_fulcrum_interface.cpp Test for the fulcrum_interface

namespace
{
    struct dummy :
        kodo_core::api::final_interface, kodo_fulcrum::api::fulcrum_interface
    {
        uint32_t expansion() const
        {
            return m_expansion();
        }

        uint32_t inner_symbols() const
        {
            return m_inner_symbols();
        }

        stub::call<uint32_t()> m_expansion;
        stub::call<uint32_t()> m_inner_symbols;
    };
}

TEST(api_test_fulcrum_interface, expansion)
{
    dummy d;
    d.m_expansion.set_return({0U, 42U});

    EXPECT_EQ(0U, kodo_fulcrum::api::expansion(&d));
    EXPECT_EQ(42U, kodo_fulcrum::api::expansion(&d));
}

TEST(api_test_fulcrum_interface, inner_symbols)
{
    dummy d;
    d.m_inner_symbols.set_return({0U, 42U});

    EXPECT_EQ(0U, kodo_fulcrum::api::inner_symbols(&d));
    EXPECT_EQ(42U, kodo_fulcrum::api::inner_symbols(&d));
}
