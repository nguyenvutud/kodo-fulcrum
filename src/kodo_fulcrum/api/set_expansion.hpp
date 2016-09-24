// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>

#include <kodo_core/api/final_interface.hpp>

#include "fulcrum_config_interface.hpp"

namespace kodo_fulcrum
{
namespace api
{
/// @copydoc layer::set_expansion() const
///
/// @param stack The final stack interface of the stack we call
///              layer::set_expansion() on
inline void set_expansion(kodo_core::api::final_interface* stack,
                          uint32_t expansion)
{
    auto api = dynamic_cast<fulcrum_config_interface*>(stack);

    assert(api);
    api->set_expansion(expansion);
}
}
}
