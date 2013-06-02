#pragma once

#include "bacs/problem/buildable.hpp"
#include "bacs/problem/problem.pb.h"

#include "bunsan/factory_helper.hpp"

#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace problem
{
    /// \see \ref utility_page
    class utility: public buildable
    BUNSAN_FACTORY_BEGIN(utility, const boost::filesystem::path &/*location*/,
                                  const boost::property_tree::ptree &/*config*/)
    public:
        static utility_ptr instance(const boost::filesystem::path &location);
        static utility_ptr instance_optional(const boost::filesystem::path &location);

    public:
        utility(const boost::filesystem::path &location,
                const boost::property_tree::ptree &config);

    public:
        virtual Utility info() const;

        /// One of sections in configuration.
        virtual boost::property_tree::ptree section(const std::string &name) const;

        virtual boost::filesystem::path location() const;

        virtual boost::filesystem::path target() const;

    private:
        const boost::filesystem::path m_location;
        const boost::filesystem::path m_target;
        const boost::property_tree::ptree m_config;
    BUNSAN_FACTORY_END(utility)
}}
