// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>
#include <string>

#include <meta/not_found.hpp>

#include <kodo_core/enable_trace.hpp>
#include <kodo_core/has_set_trace_callback.hpp>
#include <kodo_core/has_set_trace_off.hpp>
#include <kodo_core/has_set_trace_stdout.hpp>
#include <kodo_core/has_set_zone_prefix.hpp>
#include <kodo_core/set_trace_callback.hpp>
#include <kodo_core/set_trace_off.hpp>
#include <kodo_core/set_trace_stdout.hpp>
#include <kodo_core/set_zone_prefix.hpp>
#include <kodo_core/trace_callback_function.hpp>

namespace kodo_fulcrum
{
/// Fall-through case for the case where TraceTag is meta::not_found
template<class TraceTag, class SuperCoder>
class trace_fulcrum_two_stage_decoder : public SuperCoder
{
    static_assert(std::is_same<TraceTag, meta::not_found>::value,
                  "Unexpected TraceTag should be meta::not_found in the "
                  "fall-through case.");
};

/// @ingroup trace_layers
///
/// @brief Trace layer which allows inspecting the state of a
///        fulcrum_two_stage_decoder.
///
/// In this layer we don't actually write any tracing information. We
/// just enable tracing on the stage one and stage two stacks. By using
/// the prefix it is possible to differentiate which information is
/// coming from which stack.
///
template<class SuperCoder>
class trace_fulcrum_two_stage_decoder<kodo_core::enable_trace, SuperCoder> :
    public SuperCoder
{
public:

    /// The stage one decoder
    using stage_one_decoder_type =
        typename SuperCoder::stage_one_decoder_type;

    /// The stage two decoder
    using stage_two_decoder_type =
        typename SuperCoder::stage_two_decoder_type;

public:

    /// Intercept the call to the set_trace_callback function which installs
    /// a tracing callback on the main stack and also install tracing on
    /// the stage one and stage two decoders.
    ///
    /// @copydoc layer::set_trace_callback(const trace_callback_function&)
    void set_trace_callback(
        const kodo_core::trace_callback_function& callback)
    {
        if (kodo_core::has_set_trace_callback<stage_one_decoder_type>::value)
        {
            kodo_core::set_trace_callback(SuperCoder::stage_one_decoder(),
                                          callback);
        }

        if (kodo_core::has_set_trace_callback<stage_two_decoder_type>::value)
        {
            kodo_core::set_trace_callback(SuperCoder::stage_two_decoder(),
                                          callback);
        }

        prefix_stage_decoders();

        SuperCoder::set_trace_callback(callback);
    }

    /// Intercept the set_trace_stdout installation call.
    ///
    /// @copydoc layer::set_trace_stdout()
    void set_trace_stdout()
    {
        if (kodo_core::has_set_trace_stdout<stage_one_decoder_type>::value)
        {
            kodo_core::set_trace_stdout(SuperCoder::stage_one_decoder());
        }

        if (kodo_core::has_set_trace_stdout<stage_two_decoder_type>::value)
        {
            kodo_core::set_trace_stdout(SuperCoder::stage_two_decoder());
        }

        prefix_stage_decoders();

        SuperCoder::set_trace_stdout();
    }

    /// Intercept the set_trace_off call.
    ///
    /// @copydoc layer::set_trace_off()
    void set_trace_off()
    {
        if (kodo_core::has_set_trace_off<stage_one_decoder_type>::value)
        {
            kodo_core::set_trace_off(SuperCoder::stage_one_decoder());
        }

        if (kodo_core::has_set_trace_off<stage_two_decoder_type>::value)
        {
            kodo_core::set_trace_off(SuperCoder::stage_two_decoder());
        }

        SuperCoder::set_trace_off();
    }

    /// If the user wants a specific zone prefix appended, we should
    /// also use it in the stage one and stage two decoders.
    ///
    /// @copydoc layer::set_zone_prefix(const std::string&)
    void set_zone_prefix(const std::string& zone_prefix)
    {
        SuperCoder::set_zone_prefix(zone_prefix);
        prefix_stage_decoders();
    }

protected:

    /// Set the prefix of the stage one and two decoders. We always
    /// prefix the zones because otherwise it would be impossible to
    /// differentiate what is coming from the stage one or two decoder.
    ///
    /// @param zone_prefix The prefix to use before the fixed strings
    void prefix_stage_decoders()
    {
        std::string zone_prefix = SuperCoder::zone_prefix();

        // If there is a zone prefix make the glue "."
        auto glue = zone_prefix.empty() ? "" : ".";

        if (kodo_core::has_set_zone_prefix<stage_one_decoder_type>::value)
        {
            kodo_core::set_zone_prefix(
                SuperCoder::stage_one_decoder(),
                zone_prefix + glue + "stage_one_decoder");
        }

        if (kodo_core::has_set_zone_prefix<stage_two_decoder_type>::value)
        {
            kodo_core::set_zone_prefix(
                SuperCoder::stage_two_decoder(),
                zone_prefix + glue + "stage_two_decoder");
        }
    }
};
}
