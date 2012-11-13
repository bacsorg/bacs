#pragma once

#include "conf_make_install.hpp"

#include "bunsan/utility/maker.hpp"

#include <unordered_map>
#include <string>
#include <vector>

#include <boost/optional.hpp>

namespace bunsan{namespace utility{namespace builders
{
    class cmake: public conf_make_install
    {
    public:
        explicit cmake(const resolver &resolver_);
        void setup(const utility::config_type &config) override;

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

    private:
        struct generator
        {
            enum class type: std::size_t;
            const std::string m_id;
            const type m_type;
        };

    private:
        void setup_generator();
        void set_generator(const std::string &gen_id);
        const generator &get_generator() const;
        std::vector<std::string> argv_(
            const boost::filesystem::path &src) const;

    private:
        const resolver m_resolver;
        const boost::filesystem::path m_cmake_exe;

        boost::optional<std::size_t> m_generator;

        std::unordered_map<std::string, std::string> m_cmake_defines;
        utility::config_type m_make_maker_config, m_install_maker_config;

    private:
        static const std::vector<generator> generators;
        static const bool factory_reg_hook;
    };
}}}
