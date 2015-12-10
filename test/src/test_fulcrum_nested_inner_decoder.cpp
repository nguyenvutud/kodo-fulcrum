// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/fulcrum_nested_inner_decoder.hpp>

#include <gtest/gtest.h>

#include <stub/call.hpp>

namespace kodo
{
// Put dummy layers and tests classes in an anonymous namespace
// to avoid violations of ODF (one-definition-rule) in other
// translation units
namespace
{
    struct nested_stack
    {
        stub::call<bool()> is_complete;
        stub::call<uint32_t()> symbols_uncoded;
        stub::call<bool(uint32_t)> is_symbol_uncoded;
        stub::call<uint32_t()> rank;
    };

    struct dummy_layer
    {
        stub::call<nested_stack*()> nested;
        stub::call<uint32_t()> symbols;
        stub::call<uint32_t()> expansion;

        nested_stack m_nested_stack;
    };

    class dummy_stack
        : public fulcrum::fulcrum_nested_inner_decoder<dummy_layer>
    { };
}
}

// Test that if the nested is complete we are also complete
TEST(test_fulcrum_nested_inner_decoder, nested_is_complete)
{
    kodo::dummy_stack stack;
    stack.nested.set_return(&stack.m_nested_stack);

    stack.m_nested_stack.is_complete.set_return(true);
    EXPECT_TRUE(stack.is_complete());
}

// We are not complete, the number of uncoded symbols are less than the
// outer code symbols
TEST(test_fulcrum_nested_inner_decoder, uncoded_less_than_symbols)
{
    kodo::dummy_stack stack;
    stack.nested.set_return(&stack.m_nested_stack);

    stack.m_nested_stack.is_complete.set_return(false);
    stack.symbols.set_return(4U);
    stack.m_nested_stack.symbols_uncoded.set_return(3U);

    EXPECT_FALSE(stack.is_complete());
}

// User part of the data is decoded
TEST(test_fulcrum_nested_inner_decoder, user_decoded)
{
    kodo::dummy_stack stack;
    stack.nested.set_return(&stack.m_nested_stack);

    stack.m_nested_stack.is_complete.set_return(false);
    stack.symbols.set_return(4U);
    stack.m_nested_stack.symbols_uncoded.set_return(4U);
    stack.m_nested_stack.is_symbol_uncoded.set_return(true);

    EXPECT_TRUE(stack.is_complete());
}

// User part of the data is not decoded
TEST(test_fulcrum_nested_inner_decoder, user_not_decoded)
{
    kodo::dummy_stack stack;
    stack.nested.set_return(&stack.m_nested_stack);

    stack.m_nested_stack.is_complete.set_return(false);
    stack.symbols.set_return(4U);
    stack.m_nested_stack.symbols_uncoded.set_return(4U);
    stack.m_nested_stack.is_symbol_uncoded.set_return(
        {true, true, true, false});

    EXPECT_FALSE(stack.is_complete());
}

// Test that if rank less than expansion we return zero otherwise the
// difference
TEST(test_fulcrum_rank_inner_decoder, rank_zero)
{
    kodo::dummy_stack stack;
    stack.nested.set_return(&stack.m_nested_stack);
    stack.expansion.set_return(4U);
    stack.m_nested_stack.rank.set_return({3U, 4U, 5U, 6U});

    EXPECT_EQ(stack.rank(), 0U);
    EXPECT_EQ(stack.rank(), 0U);
    EXPECT_EQ(stack.rank(), 1U);
    EXPECT_EQ(stack.rank(), 2U);
}
