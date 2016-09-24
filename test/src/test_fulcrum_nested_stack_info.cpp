// Copyright Steinwurf ApS 2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>

#include <kodo_fulcrum/fulcrum_nested_stack_info.hpp>

namespace kodo_fulcrum
{
// Put dummy layers and tests classes in an anonymous namespace
// to avoid violations of ODF (one-definition-rule) in other
// translation units
namespace
{
template<uint32_t MaxExpansion>
class dummy_layer
{
public:

    class config
    {
    public:

        config(uint32_t max_symbols, uint32_t max_symbol_size) :
            m_max_symbols(max_symbols),
            m_max_symbol_size(max_symbol_size)
        { }

        uint32_t max_expansion() const
        {
            return MaxExpansion;
        }

        uint32_t expansion() const
        {
            return MaxExpansion;
        }

        uint32_t symbols() const
        {
            return m_max_symbols;
        }
        uint32_t symbol_size() const
        {
            return m_max_symbol_size;
        }

        uint32_t m_max_symbols;
        uint32_t m_max_symbol_size;
    };

    template<class Factory>
    void initialize(Factory& the_factory)
    {
        (void) the_factory;
    }
};

template<uint32_t MaxExpansion>
class dummy_stack : public
    fulcrum_nested_stack_info<dummy_layer<MaxExpansion>>
{ };
}
}

TEST(test_fulcrum_nested_stack_info, api)
{
    static const uint32_t max_expansion = 4;
    uint32_t max_symbols = 10;
    uint32_t max_symbol_size = 10;

    using test_stack = kodo_fulcrum::dummy_stack<max_expansion>;

    test_stack::config factory(max_symbols, max_symbol_size);

    EXPECT_EQ(factory.max_expansion(), max_expansion);
    EXPECT_EQ(factory.symbols(), max_symbols);
    EXPECT_EQ(factory.symbol_size(), max_symbol_size);

    test_stack stack;
    stack.initialize(factory);

    EXPECT_EQ(stack.nested_symbols(), max_symbols + max_expansion);
    EXPECT_EQ(stack.nested_symbol_size(), max_symbol_size);
}
