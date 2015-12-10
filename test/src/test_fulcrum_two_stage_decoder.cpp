// Copyright Steinwurf ApS 2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_fulcrum_two_stage_decoder.cpp Unit tests for the
/// fulcrum two stage combined decoder

#include <gtest/gtest.h>
#include <stub/call.hpp>

#include <kodo_fulcrum/fulcrum_two_stage_decoder.hpp>

#include <kodo/elimination_decoder.hpp>
#include <kodo/basic_symbol_decoder.hpp>

#include <memory>

namespace kodo
{
    // Put dummy layers and tests classes in an anonymous namespace
    // to avoid violations of ODF (one-definition-rule) in other
    // translation units
    namespace
    {
        template<class MaxExpansion>
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

                // We cannot stub this function since it called by the
                // fulcrum_two_stage_decoder in the constructor
                uint32_t max_expansion() const
                {
                    return MaxExpansion::value;
                }

                // Stub member functions
                stub::call<uint32_t()> max_symbols;
                stub::call<uint32_t()> max_symbol_size;
                stub::call<uint32_t()> max_coefficient_vector_size;
                stub::call<uint32_t()> expansion;
                stub::call<uint32_t()> symbol_size;
                stub::call<uint32_t()> symbols;
            };

        public:

            // We don't stub out this function since we have already
            // specified its value
            uint32_t max_expansion() const
            {
                return MaxExpansion::value;
            }

            /// The stubs do not support overloading
            void read_symbol(uint8_t* symbol_data, uint8_t* coefficients)
            {
                m_read_symbol(symbol_data, coefficients);
            }

            void read_uncoded_symbol(uint8_t* symbol_data, uint32_t symbol_index)
            {
                m_read_symbol_index(symbol_data, symbol_index);
            }

            // Stubs for member functions
            stub::call<void(config&)> construct;
            stub::call<void(config&)> initialize;
            stub::call<uint32_t()> expansion;
            stub::call<uint32_t()> symbols;
            stub::call<uint32_t()> symbol_size;
            stub::call<bool()> is_outer_systematic;
            stub::call<void(uint8_t*,uint8_t*)> m_read_symbol;
            stub::call<void(uint8_t*,uint32_t)> m_read_symbol_index;
            stub::call<void(const uint8_t*,uint8_t*)> map_to_outer;
        };

        // In general we try to keep the unit as independent of other
        // layers in Kodo, but in this case we would need to write a
        // lot of unit test code for the two nested decoders etc. so
        // we choose to simply reuse the two we use in the
        // fulcrum_combined_decoder.
        template<class MaxExpansion>
        class dummy_stack : public
            fulcrum::fulcrum_two_stage_decoder<
                elimination_decoder<fifi::binary>,
                basic_symbol_decoder<fifi::binary>,
            dummy_layer<MaxExpansion>>
        { };
    }
}

/// Test that the two nested decoders get the right "dimensions"
/// i.e. decode the right amount of symbols etc.
TEST(test_fulcrum_two_stage_decoder, build_nested_decoders)
{
    const uint32_t max_expansion = 4U;

    using stack_type = kodo::dummy_stack<
        std::integral_constant<uint32_t,max_expansion>>;

    // In this case we configure it so that we should have a stage one
    // decoder with 2 symbols and a stage two decoder with 12 symbols

    // Setup the factory
    stack_type::config factory(10U, 10U);

    factory.max_symbols.set_return(10U);
    factory.max_symbol_size.set_return(10U);
    factory.max_coefficient_vector_size.set_return(1U);
    factory.expansion.set_return(2U);
    factory.symbols.set_return(10U);
    factory.symbol_size.set_return(10U);

    auto stage_one = factory.build_stage_one();
    auto stage_two = factory.build_stage_two();

    EXPECT_EQ(stage_one->symbols(), 2U);
    EXPECT_EQ(stage_one->symbol_size(), 10U);

    EXPECT_EQ(stage_two->symbols(), 12U);
    EXPECT_EQ(stage_two->symbol_size(), 10U);
}

/// Test that systematic symbols get routed to the right
/// decoder. Systematic packets from the expansion should go to the
/// stage one decoder whereas systematic packets from orginal data
/// should go directly to stage two and the main decoder.
TEST(test_fulcrum_two_stage_decoder, route_systematic_symbols)
{
    using max_expansion = std::integral_constant<uint32_t, 4U>;

    using stack_type = kodo::dummy_stack<max_expansion>;

    // In this case we configure it so that we should have a stage one
    // decoder with 4 symbols and a stage two decoder with 8 symbols

    // Setup the factory
    stack_type::config factory(4U, 10U);

    factory.max_symbols.set_return(4U);
    factory.max_symbol_size.set_return(10U);
    factory.max_coefficient_vector_size.set_return(1U);
    factory.expansion.set_return(4U);
    factory.symbols.set_return(4U);
    factory.symbol_size.set_return(10U);

    // Setup the stack
    stack_type stack;
    stack.expansion.set_return(4U);
    stack.symbols.set_return(4U);
    stack.symbol_size.set_return(10U);
    stack.is_outer_systematic.set_return(true);

    // Run the test
    stack.construct(factory);
    stack.initialize(factory);

    // In order to access the stubs we have to get a reference to the layer
    // otherwise they will be hidden by the member functions with the same
    // names.
    using stubs_layer = kodo::dummy_layer<max_expansion>;
    stubs_layer& stubs = stack;

    // Create a buffer for the symbol
    std::vector<uint8_t> symbol(10U);

    // All symbols with a index 0..3 go directly to the main stack decoder
    // (and the stage two decoder). Whereas all symbols with index 4..7 are
    // from the expansion and are not forwarded directly (but go to the
    // stage one decoder).
    stack.read_uncoded_symbol(symbol.data(), 0U);
    stack.read_uncoded_symbol(symbol.data(), 4U);
    stack.read_uncoded_symbol(symbol.data(), 1U);
    stack.read_uncoded_symbol(symbol.data(), 3U);

    // We expect that all the systematic packets from the orginal part have
    // been forwarded directly to the layer::read_uncoded_symbol(uint8_t*,
    // uint32_t)
    EXPECT_TRUE(stubs.m_read_symbol_index.expect_calls()
                .with(symbol.data(), 0U)
                .with(symbol.data(), 1U)
                .with(symbol.data(), 3U)
                .to_bool());

    // We expect that the one symbol from the expansion has been forwarded
    // layer::read_symbol(uint8_t*,uint8_t*) since the rank reached 4
    EXPECT_EQ(stubs.m_read_symbol.calls(), 1U);
}

/// Test that we non systematic packets are routed to the right decoders
TEST(test_fulcrum_two_stage_decoder, route_non_systematic_symbols)
{
    using max_expansion = std::integral_constant<uint32_t, 4U>;

    using stack_type = kodo::dummy_stack<max_expansion>;

    // In this case we configure it so that we should have a stage one
    // decoder with 4 symbols and a stage two decoder with 8 symbols.
    // The main decoder will also have 4 symbols

    // Setup the factory
    stack_type::config factory(4U, 10U);

    factory.max_symbols.set_return(4U);
    factory.max_symbol_size.set_return(10U);
    factory.max_coefficient_vector_size.set_return(1U);
    factory.expansion.set_return(4U);
    factory.symbols.set_return(4U);
    factory.symbol_size.set_return(10U);

    // Setup the stack
    stack_type stack;
    stack.expansion.set_return(4U);
    stack.symbols.set_return(4U);
    stack.symbol_size.set_return(10U);
    stack.is_outer_systematic.set_return(true);

    // Run the test
    stack.construct(factory);
    stack.initialize(factory);

    // In order to access the stubs we have to get a reference to the layer
    // otherwise they will be hidden by the member functions with the same
    // names.
    using stubs_layer = kodo::dummy_layer<max_expansion>;
    stubs_layer& stubs = stack;

    // Create a buffer for the symbol and the coefficients. The inner code
    // will have 4 symbols and 4 expansion symbols so 8 symbols
    // total. Since we work in the binary field we just need 1 byte for the
    // coefficients. The symbols are 10 bytes long.
    std::vector<uint8_t> symbol(10U);
    std::vector<uint8_t> coefficients(1U);

    fifi::set_values<fifi::binary>(coefficients.data(), {1,1,1,1,0,0,0,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    fifi::set_values<fifi::binary>(coefficients.data(), {0,1,1,1,0,0,0,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    fifi::set_values<fifi::binary>(coefficients.data(), {1,1,1,1,0,0,0,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    // So far no calls should have propagated to the main decoder, they
    // should be fully absorbed in stage two decoder.
    EXPECT_EQ(stubs.m_read_symbol.calls(), 0U);

    fifi::set_values<fifi::binary>(coefficients.data(), {0,1,1,1,0,1,0,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    EXPECT_EQ(stubs.m_read_symbol.calls(), 0U);

    // Now the rank of the stage one and stage two decoders reach four and
    // packets should be propagated to the main decoder.

    fifi::set_values<fifi::binary>(coefficients.data(), {0,1,1,1,0,1,1,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    EXPECT_EQ(stubs.m_read_symbol.calls(), 4U);
}

/// Test case where the combined rank exceeds the number of
/// symbols. More just to show that it is possible.
///
/// One possible case is when the stage and stage two decoder have pivots
/// in the same location after mapping to the outer code. Lets make a very
/// simple not, and slightly unrealistic example (just to show).
///
/// We have 2 symbols and 2 expansion symbols so our inner code has 4 symbols.
///
/// In the outer code we define the following four encoding vectors:
///
///   We have two coefficients per encoding vector in binary8 (in
///   hexadecimal 00 to ff).
///
///             symbol 1: 01 00
///             symbol 2: 00 01
///   expansion symbol 1: 00 00
///   expansion symbol 2: 00 00
///
/// So the inner code will do linear combinations of these four
/// symbols. Now in this case we assume Random Linear Network Coding, so
/// coefficients are drawn at random, and unfortunately we drew two zero
/// vectors for the expansion symbols. Which means that our expansion is
/// not useful at all.
///
/// In this case we can reach a combined rank of 4 before we get a rank of
/// two in our main decoder. Because our stage one decoder can get rank two
/// but without contributing anything useful for the actual decoding. So we
/// will not decode until our stage two decoder also reaches rank two.
///
TEST(test_fulcrum_two_stage_decoder, combined_rank)
{
    using max_expansion = std::integral_constant<uint32_t, 2U>;

    using stack_type = kodo::dummy_stack<max_expansion>;

    // In this case we configure it so that we should have a stage one
    // decoder with 2 symbols and a stage two decoder with 4 symbols

    // Setup the factory
    stack_type::config factory(2U, 10U);

    factory.max_symbols.set_return(2U);
    factory.max_symbol_size.set_return(10U);
    factory.max_coefficient_vector_size.set_return(1U);
    factory.expansion.set_return(2U);
    factory.symbols.set_return(2U);
    factory.symbol_size.set_return(10U);

    // Setup the stack
    stack_type stack;
    stack.expansion.set_return(2U);
    stack.symbols.set_return(2U);
    stack.symbol_size.set_return(10U);
    stack.is_outer_systematic.set_return(true);

    // Run the test
    stack.construct(factory);
    stack.initialize(factory);

    // In order to access the stubs we have to get a reference to the
    // layer otherwise they will be hidden by the member functions
    // with the same names.
    using stubs_layer = kodo::dummy_layer<max_expansion>;
    stubs_layer& stubs = stack;

    // Create a buffer for the symbol
    std::vector<uint8_t> symbol(10U);
    std::vector<uint8_t> coefficients(1U);

    fifi::set_values<fifi::binary>(coefficients.data(), {0,0,1,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    EXPECT_EQ(stubs.m_read_symbol.calls(), 0U);

    fifi::set_values<fifi::binary>(coefficients.data(), {0,0,0,1});
    stack.read_symbol(symbol.data(), coefficients.data());

    // At this point decoding should be have two calls to the
    // layer::read_symbol(uint8_t*, uint8_t*) function.
    EXPECT_EQ(stubs.m_read_symbol.calls(), 2U);

    fifi::set_values<fifi::binary>(coefficients.data(), {1,0,0,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    EXPECT_EQ(stubs.m_read_symbol.calls(), 3U);

    fifi::set_values<fifi::binary>(coefficients.data(), {0,1,0,0});
    stack.read_symbol(symbol.data(), coefficients.data());

    EXPECT_EQ(stubs.m_read_symbol.calls(), 4U);
}
