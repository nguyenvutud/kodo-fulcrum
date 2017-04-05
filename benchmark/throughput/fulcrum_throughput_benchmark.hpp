// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cassert>

#include <gauge/gauge.hpp>

#include <throughput/throughput_benchmark.hpp>

/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct fulcrum_throughput_benchmark :
    public throughput_benchmark<Encoder,Decoder>
{
public:

    /// The type of the base benchmark
    using Super = throughput_benchmark<Encoder,Decoder>;

    /// We need access to the encoder built to adjust the average number of
    /// nonzero symbols
    using Super::m_encoder_factory;
    using Super::m_decoder_factory;

    using Super::m_encoder;
    using Super::m_decoder;

public:

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t>>();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t>>();
        auto types = options["type"].as<std::vector<std::string>>();
        auto expansion = options["expansion"].as<std::vector<uint32_t> >();

        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(types.size() > 0);
        assert(expansion.size() > 0);

        for (const auto& s : symbols)
        {
            for (const auto& p : symbol_size)
            {
                for (const auto& t : types)
                {
                    for (const auto& e: expansion)
                    {
                        gauge::config_set cs;
                        cs.set_value<uint32_t>("symbols", s);
                        cs.set_value<uint32_t>("symbol_size", p);
                        cs.set_value<std::string>("type", t);
                        cs.set_value<uint32_t>("expansion", e);

                        Super::add_configuration(cs);
                    }
                }
            }
        }
    }

    void setup_factories()
    {
        Super::setup_factories();
        gauge::config_set cs = Super::get_current_configuration();

        uint32_t expansion = cs.get_value<uint32_t>("expansion");

        m_decoder_factory->set_expansion(expansion);
        m_encoder_factory->set_expansion(expansion);
    }
    void setup()
    {
        Super::setup();

        gauge::config_set cs = Super::get_current_configuration();
	m_encoder->set_systematic_off();
    }
};
