#ifndef SRC_BUILDERS_CONF_MAKE_INSTALL_HPP
#define SRC_BUILDERS_CONF_MAKE_INSTALL_HPP

#include "bunsan/utility/builder.hpp"

namespace bunsan{namespace utility{namespace builders
{
    class conf_make_install: public builder
    {
        virtual void install(
            const boost::filesystem::path &src,
            const boost::filesystem::path &bin,
            const boost::filesystem::path &root);
    protected:
        virtual void configure_(
            const boost::filesystem::path &src,
            const boost::filesystem::path &bin)=0;
        virtual void make_(
            const boost::filesystem::path &src,
            const boost::filesystem::path &bin)=0;
        virtual void install_(
            const boost::filesystem::path &src,
            const boost::filesystem::path &bin,
            const boost::filesystem::path &root)=0;
    };
}}}

#endif //SRC_BUILDERS_CONF_MAKE_INSTALL_HPP

