// Copyright Steinwurf ApS 2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo/common_encoder_layers.hpp>
#include <kodo/nested_payload_size.hpp>
#include <kodo/nested_systematic.hpp>
#include <kodo/nested_write_payload.hpp>
#include <kodo/payload_precoder.hpp>
#include <kodo/select_storage_type.hpp>
#include <kodo/systematic_coefficient_mapper.hpp>
#include <kodo/systematic_precoder.hpp>
#include <kodo/trace_layer.hpp>
#include <kodo/trace_nested_stack.hpp>
#include <kodo/trace_systematic_coefficient_mapper.hpp>
#include <kodo/uniform_generator_layers.hpp>

#include <kodo_rlnc/shallow_full_vector_encoder.hpp>

#include "fulcrum_nested_stack.hpp"
#include "fulcrum_info.hpp"

namespace kodo
{
namespace fulcrum
{
    /// @ingroup fec_stacks
    ///
    /// @brief The fulcrum encoder supports the concatenated code
    ///        structure with an "outer" and "inner" code.
    ///
    /// For a detailed description of the fulcrum codec see the
    /// following paper on arxiv: http://arxiv.org/abs/1404.6620 by
    /// Lucani et. al.
    ///
    template
    <
        class Field,
        class Features = meta::typelist<>
    >
    class fulcrum_encoder : public
        // Payload Codec API
        nested_systematic<
        payload_precoder<
        systematic_precoder<
        trace_systematic_coefficient_mapper<find_enable_trace<Features>,
        systematic_coefficient_mapper<
        nested_write_payload<
        nested_payload_size<
        trace_nested_stack<find_enable_trace<Features>,
        fulcrum_nested_stack<
            rlnc::shallow_full_vector_encoder<fifi::binary, Features>,
        // Coefficient Generator API
        uniform_generator_layers<
        // Codec API
        common_encoder_layers<Features,
        // Coefficient Storage API
        coefficient_value_access<
        coefficient_info<
        // Symbol Storage API
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
        >>>>>>>>>>>>>>>>>
    {
    public:
        using factory = pool_factory<fulcrum_encoder>;
    };
}
}
