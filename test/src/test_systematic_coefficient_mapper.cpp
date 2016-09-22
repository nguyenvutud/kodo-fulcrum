// Copyright Steinwurf ApS 2016.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_systematic_coefficient_mapper.cpp Unit tests for the
/// systematic_coefficient_mapper class


#include <kodo_fulcrum/systematic_coefficient_mapper.hpp>

#include "kodo_fulcrum_unit_test/helper_coefficient_mapper.hpp"

#include <fifi/fifi_utils.hpp>

#include <gtest/gtest.h>

// Put dummy layers and tests classes in an anonymous namespace
// to avoid violations of ODF (one-definition-rule) in other
// translation units
namespace
{
    template<class Field>
    class dummy_stack : public kodo_fulcrum::systematic_coefficient_mapper<
        kodo_fulcrum::helper_coefficient_mapper<Field>>
    { };
}

TEST(test_systematic_coefficient_mapper, api)
{
    using field_type = fifi::binary8;
    using stack_type = dummy_stack<field_type>;

    uint32_t symbols = 4;
    uint32_t expansion = 2;

    stack_type::config factory;
    factory.m_max_symbols = symbols;
    factory.m_max_expansion = expansion;

    EXPECT_EQ(factory.max_inner_symbols(), symbols + expansion);

    // Both the expansion and max_expansion is 2 so we should create
    // a coefficients vector of size 2
    std::vector<std::vector<uint8_t> > coefficients(2);
    for (auto& v: coefficients)
    {
        // We are in the binary8 field and we have 4 symbols so we
        // need a vector of 4 bytes
        v.resize(4);
    }

    // We setup a deterministic set of outer expansion vectors so that
    // we can check that the computations are correct
    auto& v1 = coefficients[0];
    auto& v2 = coefficients[1];
    fifi::set_value<field_type>(&v1[0], 0, 0xff);
    fifi::set_value<field_type>(&v1[0], 1, 0xf0);
    fifi::set_value<field_type>(&v1[0], 2, 0x00);
    fifi::set_value<field_type>(&v1[0], 3, 0x0f);

    fifi::set_value<field_type>(&v2[0], 0, 0x44);
    fifi::set_value<field_type>(&v2[0], 1, 0x33);
    fifi::set_value<field_type>(&v2[0], 2, 0x02);
    fifi::set_value<field_type>(&v2[0], 3, 0x01);

    factory.m_coefficients = coefficients;

    stack_type stack;
    stack.construct(factory);
    stack.initialize(factory);

    EXPECT_EQ(stack.inner_symbols(), symbols + expansion);

    EXPECT_TRUE(stack.is_outer_systematic());

    // We should have one byte per symbol in the coefficient vector
    EXPECT_EQ(stack.coefficient_vector_size(), symbols);

    // Check we get the same vector
    std::vector<uint8_t> one(stack.coefficient_vector_size(), 'a');
    std::vector<uint8_t> two(stack.coefficient_vector_size(), 'b');

    EXPECT_FALSE(one == two);

    stack.map_uncoded_to_outer(3, &one[0]);
    stack.map_uncoded_to_outer(3, &two[0]);

    EXPECT_TRUE(one == two);

    // Since this is a systematic mapper and we requested symbol
    // number 3 we can check that we get a coefficient vector:
    // 0001 where the elements are bytes
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 0), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 1), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 2), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 3), 1U);

    // Since this is a systematic mapper and we requested symbol
    // number 0 we can check that we get a coefficient vector:
    // 1000 where the elements are bytes
    stack.map_uncoded_to_outer(0U, &one[0]);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 0), 1U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 1), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 2), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 3), 0U);

    // Check that we get the same vector using the inner coefficient vector
    // now the inner coefficient vector looks like this:
    // 000100 where each element is a bit and the last two elements represent
    // the expansion
    std::vector<uint8_t> inner(
        fifi::elements_to_size<fifi::binary>(stack.inner_symbols()), 0);

    fifi::set_value<fifi::binary>(&inner[0], 3, 1);

    stack.map_to_outer(&inner[0], &one[0]);

    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 0), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 1), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 2), 0U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 3), 1U);

    // Check if we ask for multiple elements 111100 for the inner code
    fifi::set_value<fifi::binary>(&inner[0], 0, 1);
    fifi::set_value<fifi::binary>(&inner[0], 1, 1);
    fifi::set_value<fifi::binary>(&inner[0], 2, 1);
    fifi::set_value<fifi::binary>(&inner[0], 3, 1);
    fifi::set_value<fifi::binary>(&inner[0], 4, 0);
    fifi::set_value<fifi::binary>(&inner[0], 5, 0);

    stack.map_to_outer(&inner[0], &one[0]);

    // This should correspond to the 1111 outer code vector
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 0), 1U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 1), 1U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 2), 1U);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 3), 1U);

    // Now lets try to include the extension symbol number 4 and 5
    // corresponds to the last extension vector
    stack.map_uncoded_to_outer(4, &one[0]);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 0), 0xff);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 1), 0xf0);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 2), 0x00);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 3), 0x0f);

    stack.map_uncoded_to_outer(5, &one[0]);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 0), 0x44);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 1), 0x33);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 2), 0x02);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 3), 0x01);

    // Check if we ask for multiple elements 100010 for the inner code
    fifi::set_value<fifi::binary>(&inner[0], 0, 1);
    fifi::set_value<fifi::binary>(&inner[0], 1, 0);
    fifi::set_value<fifi::binary>(&inner[0], 2, 0);
    fifi::set_value<fifi::binary>(&inner[0], 3, 0);
    fifi::set_value<fifi::binary>(&inner[0], 4, 1);
    fifi::set_value<fifi::binary>(&inner[0], 5, 0);

    stack.map_to_outer(&inner[0], &one[0]);

    // We expect the four byte to be where the first byte comes from
    // the first systematic symbol and the second byte comes from the
    // extension:
    //
    // 0000 0001 ^ 1111 1111 = 1111 1110
    // 0000 0000 ^ ffff 0000 = ffff 0000
    // 0000 0000 ^ 0000 0000 = 0000 0000
    // 0000 0000 ^ 0000 1111 = 0000 1111
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 0), 0xfe);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 1), 0xf0);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 2), 0x00);
    EXPECT_EQ(fifi::get_value<field_type>(&one[0], 3), 0x0f);
}
