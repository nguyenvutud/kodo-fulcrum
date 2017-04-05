// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gauge/gauge.hpp>

#include <kodo_fulcrum/fulcrum_codes.hpp>

#include "fulcrum_throughput_benchmark.hpp"

/// Using this macro we may specify options. For specifying options
/// we use the boost program options library. So you may additional
/// details on how to do it in the manual for that library.
BENCHMARK_OPTION(throughput_options)
{
    gauge::po::options_description options;

    std::vector<uint32_t> symbols;
    symbols.push_back(16);
    symbols.push_back(32);
    symbols.push_back(64);
    symbols.push_back(128);
    symbols.push_back(256);
    symbols.push_back(512);
    symbols.push_back(1024);

    auto default_symbols =
        gauge::po::value<std::vector<uint32_t> >()->default_value(
            symbols, "")->multitoken();

    std::vector<uint32_t> symbol_size;
    symbol_size.push_back(1600);

    auto default_symbol_size =
        gauge::po::value<std::vector<uint32_t> >()->default_value(
            symbol_size, "")->multitoken();

    std::vector<uint32_t> expansion;
    expansion.push_back(1);
    expansion.push_back(2);
    expansion.push_back(3);
    expansion.push_back(4);

    auto default_expansion =
        gauge::po::value<std::vector<uint32_t> >()->default_value(
            expansion, "")->multitoken();

    std::vector<std::string> types;
    types.push_back("encoder");
    types.push_back("decoder");

    auto default_types =
        gauge::po::value<std::vector<std::string> >()->default_value(
            types, "")->multitoken();

    options.add_options()
    ("symbols", default_symbols, "Set the number of symbols");

    options.add_options()
    ("symbol_size", default_symbol_size, "Set the symbol size in bytes");

    options.add_options()
    ("expansion", default_expansion,
     "Set the expansion of the fulcrum codes");

    options.add_options()
    ("type", default_types, "Set type [encoder|decoder]");

    gauge::runner::instance().register_options(options);
}

//------------------------------------------------------------------
// Fulcrum
//------------------------------------------------------------------

using setup_fulcrum_inner_throughput =
    fulcrum_throughput_benchmark<
    kodo_fulcrum::fulcrum_encoder<fifi::binary8>, 
    kodo_fulcrum::fulcrum_inner_decoder<fifi::binary>>;

/// Macro creating a benchmark using a fixture class
BENCHMARK_F(setup_fulcrum_inner_throughput, FulcrumInner, Binary, 5);

using setup_fulcrum_outer_throughput =
    fulcrum_throughput_benchmark<
    kodo_fulcrum::fulcrum_encoder<fifi::binary8>,
    kodo_fulcrum::fulcrum_outer_decoder<fifi::binary8>>;

BENCHMARK_F(setup_fulcrum_outer_throughput, FulcrumOuter, Binary8, 5);

using setup_fulcrum_combined_throughput =
    fulcrum_throughput_benchmark<
    kodo_fulcrum::fulcrum_encoder<fifi::binary8>,
    kodo_fulcrum::fulcrum_combined_decoder<fifi::binary8>>;

BENCHMARK_F(setup_fulcrum_combined_throughput, FulcrumCombined, Binary8, 5);

int main(int argc, const char* argv[])
{
    srand(static_cast<uint32_t>(time(0)));

    gauge::runner::add_default_printers();
    gauge::runner::run_benchmarks(argc, argv);

    return 0;
}
