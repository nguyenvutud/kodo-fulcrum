// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo/coefficient_info.hpp>
#include <kodo/final_layer.hpp>
#include <kodo/finite_field_info.hpp>
#include <kodo/payload_info.hpp>
#include <kodo/plain_symbol_id_reader.hpp>
#include <kodo/pool_factory.hpp>
#include <kodo/proxy_layer.hpp>
#include <kodo/storage_block_info.hpp>
#include <kodo/symbol_id_decoder.hpp>
#include <kodo/systematic_decoder_layers.hpp>

namespace kodo
{
namespace fulcrum
{
    /// @ingroup utility fulcrum
    ///
    /// @brief Decodes the payload portion of the inner code in a
    ///        fulcrum decoder.
    ///
    /// When used this layer will consume layer::read_payload(uint8_t*)
    /// calls and forward them to layer::read_symbol(uint8_t*,
    /// uint8_t*) or layer::read_uncoded_symbol(uint8_t*, uint32_t) in the
    /// main stack.
    ///
    /// This is implemented as a secondary stack since the inner and outer
    /// codes do typically not use the same finite field. Having
    /// fulcrum_payload_deocder nested means that the user will only see
    /// the outer code. Which is typically what we want.
    ///
    /// @tparam MainStack The type of the "main stack" where calls not
    ///         implemented in this stack will be forwarded curtecy of
    ///         the proxy_layer.
    ///
    template<class MainStack>
    class fulcrum_payload_decoder : public
        // Payload API
        payload_info<
        // Codec Header API
        systematic_decoder_layers<
        symbol_id_decoder<
        // Symbol ID API
        plain_symbol_id_reader<
        // Coefficient Storage API
        coefficient_info<
        // Storage API
        storage_block_info<
        // Finite Field API
        finite_field_info<fifi::binary,
        // Proxy
        proxy_layer<MainStack,
        // Final layer
        final_layer
        >>>>>>>>
    {
    public:
        using factory = pool_factory<fulcrum_payload_decoder>;
    };
}
}
