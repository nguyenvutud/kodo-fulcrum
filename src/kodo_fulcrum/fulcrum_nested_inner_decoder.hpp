// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>

namespace kodo_fulcrum
{
/// @ingroup decoder_layers
///
/// @brief Adjusts the information seen by the "user" when working with
///        the fulcrum inner decoder.
///
/// @tparam SuperCoder @copydoc layer_types::SuperCoder
///
template<class SuperCoder>
class fulcrum_nested_inner_decoder : public SuperCoder
{
public:

    /// The fulcrum inner decoder only works in the inner code. This means
    /// that the "user" portion of the data can be decoded before decoding
    /// the entire code. In this layer we try to detect this as early as
    /// possible.
    ///
    /// A good example of when early decoding is possible is when we use
    /// systematic codes on an error free channel. In that case all we will
    /// be able to decoder after receiving only the user portion of the
    /// data.
    //
    /// @copydoc layer::is_complete() const
    bool is_complete() const
    {
        const auto& nested = SuperCoder::nested();
        assert(nested);

        if (nested->is_complete())
            return true;

        uint32_t symbols = SuperCoder::symbols();
        if (symbols <= nested->symbols_uncoded())
        {
            // Check the user data part of the decoding matrix. The
            // fulcrum expansion is added to the end. So the first
            // symbols excluding the expansion are uncoded we are done.
            for (uint32_t i = 0; i < symbols; ++i)
            {
                if (!nested->is_symbol_uncoded(i))
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    /// This function "adjusts" the rank seen by the user. This is
    /// needed since the inner decoder in reality has to decoder
    /// symbols + expansion but from the users perspective the rank is
    /// expected to go from 0 to symbols.
    ///
    /// In this layer we return rank 0 as long as the rank has not
    /// increased above the expansion size. This means that symbols
    /// contributing to decoding the expansion might be interpreted as
    /// linear dependent (which is in some sense is both true and false).
    ///
    /// In any case the main purpose of this layer is to have the rank
    /// increase as an user would logically expect namely from 0 to symbols
    /// (and when rank == symbols decoding should be complete).
    ///
    /// @copydoc layer::rank() const
    uint32_t rank() const
    {
        const auto& nested = SuperCoder::nested();
        assert(nested);

        uint32_t rank = nested->rank();
        uint32_t expansion = SuperCoder::expansion();

        if (rank < expansion)
        {
            return 0;
        }
        else
        {
            return rank - expansion;
        }
    }
};
}
