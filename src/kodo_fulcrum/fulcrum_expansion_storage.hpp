// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo_core/has_set_mutable_symbol.hpp>
#include <kodo_core/has_deep_symbol_storage.hpp>

namespace kodo_fulcrum
{
/// @ingroup utility
///
/// @brief The expansion storage is used by the inner decoder to
///        allocate storage "on-the-side" for the expansion
///        symbols
///
/// @tparam SuperCoder Next layer in the stack
///
template<class SuperCoder>
class fulcrum_expansion_storage : public SuperCoder
{
public:

    /// The type of the nested stack
    using nested_stack_type = typename SuperCoder::nested_stack_type;

    /// Ensure that the nested stack uses shallow symbol storage
    /// otherwise this layer is not needed. If the nested stack
    /// uses deep storage we have to make sure we copy out the
    /// decoded symbols once completed.
    static_assert(
        kodo_core::has_set_mutable_symbol<nested_stack_type>::value,
        "This layer should not be used if the nested stack "
        "uses deep storage");

    /// Ensure that the main stack uses deep symbol. Since we
    /// specify the symbols for the nested stack in initialize we
    /// need to have the symbols available in the main stack, this
    /// is only the case with deep storage.
    static_assert(kodo_core::has_deep_symbol_storage<SuperCoder>::value,
                  "This layer requires that the main stack "
                  "uses deep storage");

public:

    /// @copydoc layer::construct(Factory&)
    template<class Factory>
    void construct(Factory& the_factory)
    {
        SuperCoder::construct(the_factory);

        m_expansion_symbols.resize(the_factory.max_expansion());

        for (auto& s: m_expansion_symbols)
        {
            s.resize(the_factory.max_symbol_size());
        }
    }

    /// @copydoc layer::initialize(Factory&)
    template<class Factory>
    void initialize(Factory& the_factory)
    {
        SuperCoder::initialize(the_factory);

        // Initialize the storage of the nested decoder
        const auto& nested_decoder = SuperCoder::nested();

        assert(nested_decoder);
        assert(nested_decoder->symbols() == SuperCoder::inner_symbols());
        assert(nested_decoder->symbol_size() == SuperCoder::symbol_size());

        uint32_t symbols = SuperCoder::symbols();
        uint32_t inner_symbols = SuperCoder::inner_symbols();

        assert(symbols > 0);
        assert(symbols <= inner_symbols);

        for (uint32_t i = 0; i < symbols; ++i)
        {
            auto symbol_storage = storage::storage(
                SuperCoder::mutable_symbol(i), SuperCoder::symbol_size());

            nested_decoder->set_mutable_symbol(i, symbol_storage);
        }

        for (uint32_t i = 0; i < SuperCoder::expansion(); ++i)
        {
            auto& expansion_symbol = m_expansion_symbols[i];

            // The index of the symbol in the inner code
            uint32_t inner_symbol = i + SuperCoder::symbols();
            assert(inner_symbol < inner_symbols);

            auto symbol_storage = storage::storage(
                expansion_symbol.data(), SuperCoder::symbol_size());

            nested_decoder->set_mutable_symbol(
                inner_symbol, symbol_storage);
        }
    }

private:

    /// Storage for the expansion symbols
    std::vector<std::vector<uint8_t> > m_expansion_symbols;
};
}
