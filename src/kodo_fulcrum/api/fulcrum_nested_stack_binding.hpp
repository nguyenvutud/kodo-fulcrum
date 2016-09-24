// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "fulcrum_nested_stack_interface.hpp"

#include <cstdint>

namespace kodo_fulcrum
{
namespace api
{
/// @ingroup interface
///
template<class Super>
struct fulcrum_nested_stack_binding : Super, fulcrum_nested_stack_interface
{
    /// @copydoc layer::nested_symbols() const
    uint32_t nested_symbols() const
    {
        return Super::stack()->nested_symbols();
    }

    /// @copydoc layer::nested_symbol_size() const
    uint32_t nested_symbol_size() const
    {
        return Super::stack()->nested_symbol_size();
    }
};
}
}
