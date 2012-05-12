#ifndef SRC_MAKERS_MAKE_HPP
#define SRC_MAKERS_MAKE_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "bunsan/utility/maker.hpp"

namespace bunsan{namespace utility{namespace makers
{
    class make: public maker
    {
        const boost::filesystem::path m_exe;
        boost::optional<std::size_t> m_threads;
        std::unordered_map<std::string, std::string> m_defines;
        std::vector<std::string> m_targets;
    public:
        explicit make(const boost::filesystem::path &exe);
        virtual void exec(
            const boost::filesystem::path &cwd,
            const std::vector<std::string> &targets);
        virtual void setup(const utility::config_type &config);
    private:
        std::vector<std::string> argv_(
            const std::vector<std::string> &targets) const;
        static bool factory_reg_hook;
    };
}}}

#endif //SRC_MAKERS_MAKE_HPP

