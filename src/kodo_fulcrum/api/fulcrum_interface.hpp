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
/// @brief The interface exposing the API available on a fulcrum stack
struct fulcrum_interface
{
    /// @copydoc layer::expansion() const
    virtual uint32_t expansion() const = 0;

    /// @copydoc layer::inner_symbols() const
    virtual uint32_t inner_symbols() const = 0;
};
}
}
