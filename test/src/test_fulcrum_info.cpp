// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>

#include <stub/call.hpp>

#include <kodo_fulcrum/fulcrum_info.hpp>

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

            config(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                (void) max_symbols;
                (void) max_symbol_size;
            }

        public:

            // Stub member functions
            stub::call<uint32_t()> symbols;
        };

    public:

        // Stub member functions
        stub::call<void(config&)> initialize;
    };

    template<class MaxExpansion, class DefaultExpansion>
    class dummy_stack : public
        kodo::fulcrum::fulcrum_info<MaxExpansion, DefaultExpansion, dummy_layer>
    {
    public:

        using factory = typename dummy_stack::config;
    };
}

/// Test that the API returns the right values
TEST(test_fulcrum_info, api)
{
    using max_expansion = std::integral_constant<uint32_t,8U>;
    using default_expansion = std::integral_constant<uint32_t,4U>;

    uint32_t symbols = 10U;
    uint32_t symbol_size = 10U;

    using stack_type = dummy_stack<max_expansion, default_expansion>;

    // Check the factory
    stack_type::factory factory(symbols, symbol_size);

    EXPECT_EQ(factory.max_expansion(), max_expansion::value);
    EXPECT_EQ(factory.expansion(), default_expansion::value);
    EXPECT_EQ(factory.max_inner_symbols(), max_expansion::value + symbols);

    factory.set_expansion(6U);

    EXPECT_EQ(factory.expansion(), 6U);
    EXPECT_EQ(factory.max_expansion(), max_expansion::value);
    EXPECT_EQ(factory.max_inner_symbols(), max_expansion::value + symbols);

    factory.symbols.set_return(10);

    // Check the stack
    stack_type stack;
    stack.initialize(factory);

    dummy_layer& test_layer = stack;
    EXPECT_EQ(test_layer.initialize.calls(), 1U);

    EXPECT_EQ(stack.max_expansion(), max_expansion::value);
    EXPECT_EQ(stack.expansion(), 6U);
    EXPECT_EQ(stack.inner_symbols(), 16U);

    //Test of the initialize function
    factory.symbols.set_return(1U);
    stack.initialize(factory);

    EXPECT_EQ(stack.max_expansion(), max_expansion::value);
    EXPECT_EQ(stack.expansion(), 6U);
    EXPECT_EQ(stack.inner_symbols(), 7U);
}
