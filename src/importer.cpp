#include "bacs/problem/importer.hpp"
#include "bacs/problem/error.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <iterator>

#include <boost/algorithm/string/trim.hpp>

namespace bacs{namespace problem
{
    BUNSAN_FACTORY_DEFINE(importer::impl)

    importer::importer(const boost::property_tree::ptree &config): m_config(config) {}

    importer::impl_ptr importer::impl::instance(const boost::filesystem::path &problem_dir,
                                                const boost::property_tree::ptree &config)
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
        constexpr char delim = '#';
        const std::string::size_type delim_pos = format.find(delim);
        impl_ptr impl_;
        if (format.find(delim, delim_pos + 1) == std::string::npos &&
            delim_pos != std::string::npos)
        {
            const std::string impl_type = format.substr(delim_pos + 1);
            const boost::property_tree::ptree::const_assoc_iterator config_iter = config.find(impl_type);
            if (config_iter != config.not_found())
                impl_ = instance_optional(impl_type, config_iter->second);
            else
                impl_ = instance_optional(impl_type, boost::property_tree::ptree());
        }
        if (!impl_)
            BOOST_THROW_EXCEPTION(unknown_format_error() <<
                                  unknown_format_error::format(format));
        return impl_;
    }

    info importer::convert(const options &options_) const
    {
        return impl::instance(options_.problem_dir, m_config)->convert(options_);
    }
}}
