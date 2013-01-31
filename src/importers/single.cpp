#include "single.hpp"

#include "bacs/problem/error.hpp"

#include "bacs/single/problem/driver.hpp"
#include "bacs/single/problem/generator.hpp"

#include "bunsan/config/cast.hpp"

#include <sstream>

namespace bacs{namespace problem{namespace importers
{
    const bool single::factory_reg_hook = importer::impl::register_new("bacs/single/problem",
        [](const boost::property_tree::ptree &config_)
        {
            impl_ptr tmp(new single(config_));
            return tmp;
        });

    namespace
    {
        template <typename GeneratorConfig>
        bacs::single::problem::generator_ptr get_generator(const GeneratorConfig &config)
        {
            return bacs::single::problem::generator::instance(config.type, config.config);
        }
    }

    single::single(const boost::property_tree::ptree &config_):
        m_generator(get_generator(bunsan::config::load<config>(config_).generator)) {}

    info single::convert(const options &options_)
    {
        const bacs::single::problem::driver_ptr drv =
            bacs::single::problem::driver::instance(options_.problem_dir);
        bacs::single::problem::generator::options opts;
        opts.driver = drv;
        opts.destination = options_.destination;
        opts.root_package = options_.root_package;
        bacs::single::api::pb::problem::Problem problem_info = m_generator->generate(opts);
        problem_info.mutable_info()->mutable_system()->set_hash(options_.hash.data(), options_.hash.size());
        std::ostringstream sout;
        if (!problem_info.SerializeToOstream(&sout))
            BOOST_THROW_EXCEPTION(problem_info_serialization_error());
        const std::string info_str = sout.str();
        return info{"bacs/single/problem", {info_str.begin(), info_str.end()}};
    }
}}}
