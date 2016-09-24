// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "fulcrum_interface.hpp"

#include <cstdint>

namespace kodo_fulcrum
{
namespace api
{
/// @ingroup interface
///
template<class Super>
struct fulcrum_binding : Super, fulcrum_interface
{
    /// @copydoc layer::expansion() const
    uint32_t expansion() const
    {
        return Super::stack()->expansion();
    }

    /// @copydoc layer::inner_symbols() const
    uint32_t inner_symbols() const
    {
        return Super::stack()->inner_symbols();
    }
};
}
}
