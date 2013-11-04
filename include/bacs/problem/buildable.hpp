#pragma once

#include <bacs/problem/error.hpp>

#include <bunsan/pm/entry.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

namespace bacs{namespace problem
{
    struct buildable_error: virtual error {};
    struct buildable_make_package_error: virtual buildable_error
    {
        typedef boost::error_info<struct tag_destination, boost::filesystem::path> destination;
        typedef boost::error_info<struct tag_package, bunsan::pm::entry> package;
    };

    /*!
     * \brief Something that may be build by bunsan::pm.
     *
     * \see \ref buildable_page
     */
    class buildable: private boost::noncopyable
    {
    public:
        virtual ~buildable();

    public:
        /// \return false if no package is needed
        virtual bool make_package(const boost::filesystem::path &destination,
                                  const bunsan::pm::entry &package) const=0;
    };
}}
