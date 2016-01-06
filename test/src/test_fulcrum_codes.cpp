// Copyright Steinwurf ApS 2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_fulcrum_codes.cpp Unit tests for the Fulcrum coding scheme

#include <kodo_fulcrum/fulcrum_codes.hpp>

#include <gtest/gtest.h>

#include <kodo_core_unit_test/helper_test_basic_api.hpp>
#include <kodo_core_unit_test/helper_test_reuse_api.hpp>
#include <kodo_core_unit_test/helper_test_initialize_api.hpp>
#include <kodo_core_unit_test/helper_test_systematic_api.hpp>
#include <kodo_core_unit_test/helper_test_mix_uncoded_api.hpp>

namespace
{
    template<class Field>
    using encoder = kodo_fulcrum::fulcrum_encoder<Field>;

    template<class Field>
    using inner_decoder = kodo_fulcrum::fulcrum_inner_decoder<Field>;

    template<class Field>
    using outer_decoder = kodo_fulcrum::fulcrum_outer_decoder<Field>;

    template<class Field>
    using combined_decoder = kodo_fulcrum::fulcrum_combined_decoder<Field>;

    template<class Field>
    using shallow_combined_decoder =
        kodo_fulcrum::shallow_fulcrum_combined_decoder<Field>;
}

TEST(test_fulcrum_codes, inner)
{
    run_test_basic_api<encoder<fifi::binary4>,
        inner_decoder<fifi::binary>>();

    run_test_basic_api<encoder<fifi::binary8>,
        inner_decoder<fifi::binary>>();

    run_test_basic_api<encoder<fifi::binary16>,
        inner_decoder<fifi::binary>>();

    run_test_initialize<encoder<fifi::binary4>,
        inner_decoder<fifi::binary>>();

    run_test_initialize<encoder<fifi::binary8>,
        inner_decoder<fifi::binary>>();

    run_test_initialize<encoder<fifi::binary16>,
        inner_decoder<fifi::binary>>();

    run_test_systematic<encoder<fifi::binary4>,
        inner_decoder<fifi::binary>>();

    run_test_systematic<encoder<fifi::binary8>,
        inner_decoder<fifi::binary>>();

    run_test_systematic<encoder<fifi::binary16>,
        inner_decoder<fifi::binary>>();

    run_test_mix_uncoded<encoder<fifi::binary4>,
        inner_decoder<fifi::binary>>();

    run_test_mix_uncoded<encoder<fifi::binary8>,
        inner_decoder<fifi::binary>>();

    run_test_mix_uncoded<encoder<fifi::binary16>,
        inner_decoder<fifi::binary>>();

    run_test_reuse<encoder<fifi::binary4>,
        inner_decoder<fifi::binary>>();

    run_test_reuse<encoder<fifi::binary8>,
        inner_decoder<fifi::binary>>();

    run_test_reuse<encoder<fifi::binary16>,
        inner_decoder<fifi::binary>>();

    run_test_reuse_incomplete<encoder<fifi::binary4>,
        inner_decoder<fifi::binary>>();

    run_test_reuse_incomplete<encoder<fifi::binary8>,
        inner_decoder<fifi::binary>>();

    run_test_reuse_incomplete<encoder<fifi::binary16>,
        inner_decoder<fifi::binary>>();
}

TEST(test_fulcrum_codes, outer)
{
    // We invoke these manually since this codec does not support the
    // prime2325 field
    run_test_basic_api<encoder<fifi::binary4>,
        outer_decoder<fifi::binary4>>();

    run_test_basic_api<encoder<fifi::binary8>,
        outer_decoder<fifi::binary8>>();

    run_test_basic_api<encoder<fifi::binary16>,
        outer_decoder<fifi::binary16>>();

    test_initialize<encoder, outer_decoder>();
    test_systematic<encoder, outer_decoder>();
    test_mix_uncoded<encoder, outer_decoder>();
    test_reuse<encoder, outer_decoder>();
    test_reuse_incomplete<encoder, outer_decoder>();
}

TEST(test_fulcrum_codes, combined)
{
    // We invoke these manually since this codec does not support the
    // prime2325 field
    run_test_basic_api<encoder<fifi::binary4>,
        combined_decoder<fifi::binary4>>();

    run_test_basic_api<encoder<fifi::binary8>,
        combined_decoder<fifi::binary8>>();

    run_test_basic_api<encoder<fifi::binary16>,
        combined_decoder<fifi::binary16>>();

    test_initialize<encoder, combined_decoder>();
    test_systematic<encoder, combined_decoder>();
    test_mix_uncoded<encoder, combined_decoder>();
    test_reuse<encoder, combined_decoder>();
    test_reuse_incomplete<encoder, combined_decoder>();
}

TEST(test_fulcrum_codes, shallow_combined)
{
    // We invoke these manually since this codec does not support the
    // prime2325 field
    run_test_basic_api<encoder<fifi::binary4>,
        shallow_combined_decoder<fifi::binary4>>();

    run_test_basic_api<encoder<fifi::binary8>,
        shallow_combined_decoder<fifi::binary8>>();

    run_test_basic_api<encoder<fifi::binary16>,
        shallow_combined_decoder<fifi::binary16>>();

    test_initialize<encoder, shallow_combined_decoder>();
    test_systematic<encoder, shallow_combined_decoder>();
    test_mix_uncoded<encoder, shallow_combined_decoder>();
    test_reuse<encoder, shallow_combined_decoder>();
    test_reuse_incomplete<encoder, shallow_combined_decoder>();
}
