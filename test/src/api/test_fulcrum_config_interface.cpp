// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/api/fulcrum_config_interface.hpp>

#include <gtest/gtest.h>
#include <stub/function.hpp>

#include <kodo_core/api/final_interface.hpp>
#include <kodo_fulcrum/api/max_expansion.hpp>
#include <kodo_fulcrum/api/set_expansion.hpp>
#include <kodo_fulcrum/api/max_inner_symbols.hpp>

/// @file test_fulcrum_interface.cpp Test for the payload_size_interface

namespace
{
struct dummy :
    kodo_core::api::final_interface,
    kodo_fulcrum::api::fulcrum_config_interface
{
    uint32_t max_expansion() const override
    {
        return m_max_expansion();
    }

    void set_expansion(uint32_t expansion) override
    {
        m_set_expansion(expansion);
    }

    uint32_t max_inner_symbols() const override
    {
        return m_max_inner_symbols();
    }

    stub::function<uint32_t()> m_max_expansion;
    stub::function<void(uint32_t)> m_set_expansion;
    stub::function<uint32_t()> m_max_inner_symbols;
};
}

TEST(api_test_fulcrum_config_interface, max_expansion)
{
    dummy d;
    d.m_max_expansion.set_return(0U, 42U);

    EXPECT_EQ(0U, kodo_fulcrum::api::max_expansion(&d));
    EXPECT_EQ(42U, kodo_fulcrum::api::max_expansion(&d));
}

TEST(api_test_fulcrum_config_interface, set_expansion)
{
    dummy d;

    kodo_fulcrum::api::set_expansion(&d, 0U);
    kodo_fulcrum::api::set_expansion(&d, 42U);

    EXPECT_TRUE(d.m_set_expansion.expect_calls()
                .with(0U)
                .with(42U)
                .to_bool());
}

TEST(api_test_fulcrum_config_interface, max_inner_symbols)
{
    dummy d;
    d.m_max_inner_symbols.set_return(0U, 42U);

    EXPECT_EQ(0U, kodo_fulcrum::api::max_inner_symbols(&d));
    EXPECT_EQ(42U, kodo_fulcrum::api::max_inner_symbols(&d));
}
