#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>

namespace bunsan {
namespace utility {

namespace detail {
template <typename Factory>
class configured_factory {
 public:
  typedef Factory factory;
  typedef typename factory::factory_type factory_type;
  typedef typename factory::bunsan_factory::result_type result_type;
  typedef typename factory::bunsan_factory::arguments_size arguments_size;

 public:
  configured_factory(const factory_type &factory_,
                     const boost::property_tree::ptree &config_)
      : m_factory(factory_), m_config(config_) {}

  template <typename... Args>
  typename std::enable_if<arguments_size::value == sizeof...(Args),
                          result_type>::type
  operator()(Args &&... args) const {
    const result_type tmp = m_factory(std::forward<Args>(args)...);
    tmp->setup(m_config);
    return tmp;
  }

 private:
  factory_type m_factory;
  boost::property_tree::ptree m_config;
};
}  // namespace detail

template <typename Factory>
struct factory_options {
  typedef Factory factory;
  typedef typename factory::factory_type factory_type;
  typedef typename factory::bunsan_factory::result_type result_type;
  typedef typename factory::bunsan_factory::arguments_size arguments_size;

  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(type);
    ar & BOOST_SERIALIZATION_NVP(config);
  }

  /// Try to create instance of type using Factory and setup it.
  template <typename... Args>
  typename std::enable_if<arguments_size::value == sizeof...(Args),
                          result_type>::type
  instance(Args &&... args) const {
    const result_type tmp =
        factory::instance(type, std::forward<Args>(args)...);
    tmp->setup(config);
    return tmp;
  }

  /// Try to create instance of type using Factory, setup it on success.
  template <typename... Args>
  typename std::enable_if<arguments_size::value == sizeof...(Args),
                          result_type>::type
  instance_optional(Args &&... args) const {
    const result_type tmp =
        factory::instance_optional(type, std::forward<Args>(args)...);
    if (tmp) tmp->setup(config);
    return tmp;
  }

  factory_type configured_factory() const {
    return detail::configured_factory<factory>(factory::factory(type), config);
  }

  std::string type;
  boost::property_tree::ptree config;
};

}  // namespace utility
}  // namespace bunsan
