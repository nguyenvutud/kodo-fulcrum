// Copyright Steinwurf ApS 2016.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

/// @file helper_coefficient_mapper.hpp layer helpful for unit-testing
/// the coefficient mapper API

#include <gtest/gtest.h>

#include <fifi/fifi_utils.hpp>

namespace kodo_fulcrum
{
    /// The helper_coefficient_mapper to be used in dummy stacks where
    /// the coefficient mapper layers need to be unit tested.
    template<class Field>
    class helper_coefficient_mapper
    {
    public:

        using field_type = Field;
        using value_type = typename field_type::value_type;

    public:

        struct field
        {
            static value_type get_value(const uint8_t* coefficients,
                uint32_t index)
            {
                assert(coefficients != nullptr);

                return fifi::get_value<Field>(coefficients, index);
            }

            static void set_value(uint8_t* coefficients, uint32_t index,
                value_type value)
            {
                assert(coefficients != nullptr);

                fifi::set_value<Field>(coefficients, index, value);
            }
        };

        struct inner_field
        {
            static fifi::binary::value_type get_value(
                const uint8_t* coefficients, uint32_t index)
            {
                assert(coefficients != nullptr);

                return fifi::get_value<fifi::binary>(coefficients, index);
            }

            static void set_value(uint8_t* coefficients, uint32_t index,
                fifi::binary::value_type value)
            {
                assert(coefficients != nullptr);

                fifi::set_value<fifi::binary>(coefficients, index, value);
            }
        };

        class config
        {
        public:

            uint32_t max_inner_symbols() const
            {
                return m_max_symbols + m_max_expansion;
            }

            uint32_t max_expansion() const
            {
                return m_max_expansion;
            }

            uint32_t max_coefficient_vector_size() const
            {
                return fifi::elements_to_size<field_type>(
                    m_max_symbols);
            }

        public:

            uint32_t m_max_symbols;
            uint32_t m_max_expansion;
            std::vector<std::vector<uint8_t> > m_coefficients;
        };

    public:

        helper_coefficient_mapper() :
            m_generated(0)
        { }

        template<class Factory>
        void construct(Factory& the_factory)
        {
            m_symbols = the_factory.m_max_symbols;
            m_expansion = the_factory.m_max_expansion;
            m_coefficients = the_factory.m_coefficients;
        }

        template<class Factory>
        void initialize(Factory& the_factory)
        {
            (void) the_factory;
        }

        void set_seed(uint32_t seed_value)
        {
            assert(seed_value < inner_symbols());
        }

        void generate(uint8_t* coefficients)
        {
            assert(coefficients);
            assert(m_coefficients.size());

            std::copy(m_coefficients[m_generated].begin(),
                      m_coefficients[m_generated].end(),
                      coefficients);

            ++m_generated;
        }

        uint32_t coefficient_vector_length() const
        {
            return fifi::elements_to_length<field_type>(m_symbols);
        }

        uint32_t coefficient_vector_size() const
        {
            return fifi::elements_to_size<field_type>(m_symbols);
        }

        uint32_t inner_symbols() const
        {
            return m_symbols + m_expansion;
        }

        uint32_t symbols() const
        {
            return m_symbols;
        }

        uint32_t expansion() const
        {
            return m_expansion;
        }

        void add(value_type* dst, const value_type* src, uint32_t length)
        {
            for (uint32_t i = 0; i < length; ++i)
            {
                dst[i] ^= src[i];
            }
        }

    public:

        // Keeps track of how many coefficients we have generated
        // this value is used as index into the m_coefficients buffer
        uint32_t m_generated;

        // Stores a set of predefined coefficient vectors
        std::vector<std::vector<uint8_t>> m_coefficients;

        // Stores the number of symbols the helper was created with
        uint32_t m_symbols;

        // Stores size of the expansion in symbols
        uint32_t m_expansion;
    };
}
