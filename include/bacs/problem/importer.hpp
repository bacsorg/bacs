#pragma once

#include <bacs/problem/common.hpp>
#include <bacs/problem/problem.pb.h>

#include <bunsan/pm/entry.hpp>
#include <bunsan/factory_helper.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

namespace bacs{namespace problem
{
    /*!
     * \brief Imports problems into internal storage.
     *
     * Use bunsan::factory to register problem types.
     */
    class importer: private boost::noncopyable
    BUNSAN_FACTORY_BEGIN(importer, const boost::property_tree::ptree &)
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
         * \return importer that converts problems for registered types.
         *
         * \param config per-implementation config, e.g.
         *
         * \code{.cpp}
         * bacs/problem/single
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
         * bacs/problem/.../another
         * {
         *     ; another configuration
         * }
         * \endcode
         */
        static importer_ptr instance(const boost::property_tree::ptree &config);

    public:
        /// It is safe to call this function from different threads.
        virtual Problem convert(const options &options_)=0;

        virtual ~importer();

    protected:
        static std::string get_problem_format(const boost::filesystem::path &problem_dir);
        static std::string get_problem_type(const boost::filesystem::path &problem_dir);
    BUNSAN_FACTORY_END(importer)
}}
