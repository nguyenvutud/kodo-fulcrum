// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <type_traits>

#include <fifi/prime2325.hpp>

#include <kodo/coefficient_storage_layers.hpp>
#include <kodo/common_decoder_layers.hpp>
#include <kodo/final_layer.hpp>
#include <kodo/find_enable_trace.hpp>
#include <kodo/finite_field_layers.hpp>
#include <kodo/nested_payload_size.hpp>
#include <kodo/nested_read_payload.hpp>
#include <kodo/pool_factory.hpp>
#include <kodo/proxy_args.hpp>
#include <kodo/select_storage_type.hpp>
#include <kodo/systematic_coefficient_mapper.hpp>
#include <kodo/trace_layer.hpp>
#include <kodo/uniform_generator.hpp>

#include "fulcrum_info.hpp"
#include "fulcrum_payload_decoder.hpp"
#include "fulcrum_proxy_stack.hpp"
#include "fulcrum_outer_symbol_mapper.hpp"


namespace kodo
{
namespace fulcrum
{
    /// @ingroup fec_stacks
    ///
    /// @brief The fulcrum outer decoder maps all incoming symbols
    ///        before starting the decoding process.
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
    class fulcrum_outer_decoder : public
        // Payload API
        nested_read_payload<
        nested_payload_size<
        fulcrum_proxy_stack<proxy_args<>, fulcrum_payload_decoder,
        // Decoder API
        fulcrum_outer_symbol_mapper<
        systematic_coefficient_mapper<
        // Coefficient Generator API
        uniform_generator<
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
        >>>>>>>>>>>>
    {
    public:

        static_assert(!std::is_same<Field,fifi::prime2325>::value,
                      "The mapping between inner and outer code requires "
                      "that both are binary extension fields");

        // Define the nested factory type
        using factory = pool_factory<fulcrum_outer_decoder>;
    };
}
}
