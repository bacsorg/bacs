#ifndef SRC_FETCHERS_COPY_HPP
#define SRC_FETCHERS_COPY_HPP

#include "bunsan/utility/fetcher.hpp"

namespace bunsan{namespace utility{namespace fetchers
{
    class copy: public fetcher
    {
    public:
        copy()=default;
        virtual void fetch(const std::string &uri, const boost::filesystem::path &dst);
    private:
        static const bool factory_reg_hook_cp;
        static const bool factory_reg_hook_copy;
    };
}}}

#endif //SRC_FETCHERS_COPY_HPP

