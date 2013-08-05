#pragma once

#include <bunsan/utility/fetcher.hpp>

namespace bunsan{namespace utility{namespace fetchers
{
    class curl: public fetcher
    {
    public:
        explicit curl(const boost::filesystem::path &exe);

        void fetch(const std::string &uri, const boost::filesystem::path &dst) override;

    private:
        const boost::filesystem::path m_exe;

    private:
        static const bool factory_reg_hook;
    };
}}}
