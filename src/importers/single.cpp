#include "single.hpp"

#include "bunsan/config/cast.hpp"

namespace bacs{namespace problem{namespace importers
{
    const bool single::factory_reg_hook = importer::impl::register_new("bacs/problem/single",
        [](const boost::property_tree::ptree &config_)
        {
            impl_ptr tmp(new single(config_));
            return tmp;
        });

    single::single(const boost::property_tree::ptree &config_):
        m_config(bunsan::config::load<config>(config_)) {}

    info single::convert(const options &options_)
    {
        return info();
    }
}}}
