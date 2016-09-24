// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "fulcrum_config_interface.hpp"

#include <cstdint>

namespace kodo_fulcrum
{
namespace api
{
/// @ingroup interface
///
template<class Super>
struct fulcrum_config_binding : Super, fulcrum_config_interface
{
    /// @copydoc layer::max_expansion() const
    uint32_t max_expansion() const
    {
        return Super::stack()->max_expansion();
    }

    /// @copydoc layer::max_inner_symbols() const
    uint32_t max_inner_symbols() const
    {
        return Super::stack()->max_inner_symbols();
    }

    /// @copydoc layer::set_expansion(uint32_t)
    void set_expansion(uint32_t expansion)
    {
        Super::stack()->set_expansion(expansion);
    }
};
}
}
