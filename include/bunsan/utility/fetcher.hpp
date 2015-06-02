#pragma once

#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

namespace bunsan{namespace utility
{
    struct fetcher_error: virtual error {};
    struct fetcher_fetch_error: virtual fetcher_error
    {
        typedef boost::error_info<struct tag_uri, std::string> uri;
        typedef boost::error_info<
            struct tag_destination,
            boost::filesystem::path
        > destination;
    };

    class fetcher: public utility
    BUNSAN_FACTORY_BEGIN(fetcher, resolver &)
    public:
        virtual void fetch(const std::string &uri, const boost::filesystem::path &dst)=0;
    BUNSAN_FACTORY_END(fetcher)
}}
