// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cassert>
#include <string>

#include <boost/program_options.hpp>

#include <kodo_core_unit_test/test_profile.hpp>

static test_profile current_profile = test_profile::desktop;

test_profile current_test_profile()
{
    return current_profile;
}

void parse_test_arguments(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::options_description desc("Test options");
    desc.add_options()
    ("profile", po::value<std::string>(), "test profile");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("profile"))
    {
        std::string profile = vm["profile"].as<std::string>();

        if (profile == "desktop")
        {
            current_profile = test_profile::desktop;
        }
        else if (profile == "embedded")
        {
            current_profile = test_profile::embedded;
        }
        else
        {
            assert(false && "Invalid test profile");
        }
    }
}
