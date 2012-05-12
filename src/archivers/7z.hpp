#ifndef SRC_ARCHIVERS_7Z_HPP
#define SRC_ARCHIVERS_7Z_HPP

#include "cwd_split.hpp"

namespace bunsan{namespace utility{namespace archivers
{
    class _7z: public cwd_split
    {
        const boost::filesystem::path m_exe;
        std::string m_format;
    public:
        explicit _7z(const boost::filesystem::path &exe);
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
        static bool factory_reg_hook_7z;
        static bool factory_reg_hook_7za;
        static bool factory_reg_hook_7zr;
    };
}}}

#endif //SRC_ARCHIVERS_7Z_HPP

