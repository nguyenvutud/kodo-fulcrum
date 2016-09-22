// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/trace_systematic_coefficient_mapper.hpp>

#include <gtest/gtest.h>

#include <stub/function.hpp>

#include <kodo_core/enable_trace.hpp>
#include <fifi/binary8.hpp>
#include <fifi/fifi_utils.hpp>

// Put dummy layers and tests classes in an anonymous namespace
// to avoid violations of ODF (one-definition-rule) in other
// translation units
namespace
{
    class dummy_layer
    {
    public:

        using field_type = fifi::binary8;
        using inner_field_type = fifi::binary8;

        struct field
        {
            static field_type::value_type get_value(const uint8_t* coefficients,
                                                    uint32_t index)
            {
                assert(coefficients != nullptr);

                return fifi::get_value<field_type>(coefficients, index);
            }
        };

        using inner_field = field;

        stub::function<void(const uint8_t*,uint8_t*)> map_to_outer;
        stub::function<void(uint32_t,uint8_t*)> map_uncoded_to_outer;
        stub::function<bool()> is_trace_enabled;
        stub::function<uint32_t()> inner_symbols;
        stub::function<uint32_t()> symbols;
        stub::function<void(std::string,std::string)> write_trace;
    };

    class dummy_stack :
        public kodo_fulcrum::trace_systematic_coefficient_mapper<
        kodo_core::enable_trace, dummy_layer>
    { };
}

/// Test that no traces are written when tracing are disabled
TEST(test_trace_systematic_coefficient_mapper, test_trace_disabled)
{
    dummy_stack stack;
    stack.is_trace_enabled.set_return(false);

    stack.map_to_outer((const uint8_t*) nullptr, (uint8_t*) nullptr);
    stack.map_uncoded_to_outer(1U, (uint8_t*) nullptr);

    EXPECT_EQ(stack.write_trace.calls(), 0U);
}

/// Test that tracing works for non systematic symbols
TEST(test_trace_systematic_coefficient_mapper, test_trace_non_systematic)
{
    dummy_stack stack;
    stack.is_trace_enabled.set_return(true);

    std::vector<uint8_t> inner_coefficients = {1,2,3,4,5};
    std::vector<uint8_t> outer_coefficients = {6,7,8,9,10};

    stack.inner_symbols.set_return(5U);
    stack.symbols.set_return(5U);

    stack.map_to_outer(inner_coefficients.data(), outer_coefficients.data());

    EXPECT_TRUE(stack.write_trace.expect_calls()
                .with("systematic_coefficient_mapper",
                      "From inner symbol: 1 2 3 4 5 \n"
                      "To outer symbol: 6 7 8 9 10 \n")
                .to_bool());
}

/// Test that tracing works for non systematic symbols
TEST(test_trace_systematic_coefficient_mapper, test_trace_systematic)
{
    dummy_stack stack;
    stack.is_trace_enabled.set_return(true);

    std::vector<uint8_t> outer_coefficients = {6,7,8,9,10};

    stack.symbols.set_return(5U);

    stack.map_uncoded_to_outer(42U, outer_coefficients.data());

    EXPECT_TRUE(stack.write_trace.expect_calls()
                .with("systematic_coefficient_mapper",
                      "From inner symbol: 42\n"
                      "To outer symbol: 6 7 8 9 10 \n")
                .to_bool());
}
