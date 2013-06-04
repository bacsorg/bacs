#include "bacs/archive/importer.hpp"

namespace bacs{namespace archive
{
    importer::importer(const boost::property_tree::ptree &config):
        m_config(config) {}

    problem::Problem importer::convert(const options &options_)
    {
        if (!m_impl.get())
        {
            const importer_ptr tmp = instance(m_config);
            m_impl.reset(new importer_ptr(tmp));
        }
        return (*m_impl)->convert(options_);
    }
}}
