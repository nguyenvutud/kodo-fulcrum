// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/fulcrum_expansion_storage.hpp>

#include <cstdint>

#include <gtest/gtest.h>
#include <stub/call.hpp>

#include <storage/storage.hpp>
#include <storage/is_same.hpp>


#include <kodo_core/has_deep_symbol_storage.hpp>

// Put dummy layers and tests classes in an anonymous namespace
// to avoid violations of ODF (one-definition-rule) in other
// translation units
namespace
{
    // The dummy nested class is used to represent the inner
    // code.
    class dummy_nested
    {
    public:

        // Stubs for the member functions
        stub::call<void(uint32_t,const storage::mutable_storage&)>
            set_mutable_symbol;
        stub::call<uint32_t()> symbol_size;
        stub::call<uint32_t()> symbols;
    };

    // Dummy SuperCoder layer for the fulcrum_expansion_storage layer
    class dummy_layer
    {
    public:

        using nested_stack_type = dummy_nested;

    public:

        template<class Factory>
        void construct(Factory& the_factory)
        {
            (void) the_factory;
        }

        template<class Factory>
        void initialize(Factory& the_factory)
        {
            (void) the_factory;
        }

        nested_stack_type* nested()
        {
            return &m_nested;
        }

    public:

        nested_stack_type m_nested;

    public:

        // Stubs for the member functions
        stub::call<uint32_t()> inner_symbols;
        stub::call<uint32_t()> symbol_size;
        stub::call<uint32_t()> symbols;
        stub::call<uint8_t*(uint32_t)> mutable_symbol;
        stub::call<uint32_t()> expansion;
    };

    // Dummy factory used to initialize the stack
    class dummy_factory
    {
    public:

        // Stubs for the member functions
        stub::call<uint32_t()> max_expansion;
        stub::call<uint32_t()> max_symbol_size;
    };
}

namespace kodo_core
{
    /// The fulcrum_expansion_storage layer only works with deep main
    /// storage layers, this is checked at compile time. To make sure
    /// it compiles, we have to mark our dummy_layer stack as deep
    template<>
    struct has_deep_symbol_storage<dummy_layer>
    {
        static const bool value = true;
    };
}

namespace
{
    /// The test stack
    class dummy_stack : public
        kodo_fulcrum::fulcrum_expansion_storage<dummy_layer>
    { };
}


/// Test that the nested decoder only sees the outer code symbols if
/// the expansion is zero i.e. no additional symbols are added in the
/// inner code
TEST(test_fulcrum_expansion_storage, no_expansion)
{
    dummy_factory factory;
    factory.max_expansion.set_return(4U);
    factory.max_symbol_size.set_return(100U);

    dummy_stack stack;

    stack.symbols.set_return(4U);
    stack.inner_symbols.set_return(4U);

    // Since the inner symbols and symbols are the same the expansion
    // must be zero
    stack.expansion.set_return(0U);

    // Make sure the nested stack also has the right information
    stack.m_nested.symbols.set_return(4U);
    stack.m_nested.symbol_size.set_return(100U);

    // The needed symbol information
    stack.mutable_symbol.set_return((uint8_t*) 0xdeadbeef);
    stack.symbol_size.set_return(100U);

    stack.construct(factory);
    stack.initialize(factory);

    // Lambda for custom comparisons (hard-coded for this setup)
    auto compare = [](
        const std::tuple<uint32_t,storage::mutable_storage>& actual,
        const std::tuple<uint32_t,storage::mutable_storage>& expected)
    {
        // Check the index (this should always match
        if (std::get<0>(actual) != std::get<0>(expected))
            return false;

        // storage::is_same returns true if the pointers and sizes are the same
        return storage::is_same(std::get<1>(actual), std::get<1>(expected));
    };

    // Now lets check
    EXPECT_TRUE(stack.m_nested.set_mutable_symbol.expect_calls(compare)
                .with(0U, {(uint8_t*)0xdeadbeef,100U})
                .with(1U, {(uint8_t*)0xdeadbeef,100U})
                .with(2U, {(uint8_t*)0xdeadbeef,100U})
                .with(3U, {(uint8_t*)0xdeadbeef,100U})
                .to_bool());
}

/// Test that the nested decoder gets initialized with expansion
/// storage when the expansion is non-zero
TEST(test_fulcrum_expansion_storage, with_expansion)
{
    dummy_factory factory;
    factory.max_expansion.set_return(4U);
    factory.max_symbol_size.set_return(100U);

    dummy_stack stack;

    stack.symbols.set_return(4U);
    stack.inner_symbols.set_return(6U);

    // In this case we have two more symbols in the inner code so the
    // expansion must be two
    stack.expansion.set_return(2U);

    // Make sure the nested stack also has the right information
    stack.m_nested.symbols.set_return(6U);
    stack.m_nested.symbol_size.set_return(100U);

    // The needed symbol information
    stack.mutable_symbol.set_return((uint8_t*) 0xdeadbeef);
    stack.symbol_size.set_return(100U);

    stack.construct(factory);
    stack.initialize(factory);

    // Lambda for custom comparisons (hard-coded for this setup)
    auto compare = [](
        const std::tuple<uint32_t,storage::mutable_storage>& actual,
        const std::tuple<uint32_t,storage::mutable_storage>& expected)
    {
        // Check the index (this should always match
        if (std::get<0>(actual) != std::get<0>(expected))
            return false;

        // Check the storage size this should always match
        if (std::get<1>(actual).m_size != std::get<1>(expected).m_size)
            return false;

        // Check the pointer this only matches for the first 4
        uint32_t index = std::get<0>(actual);
        if (index < 4)
        {
            if (std::get<1>(actual).m_data != std::get<1>(expected).m_data)
                return false;
        }

        return true;
    };

    // Now lets check (using the compare lambda)
    EXPECT_TRUE(stack.m_nested.set_mutable_symbol.expect_calls(compare)
                .with(0U, {(uint8_t*)0xdeadbeef, 100U})
                .with(1U, {(uint8_t*)0xdeadbeef, 100U})
                .with(2U, {(uint8_t*)0xdeadbeef, 100U})
                .with(3U, {(uint8_t*)0xdeadbeef, 100U})
                .with(4U, {(uint8_t*)0xdeadbeef, 100U})
                .with(5U, {(uint8_t*)0xdeadbeef, 100U})
                .to_bool());
}
