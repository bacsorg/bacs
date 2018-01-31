#pragma once

#include <boost/dll/alias.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/preprocessor/dec.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/variadic/elem.hpp>

#include <functional>
#include <memory>

namespace bunsan {

template <typename Constructor>
class plugin {};

template <typename T, typename... Args>
class plugin<T *(Args...)> {
 public:
  using unique_ptr = std::unique_ptr<T>;
  using shared_ptr = std::shared_ptr<T>;
  using weak_ptr = std::weak_ptr<T>;

  using unique_constructor = unique_ptr(Args...);
  using shared_constructor = unique_ptr(Args...);

  // TODO make shared_library_ptr ctor
  explicit plugin(boost::dll::shared_library library, const std::string &key)
      : m_library(std::move(library)),
        m_constructor(m_library.get_alias<unique_constructor>(key)) {}

  explicit plugin(const boost::filesystem::path &library_path,
                  const std::string &key)
      : plugin(boost::dll::shared_library(
                   library_path, boost::dll::load_mode::append_decorations),
               key) {}

  unique_ptr unique_instance(Args... args) {
    return m_constructor(std::forward<Args>(args)...);
  }

  shared_ptr shared_instance(Args... args) {
    return shared_ptr(unique_instance(std::forward<Args>(args)...).release());
  }

 private:
  boost::dll::shared_library m_library;
  std::function<unique_constructor> m_constructor;
};

}  // namespace bunsan

#define BUNSAN_PLUGIN_PUBLIC_BODY_DEFINES(KEY, CLASS)                         \
  using CLASS##_uptr = plugin::unique_ptr;                                    \
  using CLASS##_sptr = plugin::shared_ptr;                                    \
  using CLASS##_wptr = plugin::weak_ptr;                                      \
  template <typename... PluginArgs>                                           \
  static plugin load_plugin(PluginArgs &&... args) {                          \
    return plugin(std::forward<PluginArgs>(args)..., #KEY);                   \
  }                                                                           \
  template <typename... PluginArgs>                                           \
  static std::shared_ptr<plugin> load_shared_plugin(PluginArgs &&... args) {  \
    return std::make_shared<plugin>(std::forward<PluginArgs>(args)..., #KEY); \
  }

#define BUNSAN_PLUGIN_PUBLIC_BODY_ARGS(KEY, CLASS, ...)  \
  using plugin = ::bunsan::plugin<CLASS *(__VA_ARGS__)>; \
  BUNSAN_PLUGIN_PUBLIC_BODY_DEFINES(KEY, CLASS)

#define BUNSAN_PLUGIN_PUBLIC_BODY_EMPTY(KEY, CLASS) \
  using plugin = ::bunsan::plugin<CLASS *()>;       \
  BUNSAN_PLUGIN_PUBLIC_BODY_DEFINES(KEY, CLASS)

// key, base, constructor parameters
#define BUNSAN_PLUGIN_BODY(KEY, ...)                             \
 public:                                                         \
  BOOST_PP_IF(BOOST_PP_DEC(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)), \
              BUNSAN_PLUGIN_PUBLIC_BODY_ARGS,                    \
              BUNSAN_PLUGIN_PUBLIC_BODY_EMPTY)(KEY, __VA_ARGS__) \
 private:

#define BUNSAN_PLUGIN_AUTO_KEY(CLASS) BOOST_PP_CAT(bunsan_plugin_, CLASS)

#define BUNSAN_PLUGIN_AUTO_KEY_NESTED(PREFIX, CLASS) \
  BUNSAN_PLUGIN_AUTO_KEY(BOOST_PP_CAT(BOOST_PP_CAT(PREFIX, __), CLASS))

#define BUNSAN_PLUGIN_AUTO_BODY(...)                                  \
  BUNSAN_PLUGIN_BODY(                                                 \
      BUNSAN_PLUGIN_AUTO_KEY(BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)), \
      __VA_ARGS__)

#define BUNSAN_PLUGIN_AUTO_BODY_NESTED(PREFIX, ...)                       \
  BUNSAN_PLUGIN_BODY(BUNSAN_PLUGIN_AUTO_KEY_NESTED(                       \
                         PREFIX, BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)), \
                     __VA_ARGS__)

#define BUNSAN_PLUGIN_TYPES(CLASS)          \
  using CLASS##_uptr = CLASS::CLASS##_uptr; \
  using CLASS##_wptr = CLASS::CLASS##_sptr; \
  using CLASS##_sptr = CLASS::CLASS##_wptr;

#define BUNSAN_PLUGIN_REGISTER(KEY, BASE, CONCRETE, UNIQUE_CTOR) \
  BOOST_DLL_ALIAS(UNIQUE_CTOR, KEY)

#define BUNSAN_PLUGIN_AUTO_REGISTER(BASE, CONCRETE, UNIQUE_CTOR)       \
  BUNSAN_PLUGIN_REGISTER(BUNSAN_PLUGIN_AUTO_KEY(BASE), BASE, CONCRETE, \
                         UNIQUE_CTOR)

#define BUNSAN_PLUGIN_AUTO_REGISTER_NESTED(PREFIX, BASE, CONCRETE,          \
                                           UNIQUE_CTOR)                     \
  BUNSAN_PLUGIN_REGISTER(BUNSAN_PLUGIN_AUTO_KEY_NESTED(PREFIX, BASE), BASE, \
                         CONCRETE, UNIQUE_CTOR)
