// Copyright Steinwurf ApS 2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo_core/nested_stack.hpp>

#include "fulcrum_nested_stack_info.hpp"

namespace kodo_fulcrum
{
/// @ingroup utility
///
/// @brief Creates a nested stack representing the inner code.
///
/// In fulcrum the we only want to expose the outer code to the
/// user of either an encoder or decoder. By using the
/// fulcrum_nested_stack_info layer we can ensure that the nested
/// stack is initialized with the correct number of symbols
/// (i.e. respecting the extension selected).
///
/// @tparam NestedStack @copydoc layer_types::NestedStack
///
/// @tparam SuperCoder @copydoc layer_types::SuperCoder
///
template
<
    class NestedStack,
    class SuperCoder
>
using fulcrum_nested_stack =
    kodo_core::nested_stack<NestedStack,
    fulcrum_nested_stack_info<SuperCoder>>;
}
