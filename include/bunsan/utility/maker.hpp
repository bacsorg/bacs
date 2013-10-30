#pragma once

#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

namespace bunsan{namespace utility
{
    struct maker_error: virtual error
    {
        typedef boost::error_info<struct tag_cwd, boost::filesystem::path> cwd;
        typedef boost::error_info<struct tag_targets, std::vector<std::string>> targets;
    };
    struct maker_exec_error: virtual maker_error {};

    class maker: public utility
    BUNSAN_FACTORY_BEGIN(maker, const resolver &)
    public:
        virtual void exec(
            const boost::filesystem::path &cwd,
            const std::vector<std::string> &targets)=0;
    BUNSAN_FACTORY_END(maker)
}}

namespace boost
{
    std::string to_string(const bunsan::utility::maker_error::targets &targets);
}
