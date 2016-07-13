// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <vector>

#include <storage/storage.hpp>

#include <kodo_fulcrum/fulcrum_codes.hpp>

/// @example encode_decode_fulcrum.cpp
///
/// Simple example showing how to encode and decode a block of data using
/// the fulcrum codec.
/// For a detailed description of the fulcrum codec see the following paper
/// on arxiv: http://arxiv.org/abs/1404.6620 by Lucani et. al.

int main()
{
    // Seed the random number generator to get different random data
    srand(static_cast<uint32_t>(time(0)));

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = 42;
    uint32_t symbol_size = 160;

    // Define the fulcrum encoder/decoder types that we will use
    using encoder_type = kodo_fulcrum::fulcrum_encoder<fifi::binary8>;
    using decoder_type = kodo_fulcrum::fulcrum_combined_decoder<fifi::binary8>;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    encoder_type::factory encoder_factory(symbols, symbol_size);

    // We query the maximum number of expansion symbols for the fulcrum factory
    std::cout << "Max expansion of the encoder factory: "
              << encoder_factory.max_expansion() << std::endl;

    // Before building the encoder, you can change the number of expansion
    // symbols like this:
    //
    // encoder_factory.set_expansion(2);

    auto encoder = encoder_factory.build();

    // Get the number of expansion symbols on the fulcrum encoder
    std::cout << "Expansion symbols on the fulcrum encoder : "
              << encoder->expansion() << std::endl;

    decoder_type::factory decoder_factory(symbols, symbol_size);

    // We query the maximum number of expansion symbols for the fulcrum factory
    std::cout << "Max expansion of the decoder factory: "
              << decoder_factory.max_expansion() << std::endl;

    // Before building the decoder, you can change the number of expansion
    // symbols like this:
    //
    // decoder_factory.set_expansion(2);

    auto decoder = decoder_factory.build();

    // Get the number of expansion symbols on the fulcrum decoder
    std::cout << "Expansion symbols on the fulcrum decoder : "
              << decoder->expansion() << std::endl;

    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> payload(encoder->payload_size());

    // Allocate some data to encode. In this case we make a buffer
    // with the same size as the encoder's block size (the max.
    // amount a single encoder can encode)
    std::vector<uint8_t> data_in(encoder->block_size());

    // Just for fun - fill the data with random data
    std::generate(data_in.begin(), data_in.end(), rand);

    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols
    encoder->set_const_symbols(storage::storage(data_in));

    // Generate packets until the decoder is complete
    while (!decoder->is_complete())
    {
        // Encode a packet into the payload buffer
        encoder->write_payload(payload.data());

        // Pass that packet to the decoder
        decoder->read_payload(payload.data());
    }

    // The decoder is complete, now copy the symbols from the decoder
    std::vector<uint8_t> data_out(decoder->block_size());
    decoder->copy_from_symbols(storage::storage(data_out));

    // Check if we properly decoded the data
    if (std::equal(data_out.begin(), data_out.end(), data_in.begin()))
    {
        std::cout << "Data decoded correctly" << std::endl;
    }
    else
    {
        std::cout << "Unexpected failure to decode, "
                  << "please file a bug report :)" << std::endl;
    }
}
