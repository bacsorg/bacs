#pragma once

#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <functional>
#include <string>

namespace bunsan::utility {

namespace detail {
template <typename Factory>
class configured_factory {
 public:
  using factory = Factory;
  using factory_type = typename factory::factory_type;
  using result_type = typename factory::bunsan_factory::result_type;
  using arguments_size = typename factory::bunsan_factory::arguments_size;

 public:
  configured_factory(const factory_type &factory_,
                     const utility_config &config_)
      : m_factory(factory_), m_config(config_) {}

  result_type operator()(resolver &res) const {
    return m_factory(m_config, res);
  }

 private:
  factory_type m_factory;
  utility_config m_config;
};
}  // namespace detail

template <typename Factory>
struct factory_options {
  using factory = Factory;
  using result_type = typename factory::bunsan_factory::result_type;
  using factory_type = std::function<result_type(resolver &)>;

  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(type);
    ar & BOOST_SERIALIZATION_NVP(config);
  }

  /// Try to create instance of type using Factory.
  result_type instance(resolver &res) const {
    return factory::instance(type, config, res);
  }

  /// Try to create instance of type using Factory.
  result_type instance_optional(resolver &res) const {
    return factory::instance_optional(type, config, res);
  }

  factory_type configured_factory() const {
    return detail::configured_factory<factory>(factory::factory(type), config);
  }

  std::string type;
  utility_config config;
};

}  // namespace bunsan::utility
