// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo_fulcrum
{
namespace api
{
/// @ingroup interface
///
/// @brief The interface exposing the API available on a nested fulcrum
///        stack
struct fulcrum_nested_stack_interface
{
    /// @copydoc layer::nested_symbols() const
    virtual uint32_t nested_symbols() const = 0;

    /// @copydoc layer::nested_symbol_size() const
    virtual uint32_t nested_symbol_size() const = 0;
};
}
}
