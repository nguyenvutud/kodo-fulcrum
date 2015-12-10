// Copyright Steinwurf ApS 2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>
#include <stub/call.hpp>

#include <kodo_fulcrum/fulcrum_outer_symbol_mapper.hpp>

namespace kodo
{
    // Put dummy layers and tests classes in an anonymous namespace
    // to avoid violations of ODF (one-definition-rule) in other
    // translation units
    namespace
    {
        class dummy_layer
        {
        public:

            class config
            {
            public:
                // Stub member function
                stub::call<uint32_t()> max_coefficient_vector_size;
            };

        public:

            // The stubs do not support overloaded functions
            void map_to_outer(const uint8_t* inner_coefficients,
                              uint8_t* outer_coefficients)
            {
                m_map_to_outer(inner_coefficients, outer_coefficients);
            }

            void map_uncoded_to_outer(uint32_t inner_symbol,
                              uint8_t* outer_coefficients)
            {
                m_map_to_outer_index(inner_symbol, outer_coefficients);
            }

            void read_symbol(uint8_t* symbol_data,
                               uint8_t* symbol_coefficients)
            {
                m_read_symbol(symbol_data, symbol_coefficients);
            }

            void read_uncoded_symbol(uint8_t* symbol_data,
                               uint32_t symbol_index)
            {
                m_read_symbol_index(symbol_data, symbol_index);
            }

            // Stub member functions
            stub::call<void(const config&)> construct;
            stub::call<void(const uint8_t*, uint8_t*)> m_map_to_outer;
            stub::call<void(uint32_t, const uint8_t*)> m_map_to_outer_index;
            stub::call<void(const uint8_t*, uint8_t*)> m_read_symbol;
            stub::call<void(const uint8_t*, uint32_t)> m_read_symbol_index;
            stub::call<uint32_t()> inner_symbols;
            stub::call<uint32_t()> symbols;
            stub::call<bool()> is_outer_systematic;
        };

        class dummy_stack : public
            fulcrum::fulcrum_outer_symbol_mapper<dummy_layer>
        { };
    }
}

/// Test that read_symbol(uint8_t*,uint8_t*) works as expected
TEST(test_fulcrum_outer_symbol_mapper, decode_symbol)
{
    using stack_type = kodo::dummy_stack;

    // Setup the factory
    stack_type::config factory;
    factory.max_coefficient_vector_size.set_return(10U);

    stack_type stack;
    stack.construct(factory);
    stack.read_symbol((uint8_t*)0xdeadbeef, (uint8_t*)0xdeadbeef);

    kodo::dummy_layer& test_layer = stack;
    EXPECT_EQ(test_layer.construct.calls(), 1U);
    EXPECT_EQ(stack.m_map_to_outer.calls(), 1U);
    EXPECT_EQ(stack.m_read_symbol.calls(), 1U);
}

/// Test that read_uncoded_symbol(uint8_t*,uint32_t) in the case of a
/// systematic packets forward directly to the SuperCoder
TEST(test_fulcrum_outer_symbol_mapper, decode_symbol_index_systematic)
{
    using stack_type = kodo::dummy_stack;

    // Setup the factory
    stack_type::config factory;
    factory.max_coefficient_vector_size.set_return(10U);

    // Setup stack
    stack_type stack;
    stack.inner_symbols.set_return(6U);
    stack.is_outer_systematic.set_return(true);
    stack.symbols.set_return(4);

    // Run code
    stack.construct(factory);

    // Send a symbol which is systematic
    stack.read_uncoded_symbol((uint8_t*)0xdeadbeef, 3U);

    EXPECT_TRUE(stack.m_read_symbol_index.expect_calls()
                .with((uint8_t*)0xdeadbeef, 3U)
                .to_bool());

    // Send a symbol from the expansion
    stack.read_uncoded_symbol((uint8_t*)0xdeadbeef, 5U);

    EXPECT_TRUE(stack.m_read_symbol_index.expect_calls()
                .with((uint8_t*)0xdeadbeef, 3U)
                .to_bool());

    EXPECT_EQ(stack.m_map_to_outer_index.calls(), 1U);
    EXPECT_EQ(stack.m_read_symbol.calls(), 1U);
}

/// Test that read_uncoded_symbol(uint8_t*,uint32_t) works for a non
/// systematic outer code
TEST(test_fulcrum_outer_symbol_mapper, decode_symbol_index_non_systematic)
{
    using stack_type = kodo::dummy_stack;

    // Setup the factory
    stack_type::config factory;
    factory.max_coefficient_vector_size.set_return(10U);

    // Setup stack
    stack_type stack;
    stack.inner_symbols.set_return(6U);
    stack.is_outer_systematic.set_return(false);
    stack.symbols.set_return(4);

    // Run code
    stack.construct(factory);

    // Send a symbol which is systematic
    stack.read_uncoded_symbol((uint8_t*)0xdeadbeef, 3U);

    EXPECT_EQ(stack.m_map_to_outer_index.calls(), 1U);
    EXPECT_EQ(stack.m_read_symbol.calls(), 1U);

    // Send a symbol from the expansion
    stack.read_uncoded_symbol((uint8_t*)0xdeadbeef, 5U);

    EXPECT_EQ(stack.m_map_to_outer_index.calls(), 2U);
    EXPECT_EQ(stack.m_read_symbol.calls(), 2U);
}
