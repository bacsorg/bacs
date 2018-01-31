#pragma once

#include <bunsan/log/keywords/scope.hpp>

#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/core/record.hpp>
#include <boost/log/utility/strictest_lock.hpp>
#include <boost/scope_exit.hpp>
#include <boost/thread/lock_guard.hpp>

namespace bunsan {
namespace log {
namespace sources {

namespace detail {
template <typename T>
class attribute_setter {
 public:
  explicit attribute_setter(boost::log::attribute_set &attrs)
      : m_attrs(attrs), m_tag(m_attrs.end()) {}

  template <typename ArgsT, typename Keyword>
  bool pre(const ArgsT &args, const Keyword &keyword, const T &fallback = {}) {
    m_value = args[keyword | fallback];
    return m_value != fallback;
  }

  template <typename Pair>
  void post(const Pair &ret) {
    if (ret.second) m_tag = ret.first;
  }

  boost::log::attributes::constant<T> attr() const {
    return boost::log::attributes::constant<T>(m_value);
  }

  ~attribute_setter() {
    if (m_tag != m_attrs.end()) m_attrs.erase(m_tag);
  }

 private:
  boost::log::attribute_set &m_attrs;
  boost::log::attribute_set::iterator m_tag;
  T m_value;
};
}  // namespace detail

template <typename BaseT>
class scope_feature : public BaseT {
 public:
  using char_type = typename BaseT::char_type;
  using threading_model = typename BaseT::threading_model;
  using open_record_lock = typename boost::log::strictest_lock<
      boost::lock_guard<threading_model>, typename BaseT::open_record_lock,
      typename BaseT::add_attribute_lock,
      typename BaseT::remove_attribute_lock>::type;

  using BaseT::BaseT;

 protected:
  template <typename ArgsT>
  boost::log::record open_record_unlocked(const ArgsT &args) {
    detail::attribute_setter<std::string> file(BaseT::attributes());
    if (file.pre(args, keywords::file))
      file.post(BaseT::add_attribute_unlocked("File", file.attr()));

    detail::attribute_setter<unsigned> line(BaseT::attributes());
    if (line.pre(args, keywords::line))
      line.post(BaseT::add_attribute_unlocked("Line", line.attr()));

    detail::attribute_setter<std::string> function(BaseT::attributes());
    if (function.pre(args, keywords::function))
      function.post(BaseT::add_attribute_unlocked("Function", function.attr()));

    detail::attribute_setter<std::string> pretty_function(BaseT::attributes());
    if (pretty_function.pre(args, keywords::pretty_function))
      pretty_function.post(BaseT::add_attribute_unlocked(
          "PrettyFunction", pretty_function.attr()));

    return BaseT::open_record_unlocked(args);
  }
};

struct scope : boost::mpl::quote1<scope_feature> {};

}  // namespace sources
}  // namespace log
}  // namespace bunsan
