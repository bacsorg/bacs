#pragma once

#include <boost/uuid/random_generator.hpp>

#include <string>

namespace bacs {
namespace archive {

class revision: private boost::noncopyable {
 public:
  revision() { update(); }

  void update();

  std::string get() const;

  bool is_equal(const std::string &revision) const;

 private:
  boost::uuids::basic_random_generator<boost::mt19937> m_generator;
  boost::uuids::uuid m_uuid;
};

}  // namespace archive
}  // namespace bacs
