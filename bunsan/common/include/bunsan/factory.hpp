#pragma once

#include <bunsan/error.hpp>
#include <bunsan/lazy_ptr.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace bunsan {

struct unknown_factory_error : virtual error {
  using factory_type = boost::error_info<struct tag_factory_type, std::string>;
};

namespace detail {

/*!
 * \brief This class is implementation of bunsan::factory concept.
 *
 * bunsan::factory is abstract factory implementation, where each derived
 * class is registered in abstract factory by std::string identifier
 * on library load using bunsan::factory::register_new().
 * Later instance of this type may be created by bunsan::factory::instance().
 *
 * \note We need class name different than "factory"
 * to use it as member function's.
 */
template <typename Signature, typename UnknownError>
class factory_base;

template <typename Result, typename... Args, typename UnknownError>
class factory_base<Result(Args...), UnknownError> {
 public:
  using result_type = Result;
  using factory_type = std::function<Result(Args...)>;
  using unknown_error = UnknownError;
  using key_type = std::string;
  using arguments_size = std::integral_constant<std::size_t, sizeof...(Args)>;
  using map_type = std::unordered_map<key_type, factory_type>;
  using value_type = typename map_type::value_type;
  using map_const_iterator = typename map_type::const_iterator;
  using map_ptr_type = global_lazy_ptr<map_type>;

  struct iterator_converter
      : std::unary_function<const value_type &, const key_type &> {
    const key_type &operator()(const value_type &pair) const {
      return pair.first;
    }
  };

  using const_iterator =
      boost::transform_iterator<iterator_converter, map_const_iterator>;
  using const_range = boost::iterator_range<const_iterator>;

  /*!
   * \brief Registers new factory with unique identifier.
   *
   * \warning factory should not return null pointer
   *
   * \return false if identifier is already used
   */
  static bool register_new(const map_ptr_type &factories, const key_type &type,
                           const factory_type &f) {
    if (factories->find(type) == factories->end()) {
      factories->insert(value_type(type, f));
      return true;
    }
    return false;
  }

  /*!
   * \brief Returns factory registered with specified identifier.
   *
   * \return Empty factory if it is not registered.
   */
  static factory_type factory_optional(const map_ptr_type &factories,
                                       const key_type &type) {
    const map_const_iterator iter = factories->find(type);
    if (iter != factories->end()) return iter->second;
    return factory_type();
  }

  /*!
   * \brief Returns factory registered with specified identifier.
   *
   * \throws unknown_factory if factory is not registered
   */
  static factory_type factory(const map_ptr_type &factories,
                              const key_type &type) {
    const factory_type f = factory_optional(factories, type);
    if (f) return f;
    BOOST_THROW_EXCEPTION(unknown_error()
                          << unknown_factory_error::factory_type(type));
  }

  /*!
   * \brief Returns new instance of specified type.
   *
   * \return nullptr if factory is not registered
   */
  static result_type instance_optional(const map_ptr_type &factories,
                                       const key_type &type, Args &&... args) {
    const factory_type f = factory_optional(factories, type);
    if (f) return f(std::forward<Args>(args)...);
    return result_type();
  }

  /*!
   * \brief Returns new instance of specified type.
   *
   * \throws unknown_factory if factory is not registered
   */
  static result_type instance(const map_ptr_type &factories,
                              const key_type &type, Args &&... args) {
    const result_type r =
        instance_optional(factories, type, std::forward<Args>(args)...);
    if (r) return r;
    BOOST_THROW_EXCEPTION(unknown_error()
                          << unknown_factory_error::factory_type(type));
  }

  /// Iterate over registered factory identifiers.
  static const_range registered(const map_ptr_type &factories) {
    return const_range{factories->begin(), factories->end()};
  }
};

}  // namespace detail

template <typename Signature,
          typename UnknownError = bunsan::unknown_factory_error>
using factory = detail::factory_base<Signature, UnknownError>;

}  // namespace bunsan
