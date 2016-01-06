// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cstdint>
#include <ctime>

#include <gtest/gtest.h>

#include <kodo_core_unit_test/test_profile.hpp>

GTEST_API_ int main(int argc, char** argv)
{
    srand(static_cast<uint32_t>(time(0)));

    testing::InitGoogleTest(&argc, argv);

    parse_test_arguments(argc, argv);

    return RUN_ALL_TESTS();
}
