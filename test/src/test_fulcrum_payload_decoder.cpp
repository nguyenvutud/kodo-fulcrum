// Copyright Steinwurf ApS 2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/fulcrum_payload_decoder.hpp>
#include <kodo_fulcrum/fulcrum_encoder.hpp>

#include <gtest/gtest.h>
#include <stub/call.hpp>

namespace kodo
{
    // Put dummy layers and tests classes in an anonymous namespace
    // to avoid violations of ODF (one-definition-rule) in other
    // translation units
    namespace
    {
        class dummy_main_stack
        {
        public:

            using field_type = fifi::binary8;
            using value_type = field_type::value_type;

            class config
            {
            public:
            };

            stub::call<void(uint8_t*,uint32_t)> read_uncoded_symbol;
            stub::call<void(uint8_t*,uint8_t*)> read_symbol;
        };
    }
}

/// Test that the fulcrum payload decoder can be instantiated and that it
/// can read symbols from the fulcrum_encoder. Typically we try to isolate
/// an unit test, but in this case we take a short cut and introduce a
/// dependency.
TEST(test_fulcrum_payload_decoder, api)
{
    using encoder_stack = kodo::fulcrum::fulcrum_encoder<fifi::binary8>;

    using payload_stack = kodo::fulcrum::fulcrum_payload_decoder<
        kodo::dummy_main_stack>;

    // The main stack where the call to read_symbol(..) will go
    kodo::dummy_main_stack::config main_factory;
    kodo::dummy_main_stack main_stack;

    uint32_t expansion = 2U;
    uint32_t symbols = 16;
    uint32_t symbol_size = 10;

    encoder_stack::factory encoder_factory(symbols, symbol_size);
    encoder_factory.set_expansion(expansion);
    auto encoder = encoder_factory.build();

    // The payload stack under test
    payload_stack::factory payload_factory(
        symbols + encoder_factory.max_expansion(), symbol_size);

    payload_factory.set_main_factory(&main_factory);
    payload_factory.set_main_stack(&main_stack);
    payload_factory.set_symbols(symbols + expansion);

    auto decoder = payload_factory.build();

    EXPECT_EQ(payload_factory.max_payload_size(),
              encoder_factory.max_payload_size());

    EXPECT_EQ(decoder->payload_size(), encoder->payload_size());

    std::vector<uint8_t> block(encoder->block_size());
    std::vector<uint8_t> payload(encoder->payload_size());

    encoder->set_const_symbols(sak::storage(block));

    // The first packet should be systematic
    encoder->write_payload(payload.data());
    decoder->read_payload(payload.data());

    EXPECT_EQ(main_stack.read_uncoded_symbol.calls(), 1U);

    encoder->set_systematic_off();

    encoder->write_payload(payload.data());
    decoder->read_payload(payload.data());

    EXPECT_EQ(main_stack.read_symbol.calls(), 1U);
}
