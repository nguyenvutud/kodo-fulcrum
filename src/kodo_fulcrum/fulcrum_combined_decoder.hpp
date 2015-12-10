// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <fifi/binary.hpp>

#include <kodo/basic_symbol_decoder.hpp>
#include <kodo/coefficient_storage_layers.hpp>
#include <kodo/common_decoder_layers.hpp>
#include <kodo/elimination_decoder.hpp>
#include <kodo/final_layer.hpp>
#include <kodo/finite_field_layers.hpp>
#include <kodo/nested_payload_size.hpp>
#include <kodo/nested_read_payload.hpp>
#include <kodo/pool_factory.hpp>
#include <kodo/proxy_args.hpp>
#include <kodo/select_storage_type.hpp>
#include <kodo/systematic_coefficient_mapper.hpp>
#include <kodo/trace_layer.hpp>
#include <kodo/trace_systematic_coefficient_mapper.hpp>
#include <kodo/uniform_generator.hpp>

#include "fulcrum_info.hpp"
#include "fulcrum_payload_decoder.hpp"
#include "fulcrum_proxy_stack.hpp"
#include "fulcrum_two_stage_decoder.hpp"
#include "trace_fulcrum_two_stage_decoder.hpp"

namespace kodo
{
namespace fulcrum
{
    /// @ingroup fec_stacks
    ///
    /// @brief The fulcrum combined decoder tries to decode as much as
    ///        possible in the inner code before passing symbols to
    ///        the outer code.
    ///
    /// The main component of the combined decoder is implemented in
    /// the fulcrum_two_stage_decoder layer. Which implements decoding
    /// in the inner code, as soon as a full decoding is deemed
    /// possible symbols are taken from the fulcrum_two_stage_decoder
    /// and pass to the main decoder (which decodes in the outer
    /// code).
    ///
    /// For a detailed description of the fulcrum codec see the
    /// following paper on arxiv: http://arxiv.org/abs/1404.6620 by
    /// Lucani et. al.
    ///
    /// @tparam Field @copydoc layer_types::Field
    ///
    template
    <
        class Field,
        class Features = meta::typelist<>
    >
    class fulcrum_combined_decoder : public
        // Payload API
        nested_read_payload<
        nested_payload_size<
        fulcrum_proxy_stack<proxy_args<>, fulcrum_payload_decoder,
        // Codec Header API
        //  - implemented in outer decoder
        // Symbol ID API
        //  - implemented in outer decoder
        // Coefficient Generator API
        trace_fulcrum_two_stage_decoder<find_enable_trace<Features>,
        fulcrum_two_stage_decoder<
            elimination_decoder<fifi::binary,
                typename Features::template remove<is_storage_type>>,
            basic_symbol_decoder<fifi::binary,
                typename Features::template remove<is_storage_type>>,
        trace_systematic_coefficient_mapper<find_enable_trace<Features>,
        systematic_coefficient_mapper<
        uniform_generator<
        // Decoder API
        common_decoder_layers<Features,
        // Coefficient Storage API
        coefficient_storage_layers<
        // Storage API
        select_storage_type<Features,
        // Finite Field API
        finite_field_layers<Field,
        // Fulcrum API
        fulcrum_info<
            std::integral_constant<uint32_t,10>, // MaxExpansion
            std::integral_constant<uint32_t,4>,  // DefaultExpansion
        // Trace Layer
        trace_layer<find_enable_trace<Features>,
        // Final Layer
        final_layer
        >>>>>>>>>>>>>>
    {
    public:

        static_assert(!std::is_same<Field,fifi::prime2325>::value,
                      "The mapping between inner and outer code requires "
                      "that both are binary extension fields");

        using factory = pool_factory<fulcrum_combined_decoder>;
    };
}
}
