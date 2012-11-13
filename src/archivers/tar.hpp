#pragma once

#include "cwd_split.hpp"

#include <unordered_set>

namespace bunsan{namespace utility{namespace archivers
{
    class tar: public cwd_split
    {
    public:
        explicit tar(const boost::filesystem::path &exe);

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
        const boost::filesystem::path m_exe;
        std::string m_format;
        std::unordered_set<std::string> m_args;

    private:
        static const bool factory_reg_hook;
    };
}}}
