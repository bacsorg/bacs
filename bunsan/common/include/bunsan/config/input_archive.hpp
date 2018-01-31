#pragma once

#include <bunsan/config/error.hpp>
#include <bunsan/config/traits.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/variant.hpp>

#include <chrono>
#include <memory>

namespace bunsan {
namespace config {

template <typename Ptree>
class input_archive : private boost::noncopyable {
 public:
  using is_loading = std::integral_constant<bool, true>;
  using is_saving = std::integral_constant<bool, false>;

  constexpr unsigned int get_library_version() { return 0; }

 public:
  explicit input_archive(const Ptree &ptree) : m_ptree(&ptree) {}

  template <typename T>
  input_archive &operator&(T &obj) {
    return *this >> obj;
  }

  template <typename T>
  input_archive &operator>>(T &obj) {
    load(obj);
    return *this;
  }

  template <typename T>
  input_archive &operator>>(const boost::serialization::nvp<T> &nvp) {
    boost::optional<const Ptree &> ptree =
        m_ptree->get_child_optional(nvp.name());
    if (ptree) {
      load_from_ptree(nvp.value(), ptree.get());
    } else {
      reset(nvp.value());
    }
    return *this;
  }

  template <typename T>
  static void load_from_ptree(T &obj, const Ptree &ptree) {
    input_archive<Ptree> ia(ptree);
    ia >> obj;
  }

 private:
  template <typename T>
  void reset(T &value) {
    value = T();
  }

  /// For complex types.
  template <typename T>
  typename std::enable_if<traits::is_recursive<T>::value, void>::type load(
      T &obj) {
    traits::serializer<T>::load(obj, *this,
                                boost::serialization::version<T>::value);
  }

  /*!
   * \brief Helper for integral types to load them from std::istream
   * instances using oct and hex prefixes.
   */
  template <typename T>
  class integral_wrapper {
   public:
    constexpr integral_wrapper() = default;
    constexpr integral_wrapper(const T value) : value_(value) {}
    constexpr operator T() const { return value_; }

   public:
    template <typename CharT, typename Traits>
    friend std::basic_istream<CharT, Traits> &operator>>(
        std::basic_istream<CharT, Traits> &in, integral_wrapper &integral) {
      std::ios_base::fmtflags flags = in.flags();
      in.unsetf(std::ios_base::basefield);
      in >> integral.value_;
      in.setf(flags);
      return in;
    }

   private:
    T value_ = 0;
  };

  /// For integral primitive types.
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, void>::type
  load_primitive(T &obj) {
    obj = m_ptree->template get_value<integral_wrapper<T>>();
  }

  /// For non-integral primitive types.
  template <typename T>
  typename std::enable_if<!std::is_integral<T>::value, void>::type
  load_primitive(T &obj) {
    obj = m_ptree->template get_value<T>();
  }

  /// For primitive types.
  template <typename T>
  typename std::enable_if<traits::is_direct_assignable<T>::value, void>::type
  load(T &obj) {
    load_primitive(obj);
  }

  /// For boost::optional.
  template <typename T>
  void load(boost::optional<T> &obj) {
    T value;
    load(value);
    obj = value;
  }

  /// For Ptree.
  void load(Ptree &obj) { obj = *m_ptree; }

  /// For boost::filesystem::path.
  void load(boost::filesystem::path &obj) {
    obj = m_ptree->template get_value<std::string>();
  }

  /// For std::chrono::duration.
  template <typename Rep, typename Period>
  void load(std::chrono::duration<Rep, Period> &obj) {
    obj =
        std::chrono::duration<Rep, Period>(m_ptree->template get_value<Rep>());
  }

  /// For random access sequences.
  template <typename T>
  typename std::enable_if<traits::is_random_access_sequence<T>::value,
                          void>::type
  load(T &obj) {
    obj.resize(m_ptree->size());
    auto obj_iter = obj.begin();
    auto ptree_iter = m_ptree->begin();
    for (; obj_iter != obj.end(); ++obj_iter, ++ptree_iter)
      load_from_ptree(*obj_iter, ptree_iter->second);
  }

  /// For sequences.
  template <typename T>
  typename std::enable_if<traits::is_sequence<T>::value, void>::type load(
      T &obj) {
    obj.clear();
    for (const typename Ptree::value_type &key_value : *m_ptree) {
      typename T::value_type value;
      load_from_ptree(value, key_value.second);
      obj.insert(value);
    }
  }

  /// For sets.
  template <typename T>
  typename std::enable_if<traits::is_set<T>::value, void>::type load(T &obj) {
    obj.clear();
    for (const typename Ptree::value_type &key_value : *m_ptree) {
      typename T::value_type value;
      load_from_ptree(value, key_value.second);
      obj.insert(value);
    }
  }

  /// For maps.
  template <typename T>
  typename std::enable_if<traits::is_map<T>::value, void>::type load(T &obj) {
    obj.clear();
    for (const typename Ptree::value_type &key_value : *m_ptree) {
      const typename T::key_type key =
          boost::lexical_cast<typename T::key_type>(key_value.first);
      typename T::mapped_type value;
      load_from_ptree(value, key_value.second);
      // FIXME should be replaced by emplace in the future
      obj.insert(typename T::value_type(key, value));
    }
  }

  /// For boost::variant.
  template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
  void load(boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &obj);

 private:
  const Ptree *const m_ptree;
};

namespace input_archive_detail {
/// Base declaration.
template <typename... Args>
struct load_variant;

/// Implementation.
template <typename Arg, typename... Args>
struct load_variant<Arg, Args...> {
  /*!
   * \brief Key is unknown.
   *
   * \note This overload is needed
   * for boost::variant implementation
   * that uses boost::detail::variant::void_.
   */
  template <typename Archive, typename Variant>
  static typename std::enable_if<!traits::type_key<Variant, Arg>::call(),
                                 void>::type
  load(const std::string &name, Archive &, Variant &) {
    BOOST_THROW_EXCEPTION(variant_load_invalid_key_error()
                          << variant_load_key_error::key(name)
                          << error::message("Unknown type."));
  }

  /// Check Arg's key.
  template <typename Archive, typename Variant>
  static typename std::enable_if<
      static_cast<bool>(traits::type_key<Variant, Arg>::call()), void>::type
  load(const std::string &name, Archive &ar, Variant &obj) {
    if (name == traits::type_key<Variant, Arg>::call()) {
      Arg arg;
      ar >> arg;
      obj = arg;
    } else {
      load_variant<Args...>::load(name, ar, obj);
    }
  }
};

/// There is no types left.
template <>
struct load_variant<> {
  template <typename Archive, typename Variant>
  static void load(const std::string &name, Archive &, Variant &) {
    BOOST_THROW_EXCEPTION(
        variant_load_invalid_key_error()
        << variant_load_key_error::key(name)
        << error::message("There is no types left for variant."));
  }
};
}  // namespace input_archive_detail

template <typename Ptree>
template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
void input_archive<Ptree>::load(
    boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &obj) {
  if (m_ptree->empty()) BOOST_THROW_EXCEPTION(variant_load_no_key_error());
  if (m_ptree->size() > 1)
    BOOST_THROW_EXCEPTION(variant_load_multiple_keys_error());
  const std::string type_name = m_ptree->front().first;
  input_archive<Ptree> ar(m_ptree->front().second);
  input_archive_detail::load_variant<BOOST_VARIANT_ENUM_PARAMS(T)>::load(
      type_name, ar, obj);
}

}  // namespace config
}  // namespace bunsan
