// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cassert>
#include <ctime>
#include <memory>

#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <gauge/console_printer.hpp>
#include <gauge/csv_printer.hpp>
#include <gauge/gauge.hpp>
#include <gauge/python_printer.hpp>

#include <tables/table.hpp>

#include <kodo_core/has_deep_symbol_storage.hpp>
#include <kodo_core/has_set_mutable_symbols.hpp>
#include <kodo_core/set_mutable_symbols.hpp>
#include <kodo_core/set_systematic_off.hpp>
#include <kodo_core/set_systematic_on.hpp>

#include <kodo_rlnc/full_vector_codes.hpp>
#include <kodo_fulcrum_sparse/fulcrum_sparse_codes.hpp>
#include <kodo_fulcrum/fulcrum_codes.hpp>

/// A test block represents an encoder and decoder pair
template<class Encoder, class Recoder, class Decoder>
struct decoding_probability_benchmark : public gauge::benchmark
{
public:

    using encoder_factory = typename Encoder::factory;
    using encoder_ptr = typename Encoder::factory::pointer;

    using decoder_factory = typename Decoder::factory;
    using decoder_ptr = typename Decoder::factory::pointer;

    using recoder_factory = typename Recoder::factory;
    using recoder_ptr = typename Recoder::factory::pointer;


    decoding_probability_benchmark()
    {
        // Seed the random generator controlling the erasures
        m_random_generator.seed((uint32_t)time(0));
    }

    void start()
    { }

    void stop()
    { }

    void store_run(tables::table& results)
    {
        assert(m_encoder->block_size() ==
               m_decoder->block_size());

        assert(m_symbols_used > 0);
        assert(m_symbols_used >= m_encoder->symbols());

        if (!results.has_column("used"))
            results.add_column("used");

        results.set_value("used", m_symbols_used);

//        if (!results.has_column("rank"))
//            results.add_column("rank");

//        results.set_value("rank", m_rank_used);

        if (!results.has_column("coded"))
            results.add_column("coded");
        results.set_value("coded", m_encoder->block_size());

        if (!results.has_column("bytes_used"))
            results.add_column("bytes_used");
        results.set_value("bytes_used", m_bytes_used);

    }

    std::string unit_text() const
    {
        return "symbols";
    }

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t> >();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t> >();

        auto erasure1 = options["erasure1"].as<std::vector<double> >();
        auto erasure2 = options["erasure2"].as<std::vector<double> >();
//        auto systematic = options["systematic"].as<std::vector<bool> >();
//        auto expansion = options["expansion"].as<std::vector<uint32_t> >();


        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(erasure1.size() > 0);
        assert(erasure2.size() > 0);

        m_max_symbols = *std::max_element(symbols.begin(),
                                          symbols.end());

        m_max_symbol_size = *std::max_element(symbol_size.begin(),
                                              symbol_size.end());

        // Make the factories fit perfectly otherwise there seems to
        // be problems with memory access i.e. when using a factory
        // with max symbols 1024 with a symbols 16
        m_decoder_factory = std::make_shared<decoder_factory>(
            m_max_symbols, m_max_symbol_size);

        m_encoder_factory = std::make_shared<encoder_factory>(
            m_max_symbols, m_max_symbol_size);

        m_recoder_factory = std::make_shared<recoder_factory>(
        	m_max_symbols, m_max_symbol_size);

        for (const auto& s : symbols)
        {
            for (const auto& p : symbol_size)
            {
                for (const auto& e1 : erasure1)
                {
                	 for (const auto& e2 : erasure2)
                	 {
                        gauge::config_set cs;
                        cs.set_value<uint32_t>("symbols", s);
                        cs.set_value<uint32_t>("symbol_size", p);
                        cs.set_value<double>("erasure1", e1);
                        cs.set_value<double>("erasure2", e2);
//                        cs.set_value<bool>("systematic", sy);
//                        cs.set_value<uint32_t>("expansion", ex);
                        add_configuration(cs);
                	 }

                }
            }
        }
    }

    void setup()
    {
        gauge::config_set cs = get_current_configuration();

        uint32_t symbols = cs.get_value<uint32_t>("symbols");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

        m_decoder_factory->set_symbols(symbols);
        m_decoder_factory->set_symbol_size(symbol_size);

        m_encoder_factory->set_symbols(symbols);
        m_encoder_factory->set_symbol_size(symbol_size);

        m_recoder_factory->set_symbols(symbols);
        m_recoder_factory->set_symbol_size(symbol_size);

        m_encoder = m_encoder_factory->build();
        m_decoder = m_decoder_factory->build();
        m_recoder = m_recoder_factory->build();

        // Prepare the data to be encoded
        m_data_in.resize(m_encoder->block_size());
        m_data_out.resize(m_encoder->block_size());

        for (uint8_t& e : m_data_in)
        {
            e = rand() % 256;
        }

        m_symbols_used = 0;
        m_rank_used.resize(symbols);
        std::fill(m_rank_used.begin(), m_rank_used.end(), 0);

        //Loi thu 2 ve gia tri systematic
//        m_systematic = cs.get_value<bool>("systematic");

        double erasure1 = cs.get_value<double>("erasure1");
        m_distribution1 = boost::random::bernoulli_distribution<>(erasure1);

        double erasure2 = cs.get_value<double>("erasure2");
        m_distribution2 = boost::random::bernoulli_distribution<>(erasure2);

        m_bytes_used = 0;
    }

    /// Run the benchmark
    void run_benchmark()
    {
        assert(m_symbols_used == 0);
        assert(m_encoder);
        assert(m_decoder);
        assert(m_recoder);

        std::vector<uint8_t> payload(m_encoder->payload_size());

        // Ensure the encoding vectors are randomized between runs
        m_encoder->set_seed(rand());

        // The clock is running
        RUN
        {
            // The encoder and decoder should be in a "clean" state
            m_encoder->initialize(*m_encoder_factory);
            m_decoder->initialize(*m_decoder_factory);
            m_recoder->initialize(*m_recoder_factory);

            m_encoder->set_const_symbols(storage::storage(m_data_in));

            // If the decoder uses shallow storage, we have to initialize
            // its decoding buffer
            if (kodo_core::has_set_mutable_symbols<Decoder>::value)
            {
                kodo_core::set_mutable_symbols(
                    *m_decoder, storage::storage(m_data_out));
            }

            // We switch any systematic operations off so we code
            // symbols from the beginning
//            if (kodo_core::has_set_systematic_off<Encoder>::value &&
//                kodo_core::has_set_systematic_on<Encoder>::value)
//            {
//                if (m_systematic)
//                {
//                    kodo_core::set_systematic_on(*m_encoder);
//                }
//                else
//                {
//                    kodo_core::set_systematic_off(*m_encoder);
//                }
//            }

            kodo_core::set_systematic_off(*m_encoder);

            while (!m_decoder->is_complete())
            {
                m_bytes_used += m_encoder->write_payload(payload.data());

                if (m_distribution1(m_random_generator))
                	continue;

                m_recoder->read_payload(payload.data());
                m_recoder->nested()->write_payload(payload.data());

                if (m_distribution2(m_random_generator))
                    continue;

                ++m_symbols_used;
                ++m_rank_used[m_decoder->rank()];
                m_decoder->read_payload(payload.data());
            }
        }
    }

protected:

    /// The maximum number of symbols
    uint32_t m_max_symbols;

    /// The maximum symbol size
    uint32_t m_max_symbol_size;

    /// The decoder factory
    std::shared_ptr<decoder_factory> m_decoder_factory;

    /// The encoder factory
    std::shared_ptr<encoder_factory> m_encoder_factory;

    ///The recoder factory
    std::shared_ptr<recoder_factory> m_recoder_factory;

    /// The encoder to use
    encoder_ptr m_encoder;

    /// The decoder to use
    decoder_ptr m_decoder;

    recoder_ptr m_recoder;

    ///The recoder to use

    /// The input data
    std::vector<uint8_t> m_data_in;

    /// The output data
    std::vector<uint8_t> m_data_out;

    /// Systematic flag for the encoder
    bool m_systematic;

    /// The number of symbols used to decode
    uint32_t m_symbols_used;

    /// The number of symbols used to decode indexed by the rank of the
    /// decoder
    std::vector<uint32_t> m_rank_used;

    // The random generator
    boost::random::mt19937 m_random_generator;

    // The distribution wrapping the random generator
    boost::random::bernoulli_distribution<> m_distribution1;

    // The distribution wrapping the random generator
    boost::random::bernoulli_distribution<> m_distribution2;

    /// The number of bytes used
    uint32_t m_bytes_used;
};

//----------------------------------------------------------
template<class Encoder, class Recoder, class Decoder>
struct fulcrum_decoding_probability_benchmark :
    public decoding_probability_benchmark<Encoder, Recoder, Decoder>
{
public:

    /// The encoder and decoder factories
    using encoder_factory = typename Encoder::factory;
    using decoder_factory = typename Decoder::factory;

    using recoder_factory = typename Recoder::factory;

    /// The type of the base benchmark
    using Super = decoding_probability_benchmark<Encoder,Recoder, Decoder>;

    /// We need to access a couple of member variables from the
    /// base benchmark to setup the benchmark correctly
    using Super::m_encoder;
    using Super::m_decoder_factory;
    using Super::m_encoder_factory;
    using Super::m_recoder_factory;
    using Super::m_max_symbols;
    using Super::m_max_symbol_size;


public:

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t>>();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t>>();
        auto erasure1 = options["erasure1"].as<std::vector<double>>();
        auto erasure2 = options["erasure2"].as<std::vector<double>>();
//        auto systematic = options["systematic"].as<std::vector<bool>>();
        auto expansion = options["expansion"].as<std::vector<uint32_t> >();

        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(erasure1.size() > 0);
        assert(erasure2.size() > 0);
        assert(expansion.size() > 0);

        m_max_symbols = *std::max_element(symbols.begin(),
                                          symbols.end());

        m_max_symbol_size = *std::max_element(symbol_size.begin(),
                                              symbol_size.end());


        // Make the factories fit perfectly otherwise there seems to
        // be problems with memory access i.e. when using a factory
        // with max symbols 1024 with a symbols 16
        m_decoder_factory = std::make_shared<decoder_factory>(
            m_max_symbols, m_max_symbol_size);

        m_encoder_factory = std::make_shared<encoder_factory>(
            m_max_symbols, m_max_symbol_size);

        //Them vao sau khi gap loi symbols<=max_symbol_size
        m_recoder_factory = std::make_shared<recoder_factory>(
            m_max_symbols, m_max_symbol_size);


        for (const auto& s : symbols)
        {
            for (const auto& p : symbol_size)
            {
                for (const auto& e1 : erasure1)
                {
                	for (const auto& e2 : erasure2)
                	{
                		for (const auto& ex: expansion)
                    	{
                            gauge::config_set cs;
                            cs.set_value<uint32_t>("symbols", s);
                            cs.set_value<uint32_t>("symbol_size", p);
                            cs.set_value<double>("erasure1", e1);
                            cs.set_value<double>("erasure2", e2);

//                            cs.set_value<bool>("systematic", sy);
                            cs.set_value<uint32_t>("expansion", ex);
                            Super::add_configuration(cs);
                        }
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
         m_recoder_factory->set_expansion(expansion);
     }
};

//----------------------------------------------------------

template<class Encoder, class Recoder, class Decoder>
struct fulcrum_sparse_decoding_probability_benchmark :
    public decoding_probability_benchmark<Encoder, Recoder, Decoder>
{
public:

    /// The encoder and decoder factories
    using encoder_factory = typename Encoder::factory;
    using decoder_factory = typename Decoder::factory;

    using recoder_factory = typename Recoder::factory;

    /// The type of the base benchmark
    using Super = decoding_probability_benchmark<Encoder,Recoder, Decoder>;

    /// We need to access a couple of member variables from the
    /// base benchmark to setup the benchmark correctly
    using Super::m_encoder;
    using Super::m_decoder_factory;
    using Super::m_encoder_factory;
    using Super::m_recoder_factory;
    using Super::m_max_symbols;
    using Super::m_max_symbol_size;


public:

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t>>();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t>>();
        auto erasure1 = options["erasure1"].as<std::vector<double>>();
        auto erasure2 = options["erasure2"].as<std::vector<double>>();
        auto density = options["density"].as<std::vector<double>>();
//        auto systematic = options["systematic"].as<std::vector<bool>>();
        auto expansion = options["expansion"].as<std::vector<uint32_t> >();

        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(erasure1.size() > 0);
        assert(erasure2.size() > 0);
        assert(density.size() > 0);
        assert(expansion.size() > 0);

        m_max_symbols = *std::max_element(symbols.begin(),
                                          symbols.end());

        m_max_symbol_size = *std::max_element(symbol_size.begin(),
                                              symbol_size.end());

        std::cout<<"m_max_symbols:" <<m_max_symbols <<"--m_max_symbol_size:" <<m_max_symbol_size <<std::endl;

        // Make the factories fit perfectly otherwise there seems to
        // be problems with memory access i.e. when using a factory
        // with max symbols 1024 with a symbols 16
        m_decoder_factory = std::make_shared<decoder_factory>(
            m_max_symbols, m_max_symbol_size);

        m_encoder_factory = std::make_shared<encoder_factory>(
            m_max_symbols, m_max_symbol_size);

        //Them vao sau khi gap loi symbols<=max_symbol_size
        m_recoder_factory = std::make_shared<recoder_factory>(
            m_max_symbols, m_max_symbol_size);


        for (const auto& s : symbols)
        {
            for (const auto& p : symbol_size)
            {
                for (const auto& e1 : erasure1)
                {
                  for (const auto& e2 : erasure2)
                	                {
                    for (const auto& d: density)
                    {
                    	for (const auto& ex: expansion)
                    	{
                            gauge::config_set cs;
                            cs.set_value<uint32_t>("symbols", s);
                            cs.set_value<uint32_t>("symbol_size", p);
                            cs.set_value<double>("erasure1", e1);
                            cs.set_value<double>("erasure2", e2);
                            cs.set_value<double>("density", d);
//                            cs.set_value<bool>("systematic", sy);
                            cs.set_value<uint32_t>("expansion", ex);
                            Super::add_configuration(cs);
                        }
                    }
                  }
                }
            }
        }
    }

    void setup()
    {
        Super::setup();

        gauge::config_set cs = Super::get_current_configuration();

        double density = cs.get_value<double>("density");

        assert(m_encoder);
        m_encoder->set_density(density);
    }

    void setup_factories()
     {
         Super::setup_factories();
         gauge::config_set cs = Super::get_current_configuration();

         uint32_t expansion = cs.get_value<uint32_t>("expansion");

         m_decoder_factory->set_expansion(expansion);
         m_encoder_factory->set_expansion(expansion);
         m_recoder_factory->set_expansion(expansion);
     }
};


/// Using this macro we may specify options. For specifying options
/// we use the boost program options library. So you may additional
/// details on how to do it in the manual for that library.
BENCHMARK_OPTION(overhead_options)
{
    gauge::po::options_description options;

    options.add_options()
    ("symbols", gauge::po::value<std::vector<uint32_t> >()->default_value(
        {16, 32, 64, 128, 256, 512, 1024}, "")->multitoken(), "Set the number of symbols");

    options.add_options()("symbol_size",
                          gauge::po::value<std::vector<uint32_t>>()->
                          default_value({1500}, "")->multitoken(),
                          "Set the symbol size in bytes");

    options.add_options()
       ("erasure1", gauge::po::value<std::vector<double> >()->default_value(
           {0.0, 0.1, 0.2}, "")->multitoken(), "Set the symbol erasure probability for the first link");

    options.add_options()
    ("erasure2", gauge::po::value<std::vector<double> >()->default_value(
        {0.0, 0.1, 0.2, 0.3, 0.4, 0.5}, "")->multitoken(), "Set the symbol erasure probability for the second link");

//    options.add_options()
//    ("systematic", gauge::po::value<std::vector<bool> >()->default_value(
//        {true, false}, "")->multitoken(), "Set the encoder systematic");


    options.add_options()
      ("expansion", gauge::po::value<std::vector<uint32_t> >()->default_value(
          {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, "")->multitoken(), "Set the expansion of the fulcrum codes");

    gauge::runner::instance().register_options(options);
}

BENCHMARK_OPTION(overhead_density_options)
{
    gauge::po::options_description options;

    auto default_density =
        gauge::po::value<std::vector<double> >()->default_value(
            {0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 }, "")->multitoken();

    options.add_options()
    ("density", default_density, "Set the density of the sparse codes");

    gauge::runner::instance().register_options(options);
}

//using setup_rlnc_overhead =
//    decoding_probability_benchmark<
//    kodo_rlnc::full_vector_encoder<fifi::binary>,
//	kodo_rlnc::full_vector_decoder<fifi::binary>,
//    kodo_rlnc::full_vector_decoder<fifi::binary>>;
//
//BENCHMARK_F_INLINE(setup_rlnc_overhead, FullRLNC, Binary, 5)
//{
//    run_benchmark();
//}

using setup_fulcrum_decoding_probability_inner =
		fulcrum_decoding_probability_benchmark<
		kodo_fulcrum::fulcrum_encoder<fifi::binary8>,
		kodo_fulcrum::fulcrum_inner_decoder<fifi::binary>,
		kodo_fulcrum::fulcrum_inner_decoder<fifi::binary>>;

BENCHMARK_F_INLINE(setup_fulcrum_decoding_probability_inner, FulcrumInner,
					Binary8, 30)
{
	run_benchmark();
}

using setup_fulcrum_decoding_probability_outer =
		fulcrum_decoding_probability_benchmark<
		kodo_fulcrum::fulcrum_encoder<fifi::binary8>,
		kodo_fulcrum::fulcrum_inner_decoder<fifi::binary>,
		kodo_fulcrum::fulcrum_outer_decoder<fifi::binary8>>;

BENCHMARK_F_INLINE(setup_fulcrum_decoding_probability_outer, FulcrumOuter,
					Binary8, 30)
{
	run_benchmark();
}

using setup_fulcrum_decoding_probability_combined =
		fulcrum_decoding_probability_benchmark<
		kodo_fulcrum::fulcrum_encoder<fifi::binary8>,
		kodo_fulcrum::fulcrum_inner_decoder<fifi::binary>,
		kodo_fulcrum::fulcrum_combined_decoder<fifi::binary8>>;

BENCHMARK_F_INLINE(setup_fulcrum_decoding_probability_combined, FulcrumCombined,
					Binary8, 30)
{
	run_benchmark();
}
//----------------Fulcrum Sparse-----------------//

using setup_fulcrum_sparse_decoding_probability8_inner =
		fulcrum_sparse_decoding_probability_benchmark<
		kodo_fulcrum::fulcrum_sparse_encoder<fifi::binary8>,
		kodo_fulcrum::fulcrum_sparse_inner_decoder<fifi::binary>,
		kodo_fulcrum::fulcrum_sparse_inner_decoder<fifi::binary>>;

BENCHMARK_F_INLINE(setup_fulcrum_sparse_decoding_probability8_inner, FulcrumSparseInner,
					Binary8, 30)
{
	run_benchmark();
}

using setup_fulcrum_sparse_decoding_probability8_outer =
		fulcrum_sparse_decoding_probability_benchmark<
		kodo_fulcrum::fulcrum_sparse_encoder<fifi::binary8>,
		kodo_fulcrum::fulcrum_sparse_inner_decoder<fifi::binary>,
		kodo_fulcrum::fulcrum_sparse_outer_decoder<fifi::binary8>>;

BENCHMARK_F_INLINE(setup_fulcrum_sparse_decoding_probability8_outer, FulcrumSparseOuter,
					Binary8, 30)
{
	run_benchmark();
}

using setup_fulcrum_sparse_decoding_probability8_combined =
		fulcrum_sparse_decoding_probability_benchmark<
		kodo_fulcrum::fulcrum_sparse_encoder<fifi::binary8>,
		kodo_fulcrum::fulcrum_sparse_inner_decoder<fifi::binary>,
		kodo_fulcrum::fulcrum_sparse_combined_decoder<fifi::binary8>>;

BENCHMARK_F_INLINE(setup_fulcrum_sparse_decoding_probability8_combined, FulcrumSparseCombined,
					Binary8, 30)
{
	run_benchmark();
}


int main(int argc, const char* argv[])
{
    srand(static_cast<uint32_t>(time(0)));

    gauge::runner::add_default_printers();
    gauge::runner::run_benchmarks(argc, argv);

    return 0;
}
