// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>

#include "fulcrum_interface.hpp"
#include <kodo_core/api/final_interface.hpp>

namespace kodo_fulcrum
{
namespace api
{
/// @copydoc layer::inner_symbols() const
///
/// @param stack The final_interface of the stack we call
///        layer::inner_symbols() on
inline uint32_t inner_symbols(kodo_core::api::final_interface* stack)
{
    auto api = dynamic_cast<fulcrum_interface*>(stack);
    assert(api);
    return api->inner_symbols();
}
}
}
