// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_fulcrum/trace_fulcrum_two_stage_decoder.hpp>

#include <kodo/trace_callback_function.hpp>

#include <gtest/gtest.h>

#include <kodo_unit_test/helper_test_trace.hpp>

// Put dummy layers and tests classes in an anonymous namespace
// to avoid violations of ODF (one-definition-rule) in other
// translation units
namespace
{
    template<class DecoderType>
    class dummy_layer : public kodo::with_trace
    {
    public:

        dummy_layer()
        {
            zone_prefix.set_return("");
        }

        using stage_one_decoder_type = DecoderType;
        using stage_two_decoder_type = DecoderType;

        DecoderType m_stage_one_decoder;
        DecoderType m_stage_two_decoder;

        DecoderType& stage_one_decoder()
        {
            return m_stage_one_decoder;
        }

        DecoderType& stage_two_decoder()
        {
            return m_stage_two_decoder;
        }
    };

    template<class DecoderType>
    class test_stack : public
        kodo::fulcrum::trace_fulcrum_two_stage_decoder<kodo::enable_trace,
        dummy_layer<DecoderType>>
    { };
}

/// Test that the trace layer works even if the stage one and two decoders
/// do not support tracing.
TEST(test_trace_fulcrum_two_stage_decoder, test_trace_disabled)
{
    using stack_type = test_stack<kodo::without_trace>;

    stack_type stack;

    // In this case we get a reference to the test layer since otherwise we
    // cannot access some of the stubs which have the same name as
    // functions in the trace_fulcrum_two_stage_decoder
    dummy_layer<kodo::without_trace>& layer = stack;

    // Invoke standard trace
    stack.set_trace_stdout();
    EXPECT_EQ(layer.set_trace_stdout.calls(), 1U);

    // Invoke trace with callback
    stack.set_trace_callback(kodo::trace_callback_function());
    EXPECT_EQ(layer.set_trace_callback.calls(), 1U);
}

/// Test that the default trace works
TEST(test_trace_fulcrum_two_stage_decoder, test_set_trace_stdout)
{
    using stack_type = test_stack<kodo::with_trace>;

    stack_type stack;

    // In this case we get a reference to the test layer since otherwise we
    // cannot access some of the stubs which have the same name as
    // functions in the trace_fulcrum_two_stage_decoder
    dummy_layer<kodo::with_trace>& layer = stack;

    // Invoke standard trace
    stack.set_trace_stdout();
    EXPECT_EQ(layer.set_trace_stdout.calls(), 1U);
    EXPECT_EQ(layer.set_trace_callback.calls(), 0U);

    // Now both the stage one and two decoders should have a call to
    // set_trace_stdout()
    EXPECT_EQ(stack.m_stage_one_decoder.set_trace_stdout.calls(), 1U);
    EXPECT_EQ(stack.m_stage_two_decoder.set_trace_stdout.calls(), 1U);
    EXPECT_EQ(stack.m_stage_one_decoder.set_trace_callback.calls(), 0U);
    EXPECT_EQ(stack.m_stage_two_decoder.set_trace_callback.calls(), 0U);
}

/// Test that the trace with callback works
TEST(test_trace_fulcrum_two_stage_decoder, test_set_trace_callback)
{
    using stack_type = test_stack<kodo::with_trace>;

    stack_type stack;

    // In this case we get a reference to the test layer since otherwise we
    // cannot access some of the stubs which have the same name as
    // functions in the trace_fulcrum_two_stage_decoder
    dummy_layer<kodo::with_trace>& layer = stack;

    // Invoke standard trace
    stack.set_trace_callback(kodo::trace_callback_function());
    EXPECT_EQ(layer.set_trace_stdout.calls(), 0U);
    EXPECT_EQ(layer.set_trace_callback.calls(), 1U);

    // Now both the stage one and two decoders should have a call to
    // set_trace_callback()
    EXPECT_EQ(stack.m_stage_one_decoder.set_trace_stdout.calls(), 0U);
    EXPECT_EQ(stack.m_stage_two_decoder.set_trace_stdout.calls(), 0U);
    EXPECT_EQ(stack.m_stage_one_decoder.set_trace_callback.calls(), 1U);
    EXPECT_EQ(stack.m_stage_two_decoder.set_trace_callback.calls(), 1U);
}

/// Test that setting the tracing off works
TEST(test_trace_fulcrum_two_stage_decoder, test_set_trace_off)
{
    using stack_type = test_stack<kodo::with_trace>;

    stack_type stack;

    // In this case we get a reference to the test layer since otherwise we
    // cannot access some of the stubs which have the same name as
    // functions in the trace_fulcrum_two_stage_decoder
    dummy_layer<kodo::with_trace>& layer = stack;

    // Set trace off on the main stack
    stack.set_trace_off();
    EXPECT_EQ(layer.set_trace_off.calls(), 1U);

    // Now both of the two recoders should have a call to set_trace_off()
    EXPECT_EQ(stack.m_stage_one_decoder.set_trace_off.calls(), 1U);
    EXPECT_EQ(stack.m_stage_two_decoder.set_trace_off.calls(), 1U);
}

/// Test that the trace layer works and uses the correct zone prefixes
TEST(test_trace_fulcrum_two_stage_decoder, test_set_zone_prefix)
{
    using stack_type = test_stack<kodo::with_trace>;

    stack_type stack;

    // In this case we get a reference to the test layer since otherwise we
    // cannot access some of the stubs which have the same name as
    // functions in the trace_fulcrum_two_stage_decoder
    dummy_layer<kodo::with_trace>& layer = stack;

    stack.set_trace_stdout();

    EXPECT_EQ(layer.set_zone_prefix.calls(), 0U);
    EXPECT_TRUE(layer.m_stage_one_decoder.set_zone_prefix.expect_calls()
                .with("stage_one_decoder")
                .to_bool());
    EXPECT_TRUE(layer.m_stage_two_decoder.set_zone_prefix.expect_calls()
                .with("stage_two_decoder")
                .to_bool());

    std::string zone_prefix = "whoot";
    stack.zone_prefix.set_return(zone_prefix);
    stack.set_zone_prefix(zone_prefix);

    EXPECT_TRUE(layer.set_zone_prefix.expect_calls()
                .with("whoot")
                .to_bool());

    EXPECT_TRUE(layer.m_stage_one_decoder.set_zone_prefix.expect_calls()
                .with("stage_one_decoder")
                .with("whoot.stage_one_decoder")
                .to_bool());

    EXPECT_TRUE(layer.m_stage_two_decoder.set_zone_prefix.expect_calls()
                .with("stage_two_decoder")
                .with("whoot.stage_two_decoder")
                .to_bool());
}

/// Test that the trace layer works and uses the correct zone prefixes when
/// the set_zone_prefix is called before trace.
TEST(test_trace_fulcrum_two_stage_decoder, test_set_zone_prefix_first)
{
    using stack_type = test_stack<kodo::with_trace>;

    stack_type stack;

    // In this case we get a reference to the test layer since otherwise we
    // cannot access some of the stubs which have the same name as
    // functions in the trace_pure_recoder
    dummy_layer<kodo::with_trace>& layer = stack;

    std::string zone_prefix = "whoot";

    // Set zone prefix before calling trace
    stack.zone_prefix.set_return(zone_prefix);
    stack.set_zone_prefix(zone_prefix);

    stack.set_trace_stdout();

    EXPECT_TRUE(layer.set_zone_prefix.expect_calls()
                .with("whoot")
                .to_bool());

    EXPECT_TRUE(layer.m_stage_one_decoder.set_zone_prefix.expect_calls()
                .with("whoot.stage_one_decoder")
                .with("whoot.stage_one_decoder")
                .to_bool());

    EXPECT_TRUE(layer.m_stage_two_decoder.set_zone_prefix.expect_calls()
                .with("whoot.stage_two_decoder")
                .with("whoot.stage_two_decoder")
                .to_bool());
}
