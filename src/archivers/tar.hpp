#ifndef SRC_ARCHIVERS_TAR_HPP
#define SRC_ARCHIVERS_TAR_HPP

#include <unordered_set>

#include "cwd_split.hpp"

namespace bunsan{namespace utility{namespace archivers
{
    class tar: public cwd_split
    {
        const boost::filesystem::path m_exe;
        std::string m_format;
        std::unordered_set<std::string> m_args;
    public:
        explicit tar(const boost::filesystem::path &exe);
        virtual void unpack(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &dir);
        virtual void setarg(const std::string &key, const std::string &value);
    protected:
        virtual void pack_from(
            const boost::filesystem::path &cwd,
            const boost::filesystem::path &archive,
            const boost::filesystem::path &file);
    private:
        static const bool factory_reg_hook;
    };
}}}

#endif //SRC_ARCHIVERS_TAR_HPP

