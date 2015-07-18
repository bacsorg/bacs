#include <bacs/archive/importer.hpp>

#include <bacs/archive/error.hpp>

namespace bacs {
namespace archive {

importer::importer(const boost::property_tree::ptree &config)
    : m_config(config) {}

bacs::problem::Problem importer::convert(const options &options_) {
  try {
    if (!m_impl.get()) {
      const importer_ptr tmp = instance(m_config);
      m_impl.reset(new importer_ptr(tmp));
    }
    return (*m_impl)->convert(options_);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(bacs::problem::importer_convert_error()
                          << bacs::problem::importer_convert_error::options(
                                 options_) << bunsan::enable_nested_current());
  }
}

}  // namespace archive
}  // namespace bacs
