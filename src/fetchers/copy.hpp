#pragma once

#include "bunsan/utility/fetcher.hpp"

namespace bunsan{namespace utility{namespace fetchers
{
    class copy: public fetcher
    {
    public:
        copy()=default;
        void fetch(const std::string &uri, const boost::filesystem::path &dst) override;

    private:
        static const bool factory_reg_hook_cp;
        static const bool factory_reg_hook_copy;
    };
}}}
