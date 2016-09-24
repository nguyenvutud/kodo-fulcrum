// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>

#include "fulcrum_config_interface.hpp"
#include <kodo_core/api/final_interface.hpp>

namespace kodo_fulcrum
{
namespace api
{
/// @copydoc layer::max_inner_symbols() const
///
/// @param stack The final interface of the stack we call
///              layer::max_inner_symbols() on
inline uint32_t max_inner_symbols(kodo_core::api::final_interface* stack)
{
    auto api = dynamic_cast<fulcrum_config_interface*>(stack);
    assert(api);
    return api->max_inner_symbols();
}
}
}
