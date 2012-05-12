#ifndef BUNSAN_UTILITY_ARCHIVER_HPP
#define BUNSAN_UTILITY_ARCHIVER_HPP

#include "bunsan/factory_helper.hpp"
#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/utility.hpp"

namespace bunsan{namespace utility
{
    class archiver: public utility
    BUNSAN_FACTORY_BEGIN(archiver, const resolver &)
    public:
        virtual void pack(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &file)=0;
        virtual void pack_content(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &dir)=0;
        virtual void unpack(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &dir)=0;
    BUNSAN_FACTORY_END(archiver)
}}

#endif //BUNSAN_UTILITY_ARCHIVER_HPP

