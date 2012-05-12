#ifndef BUNSAN_UTILITY_FETCHER_HPP
#define BUNSAN_UTILITY_FETCHER_HPP

#include "bunsan/factory_helper.hpp"
#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/utility.hpp"

namespace bunsan{namespace utility
{
    class fetcher: public utility
    BUNSAN_FACTORY_BEGIN(fetcher, const resolver &)
    public:
        virtual void fetch(const std::string &uri, const boost::filesystem::path &dst)=0;
    BUNSAN_FACTORY_END(fetcher)
}}

#endif //BUNSAN_UTILITY_FETCHER_HPP

