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
#include <boost/variant/static_visitor.hpp>

#include <chrono>
#include <memory>

namespace bunsan {
namespace config {

template <typename Ptree>
class output_archive {
 public:
  using is_loading = std::integral_constant<bool, false>;
  using is_saving = std::integral_constant<bool, true>;

  constexpr unsigned int get_library_version() { return 0; }

 public:
  explicit output_archive(Ptree &ptree) : m_ptree(&ptree) {}

  template <typename T>
  output_archive &operator&(const T &obj) {
    return *this << obj;
  }

  template <typename T>
  output_archive &operator<<(const T &obj) {
    save(obj);
    return *this;
  }

  template <typename T>
  output_archive &operator<<(const boost::serialization::nvp<T> &nvp) {
    Ptree ptree;
    save_to_ptree(nvp.value(), ptree);
    m_ptree->put_child(nvp.name(), ptree);
    return *this;
  }

  template <typename T>
  output_archive &operator<<(
      const boost::serialization::nvp<boost::optional<T>> &nvp) {
    if (nvp.value())
      *this << boost::serialization::make_nvp(nvp.name(), nvp.value().get());
    return *this;
  }

  template <typename T>
  static void save_to_ptree(const T &obj, Ptree &ptree) {
    output_archive<Ptree> oa(ptree);
    oa << obj;
  }

 private:
  /// For complex types.
  template <typename T>
  typename std::enable_if<traits::is_recursive<T>::value, void>::type save(
      const T &obj) {
    const_cast<T &>(obj)
        .serialize(*this, boost::serialization::version<T>::value);
    // FIXME boost::serialization::serialize_adl(*this, obj,
    // boost::serialization::version<T>::value);
  }

  /// For primitive types.
  template <typename T>
  typename std::enable_if<traits::is_direct_assignable<T>::value, void>::type
  save(const T &obj) {
    m_ptree->put_value(obj);
  }

  /// For Ptree.
  void save(const Ptree &obj) { *m_ptree = obj; }

  /// For boost::filesystem::path.
  void save(const boost::filesystem::path &obj) { save(obj.string()); }

  /// For std::chrono::duration.
  template <typename Rep, typename Period>
  void save(const std::chrono::duration<Rep, Period> &obj) {
    save(obj.count());
  }

  /// For sequences except maps.
  template <typename T>
  typename std::enable_if<traits::is_random_access_sequence<T>::value ||
                              traits::is_sequence<T>::value ||
                              traits::is_set<T>::value,
                          void>::type
  save(const T &obj) {
    m_ptree->clear();
    for (const auto &value : obj) {
      typename Ptree::value_type ptree_value;
      save_to_ptree(value, ptree_value.second);
      m_ptree->push_back(ptree_value);
    }
  }

  /// For maps.
  template <typename T>
  typename std::enable_if<traits::is_map<T>::value, void>::type save(
      const T &obj) {
    m_ptree->clear();
    for (const auto &value : obj) {
      typename Ptree::value_type ptree_value = {
          boost::lexical_cast<std::string>(value.first), Ptree()};
      save_to_ptree(value.second, ptree_value.second);
      m_ptree->push_back(ptree_value);
    }
  }

  /// For boost::variant.
  template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
  void save(const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &obj);

 private:
  Ptree *const m_ptree;
};

namespace output_archive_detail {
template <typename Archive, typename Variant>
class save_visitor : public boost::static_visitor<void> {
 public:
  explicit save_visitor(Archive &archive) : m_archive(&archive) {}
  save_visitor(const save_visitor<Archive, Variant> &) = default;
  save_visitor &operator=(const save_visitor<Archive, Variant> &) = default;

  template <typename T>
  void operator()(const T &obj) const {
    using type_key = traits::type_key<Variant, T>;
    static_assert(type_key::call(), "Undefined type key.");
    *m_archive &boost::serialization::make_nvp(type_key::call(), obj);
  }

 private:
  Archive *m_archive;
};
}  // namespace output_archive_detail

template <typename Ptree>
template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
void output_archive<Ptree>::save(
    const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &obj) {
  using visitor_type = boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>;
  using save_visitor =
      output_archive_detail::save_visitor<output_archive<Ptree>, visitor_type>;
  save_visitor visitor(*this);
  boost::apply_visitor(visitor, obj);
}

}  // namespace config
}  // namespace bunsan
