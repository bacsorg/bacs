#pragma once

#include "bacs/problem/common.hpp"

#include "bunsan/pm/entry.hpp"
#include "bunsan/factory_helper.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

namespace bacs{namespace problem
{
    class importer
    {
    public:
        struct options
        {
            boost::filesystem::path problem_dir;
            boost::filesystem::path destination;
            bunsan::pm::entry root_package;
            problem::id id;
            problem::hash hash;
        };

    public:
        /*!
         * \param config per-implementation config, e.g.
         *
         * \code{.cpp}
         * bacs/single/problem
         * {
         *     generator
         *     {
         *         type internal0
         *         config
         *         {
         *         }
         *     }
         * }
         *
         * bacs/.../another
         * {
         *     ; another configuration
         * }
         * \endcode
         */
        explicit importer(const boost::property_tree::ptree &config);

        importer(const importer &)=default;
        importer(importer &&)=default;
        importer &operator=(const importer &)=default;
        importer &operator=(importer &&)=default;

        /// It is safe to call this function from different threads.
        info convert(const options &options_) const;

    public:
        class impl
        BUNSAN_FACTORY_BEGIN(impl, const boost::property_tree::ptree &)
        public:
            typedef importer::options options;

        public:
            virtual ~impl();

        public:
            static impl_ptr instance(const boost::filesystem::path &problem_dir,
                                     const boost::property_tree::ptree &config);

        public:
            virtual info convert(const options &options_)=0;
        BUNSAN_FACTORY_END(impl)

    private:
        boost::property_tree::ptree m_config;
    };
}}
