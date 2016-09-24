// Copyright Steinwurf ApS 2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo_core/proxy_stack.hpp>

#include "fulcrum_nested_stack.hpp"

namespace kodo_fulcrum
{
/// @ingroup utility
///
/// @brief Proxy stack for fulcrum codes.
///
/// The proxy stack and nested stacks allow us to "embed" a stack
/// into an existing stack this is useful in fulcrum since we
/// there have two codecs an inner and an outer. In this
/// implementation the nested stack will represent the inner
/// codec.
///
/// The difference between a proxy and nested stack is that the
/// proxy stack has a back pointer to the containing stack. By
/// adding the proxy_layer.hpp layer any function not implemented
/// in the proxy stack will get forwarded into the containing
/// stack.
///
/// @tparam Args is a proxy_args<> variadic template wrapper used
///         to contain template arguments which should be passed
///         to the nested stack.
///
/// @tparam NestedStack is the stack that will be nested.
///
/// @tparam SuperCoder @copydoc layer_types::SuperCoder
///
template
<
    class Args,
    template <class...> class NestedStack,
    class SuperCoder
>
using fulcrum_proxy_stack = kodo_core::proxy_stack<
                            fulcrum_nested_stack, Args, NestedStack, SuperCoder>;
}
