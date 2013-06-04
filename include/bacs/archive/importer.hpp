#pragma once

#include "bacs/problem/importer.hpp"

#include <boost/noncopyable.hpp>
#include <boost/thread/tss.hpp>

namespace bacs{namespace archive
{
    class importer: public bacs::problem::importer
    {
    public:
        explicit importer(const boost::property_tree::ptree &config);

        /// This function is thread safe.
        bacs::problem::Problem convert(const options &options_) override;

    private:
        const boost::property_tree::ptree m_config;
        boost::thread_specific_ptr<bacs::problem::importer_ptr> m_impl;
    };
}}
