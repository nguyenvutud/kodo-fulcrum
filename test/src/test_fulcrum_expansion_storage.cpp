// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/fulcrum_expansion_storage.hpp>

#include <cstdint>

#include <gtest/gtest.h>
#include <stub/function.hpp>

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
    stub::function<void(uint32_t,const storage::mutable_storage&)>
    set_mutable_symbol;
    stub::function<uint32_t()> symbol_size;
    stub::function<uint32_t()> symbols;
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
    stub::function<uint32_t()> inner_symbols;
    stub::function<uint32_t()> symbol_size;
    stub::function<uint32_t()> symbols;
    stub::function<uint8_t* (uint32_t)> mutable_symbol;
    stub::function<uint32_t()> expansion;
};

// Dummy factory used to initialize the stack
class dummy_factory
{
public:

    // Stubs for the member functions
    stub::function<uint32_t()> max_expansion;
    stub::function<uint32_t()> max_symbol_size;
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

    // Now lets check
    EXPECT_TRUE(stack.m_nested.set_mutable_symbol.expect_calls()
                .with(0U, storage::storage((uint8_t*)0xdeadbeef,100U))
                .with(1U, storage::storage((uint8_t*)0xdeadbeef,100U))
                .with(2U, storage::storage((uint8_t*)0xdeadbeef,100U))
                .with(3U, storage::storage((uint8_t*)0xdeadbeef,100U))
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

    // Comparison function which checks the size of the storage object
    // and that the data pointer is not null
    auto compare = [](storage::const_storage actual) -> bool
    {
        return actual.m_data != nullptr && actual.m_size == 100U;
    };

    // Now lets check (using the compare lambda)
    EXPECT_TRUE(stack.m_nested.set_mutable_symbol.expect_calls()
                .with(0U, storage::storage((uint8_t*)0xdeadbeef, 100U))
                .with(1U, storage::storage((uint8_t*)0xdeadbeef, 100U))
                .with(2U, storage::storage((uint8_t*)0xdeadbeef, 100U))
                .with(3U, storage::storage((uint8_t*)0xdeadbeef, 100U))
                .with(4U, stub::make_compare(compare))
                .with(5U, stub::make_compare(compare))
                .to_bool());
}
