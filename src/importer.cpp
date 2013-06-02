#include "bacs/problem/importer.hpp"
#include "bacs/problem/error.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <iterator>

#include <boost/algorithm/string/trim.hpp>

namespace bacs{namespace problem
{
    BUNSAN_FACTORY_DEFINE(importer)

    importer::~importer() {}

    std::string importer::get_problem_format(const boost::filesystem::path &problem_dir)
    {
        std::string format;
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream fin(problem_dir / "format");
            format.assign(std::istreambuf_iterator<char>(fin),
                        std::istreambuf_iterator<char>());
            fin.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
        boost::algorithm::trim(format);
        if (format.empty())
            BOOST_THROW_EXCEPTION(empty_problem_format_error());
        return format;
    }

    std::string importer::get_problem_type(const boost::filesystem::path &problem_dir)
    {
        const std::string format = get_problem_format(problem_dir);
        constexpr char delim = '#';
        const std::string::size_type delim_pos = format.find(delim);
        const std::string type = format.substr(0, delim_pos);
        if (type.empty())
            BOOST_THROW_EXCEPTION(empty_problem_type_error() <<
                                  empty_problem_type_error::problem_format(format));
        return type;
    }

    namespace
    {
        class universal_importer: public importer
        {
        public:
            explicit universal_importer(const boost::property_tree::ptree &config):
                m_config(config) {}

            pb::Problem convert(const options &options_) override
            {
                const std::string type = get_problem_type(options_.problem_dir);
                const pb::Problem problem = get_implementation(type)->convert(options_);
                BOOST_ASSERT(problem.info().system().has_problem_type());
                BOOST_ASSERT(problem.info().system().problem_type() == type);
                return problem;
            }

        private:
            // TODO consider implementation caching
            importer_ptr get_implementation(const std::string &type)
            {
                importer_ptr impl;
                const boost::property_tree::ptree::const_assoc_iterator config_iter = m_config.find(type);
                if (config_iter != m_config.not_found())
                    impl = instance_optional(type, config_iter->second);
                else
                    impl = instance_optional(type, boost::property_tree::ptree());
                if (!impl)
                    BOOST_THROW_EXCEPTION(unknown_problem_type_error() <<
                                          unknown_problem_type_error::problem_type(type));
                return impl;
            }

        private:
            const boost::property_tree::ptree m_config;
        };
    }

    importer_ptr importer::instance(const boost::property_tree::ptree &config)
    {
        const importer_ptr tmp(new universal_importer(config));
        return tmp;
    }
}}
