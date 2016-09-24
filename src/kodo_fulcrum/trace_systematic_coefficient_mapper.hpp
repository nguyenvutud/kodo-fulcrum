// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <algorithm>
#include <sstream>

#include <meta/not_found.hpp>

#include <kodo_core/enable_trace.hpp>

namespace kodo_fulcrum
{
/// Fall-through case for the case where TraceTag is meta::not_found
template<class TraceTag, class SuperCoder>
class trace_systematic_coefficient_mapper : public SuperCoder
{
    static_assert(std::is_same<TraceTag, meta::not_found>::value,
                  "Unexpected TraceTag should be meta::not_found in the "
                  "fall-through case.");
};

/// @ingroup trace_layers
///
/// @brief This layer creates the trace mapping between the coding
///        coefficient between the inner and outer fulcrum code.
///
template<class SuperCoder>
class trace_systematic_coefficient_mapper<kodo_core::enable_trace, SuperCoder> :
    public SuperCoder
{
public:

    /// The outer finite field
    using field_type = typename SuperCoder::field_type;

    /// The value type used by the outer field
    using value_type = typename field_type::value_type;

public:

    /// @copydoc systematic_coefficient_mapper::map_to_outer(
    ///      const uint8_t*, uint8_t*)
    void map_to_outer(const uint8_t* inner_coefficients,
                      uint8_t* outer_coefficients)
    {
        SuperCoder::map_to_outer(inner_coefficients, outer_coefficients);

        if (SuperCoder::is_trace_enabled())
        {
            std::stringstream stream;
            stream << "From inner symbol: ";

            for (uint32_t i = 0; i < SuperCoder::inner_symbols(); ++i)
            {
                auto c = SuperCoder::inner_field::get_value(
                    inner_coefficients, i);

                stream << (uint32_t) c << " ";
            }

            stream << std::endl;

            stream << "To outer symbol: ";

            for (uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                auto c = SuperCoder::field::get_value(
                    outer_coefficients, i);

                stream << (uint32_t) c << " ";
            }

            stream << std::endl;

            SuperCoder::write_trace("systematic_coefficient_mapper",
                                    stream.str());
        }
    }

    /// @copydoc systematic_coefficient_mapper::map_uncoded_to_outer(
    ///      uint32_t, uint8_t*)
    void map_uncoded_to_outer(uint32_t inner_symbol,
                              uint8_t* outer_coefficients)
    {
        SuperCoder::map_uncoded_to_outer(inner_symbol, outer_coefficients);

        if (SuperCoder::is_trace_enabled())
        {
            std::stringstream stream;
            stream << "From inner symbol: " << inner_symbol << std::endl;
            stream << "To outer symbol: ";

            for (uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                auto c = SuperCoder::field::get_value(
                    outer_coefficients, i);

                stream << (uint32_t) c << " ";
            }

            stream << std::endl;

            SuperCoder::write_trace("systematic_coefficient_mapper",
                                    stream.str());
        }
    }
};
}
