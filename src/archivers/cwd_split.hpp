#pragma once

#include "bunsan/utility/archiver.hpp"

namespace bunsan{namespace utility{namespace archivers
{
    class cwd_split: public archiver
    {
    public:
        void pack(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &file) override;

        void pack_contents(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &dir) override;

    protected:
        virtual void pack_from(
            const boost::filesystem::path &cwd,
            const boost::filesystem::path &archive,
            const boost::filesystem::path &file)=0;
    };
}}}
