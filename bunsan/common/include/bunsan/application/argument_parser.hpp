#pragma once

#include <boost/program_options.hpp>

#include <functional>
#include <utility>

namespace bunsan {
namespace application {

using boost::program_options::bool_switch;
using boost::program_options::value;
using boost::program_options::variables_map;

class argument_parser {
 public:
  explicit argument_parser(const std::string &caption)
      : m_visible_description(caption) {}

  boost::program_options::options_description_easy_init add_options() {
    return m_visible_description.add_options();
  }

  argument_parser &add_positional(const char *const name, const int max_count) {
    m_positional_description.add(name, max_count);
    return *this;
  }

  template <typename Arg, typename... Args>
  argument_parser &add_positional(const char *const name, const int max_count,
                                  Arg &&arg, Args &&... args) {
    add_positional(name, max_count);
    m_description.add_options()(name, std::forward<Arg>(arg),
                                std::forward<Args>(args)...);
    return *this;
  }

  template <typename... Args>
  variables_map parse_command_line(Args &&... args) {
    boost::program_options::options_description description = m_description;
    description.add(m_visible_description);
    boost::program_options::variables_map variables_map;
    boost::program_options::store(
        boost::program_options::command_line_parser(std::forward<Args>(args)...)
            .options(description)
            .positional(m_positional_description)
            .run(),
        variables_map);
    boost::program_options::notify(variables_map);
    return variables_map;
  }

  /// \return unspecified-streamable-type
  const boost::program_options::options_description &help() const {
    return m_visible_description;
  }

 private:
  boost::program_options::options_description m_description;
  boost::program_options::options_description m_visible_description;
  boost::program_options::positional_options_description
      m_positional_description;
};

}  // namespace application
}  // namespace bunsan
