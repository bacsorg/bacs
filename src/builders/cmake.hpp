#pragma once

#include "conf_make_install.hpp"

#include <bunsan/utility/maker.hpp>

#include <bunsan/stream_enum.hpp>

#include <unordered_map>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace utility{namespace builders
{
    class cmake: public conf_make_install
    {
    public:
        struct config
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(cmake);
                ar & BOOST_SERIALIZATION_NVP(make_maker);
                ar & BOOST_SERIALIZATION_NVP(install_maker);
            }

            struct
            {
                template <typename Archive>
                void serialize(Archive &ar, const unsigned int)
                {
                    ar & BOOST_SERIALIZATION_NVP(defines);
                    ar & BOOST_SERIALIZATION_NVP(generator);
                }

                std::unordered_map<std::string, std::string> defines; ///< for example {"CMAKE_INSTALL_PREFIX": "/usr"}
                boost::optional<std::string> generator; ///< for example "Unix Makefiles"
            } cmake;

            boost::property_tree::ptree make_maker, install_maker;
        };

    public:
        explicit cmake(const resolver &resolver_);
        void setup(const boost::property_tree::ptree &ptree) override;

    protected:
        void configure_(
            const boost::filesystem::path &src,
            const boost::filesystem::path &bin) override;

        void make_(
            const boost::filesystem::path &src,
            const boost::filesystem::path &bin) override;

        void install_(
            const boost::filesystem::path &src,
            const boost::filesystem::path &bin,
            const boost::filesystem::path &root) override;

    public:
        BUNSAN_INCLASS_STREAM_ENUM_CLASS(generator_type,
        (
            MAKEFILE,
            NMAKEFILE,
            VISUAL_STUDIO,
            UNKNOWN
        ))

    private:
        struct generator
        {
            const std::string name;
            const generator_type type;
        };

    private:
        void set_default_generator();
        void set_generator(const std::string &generator_name);
        const generator &get_generator() const;
        std::vector<std::string> arguments_(
            const boost::filesystem::path &src) const;

    private:
        const resolver m_resolver;
        const boost::filesystem::path m_cmake_exe;
        boost::optional<std::size_t> m_generator;
        config m_config;

    private:
        static const std::vector<generator> generators;
        static const bool factory_reg_hook;
    };

    struct cmake_error: virtual error {};
    struct cmake_unknown_generator_error: virtual cmake_error {};

    struct cmake_unknown_generator_type_error: virtual cmake_unknown_generator_error
    {
        typedef boost::error_info<struct tag_generator_type, cmake::generator_type> generator_type;
    };

    struct cmake_unknown_generator_name_error: virtual cmake_unknown_generator_error
    {
        typedef boost::error_info<struct tag_generator_name, std::string> generator_name;
    };

    struct cmake_unknown_platform_generator_error: virtual cmake_unknown_generator_error {};
}}}
