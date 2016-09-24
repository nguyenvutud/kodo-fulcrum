// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo_core/mutable_shallow_storage_layers.hpp>

#include "fulcrum_combined_decoder.hpp"

namespace kodo_fulcrum
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
using shallow_fulcrum_combined_decoder =
    fulcrum_combined_decoder<
    Field,
    meta::typelist<
    kodo_core::mutable_shallow_storage_layers>::
    extend<Features>>;
}
