#pragma once

#include "bunsan/utility/maker.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace bunsan{namespace utility{namespace makers
{
    class make: public maker
    {
    public:
        explicit make(const boost::filesystem::path &exe);

        void exec(
            const boost::filesystem::path &cwd,
            const std::vector<std::string> &targets) override;

        void setup(const utility::config_type &config) override;

    private:
        std::vector<std::string> argv_(
            const std::vector<std::string> &targets) const;

    private:
        const boost::filesystem::path m_exe;
        boost::optional<std::size_t> m_threads;
        std::unordered_map<std::string, std::string> m_defines;
        std::vector<std::string> m_targets;

    private:
        static const bool factory_reg_hook;
    };
}}}
