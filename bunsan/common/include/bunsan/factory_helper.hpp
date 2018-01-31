#pragma once

#include <bunsan/factory.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/dec.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/variadic/elem.hpp>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>

#define BUNSAN_FACTORY_PUBLIC_BODY_PRE(CLASS)                                  \
  struct unknown_##CLASS##_error : virtual ::bunsan::unknown_factory_error {}; \
  template <typename T>                                                        \
  using shared_ptr = std::shared_ptr<T>;                                       \
  using CLASS##_ptr = shared_ptr<CLASS>;

#define BUNSAN_FACTORY_PUBLIC_BODY_POST(CLASS)                                 \
  using factory_type = typename bunsan_factory::factory_type;                  \
  static factory_type factory_optional(                                        \
      const typename bunsan_factory::key_type &type) {                         \
    return bunsan_factory::factory_optional(factories, type);                  \
  }                                                                            \
  static factory_type factory(const typename bunsan_factory::key_type &type) { \
    return bunsan_factory::factory(factories, type);                           \
  }                                                                            \
  template <typename... Args>                                                  \
  static typename std::enable_if<bunsan_factory::arguments_size::value ==      \
                                     sizeof...(Args),                          \
                                 CLASS##_ptr>::type                            \
  instance_optional(const typename bunsan_factory::key_type &type,             \
                    Args &&... args) {                                         \
    return bunsan_factory::instance_optional(factories, type,                  \
                                             std::forward<Args>(args)...);     \
  }                                                                            \
  template <typename... Args>                                                  \
  static typename std::enable_if<bunsan_factory::arguments_size::value ==      \
                                     sizeof...(Args),                          \
                                 CLASS##_ptr>::type                            \
  instance(const typename bunsan_factory::key_type &type, Args &&... args) {   \
    return bunsan_factory::instance(factories, type,                           \
                                    std::forward<Args>(args)...);              \
  }                                                                            \
  static typename bunsan_factory::const_range registered() {                   \
    return bunsan_factory::registered(factories);                              \
  }                                                                            \
  template <typename T, typename... Args>                                      \
  static shared_ptr<T> make_shared(Args &&... args) {                          \
    return std::make_shared<T>(std::forward<Args>(args)...);                   \
  }                                                                            \
  static bool register_new(const typename bunsan_factory::key_type &type,      \
                           const factory_type &f) {                            \
    return bunsan_factory::register_new(factories, type, f);                   \
  }

#define BUNSAN_FACTORY_PUBLIC_BODY_ARGS(CLASS, ...)                       \
  BUNSAN_FACTORY_PUBLIC_BODY_PRE(CLASS)                                   \
  using bunsan_factory =                                                  \
      bunsan::factory<CLASS##_ptr(__VA_ARGS__), unknown_##CLASS##_error>; \
  BUNSAN_FACTORY_PUBLIC_BODY_POST(CLASS)

#define BUNSAN_FACTORY_PUBLIC_BODY_EMPTY(CLASS)                \
  BUNSAN_FACTORY_PUBLIC_BODY_PRE(CLASS)                        \
  using bunsan_factory =                                       \
      bunsan::factory<CLASS##_ptr(), unknown_##CLASS##_error>; \
  BUNSAN_FACTORY_PUBLIC_BODY_POST(CLASS)

#define BUNSAN_FACTORY_PRIVATE_BODY(CLASS) \
  static typename bunsan_factory::map_ptr_type factories;

/*!
 * \def BUNSAN_FACTORY_PUBLIC_BODY(CLASS, ...)
 *
 * \brief Defines abstract factory's body.
 *
\code
namespace bunsan {
class factory_base {
  BUNSAN_FACTORY_BODY(factory_base, const std::string &arg)
 public:
  virtual void f()=0;
};
BUNSAN_FACTORY_TYPES(factory_base)
}  // namespace bunsan
\endcode
 */
#define BUNSAN_FACTORY_BODY(...)                                 \
 public:                                                         \
  BOOST_PP_IF(BOOST_PP_DEC(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)), \
              BUNSAN_FACTORY_PUBLIC_BODY_ARGS,                   \
              BUNSAN_FACTORY_PUBLIC_BODY_EMPTY)(__VA_ARGS__)     \
 private:                                                        \
  BUNSAN_FACTORY_PRIVATE_BODY(BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__))

#define BUNSAN_FACTORY_TYPES(CLASS)       \
  using CLASS##_ptr = CLASS::CLASS##_ptr; \
  using unknown_##CLASS##_error = CLASS::unknown_##CLASS##_error;

/*!
 * \def BUNSAN_FACTORY_BEGIN(CLASS, ...)
 *
 * \param CLASS class name
 * \param ... constructor argument types
 *
 * should be used after class name declaration,
 * for example
 *
\code
namespace bunsan {
class factory_base
BUNSAN_FACTORY_BEGIN(factory_base, const std::string &arg)
 public:
  virtual void f()=0;
BUNSAN_FACTORY_END(factory_base)
}  // namespace bunsan
\endcode
 */
#define BUNSAN_FACTORY_BEGIN(...) \
  {                               \
   BUNSAN_FACTORY_BODY(__VA_ARGS__)

/*!
 * \def BUNSAN_FACTORY_END(CLASS)
 *
 * \param CLASS class name
 *
 * should be used after class members declarations
 *
 * \see BUNSAN_FACTORY_BEGIN
 */
#define BUNSAN_FACTORY_END(CLASS) \
  };                              \
  BUNSAN_FACTORY_TYPES(CLASS)

/*!
 * \def BUNSAN_FACTORY_DEFINE(CLASS)
 *
 * \param CLASS class name
 *
 * \note
 * class name should be qualified
 * depending on surrounding namespaces
 *
 * should be used in implementation file, for example
 *
\code
BUNSAN_FACTORY_DEFINE(bunsan::factory_base)
\endcode
\code
namespace bunsan {
  BUNSAN_FACTORY_DEFINE(factory_base)
}  // namespace bunsan
\endcode
 */
#define BUNSAN_FACTORY_DEFINE(CLASS) \
  typename CLASS::bunsan_factory::map_ptr_type CLASS::factories;

#define BUNSAN_FACTORY_REGISTER(VAR, FACTORY, NAME, CODE)       \
  static const bool BOOST_PP_CAT(bunsan_factory_register_,      \
                                 BOOST_PP_CAT(VAR, __LINE__)) = \
      FACTORY::register_new(NAME, CODE);

#define BUNSAN_FACTORY_REGISTER_TOKEN(FACTORY, TOKEN, CODE) \
  BUNSAN_FACTORY_REGISTER(TOKEN, FACTORY, #TOKEN, CODE)
