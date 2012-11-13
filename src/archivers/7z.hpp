#pragma once

#include "cwd_split.hpp"

namespace bunsan{namespace utility{namespace archivers
{
    class _7z: public cwd_split
    {
    public:
        explicit _7z(const boost::filesystem::path &exe);

        void unpack(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &dir) override;

        void setarg(const std::string &key, const std::string &value) override;

    protected:
        void pack_from(
            const boost::filesystem::path &cwd,
            const boost::filesystem::path &archive,
            const boost::filesystem::path &file) override;

    private:
        static const bool factory_reg_hook_7z;
        static const bool factory_reg_hook_7za;
        static const bool factory_reg_hook_7zr;

    private:
        const boost::filesystem::path m_exe;
        std::string m_format;
    };
}}}
