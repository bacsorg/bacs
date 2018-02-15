#pragma once

#include <bacs/problem/single/test/error.hpp>
#include <bacs/problem/single/testing.pb.h>

#include <memory>

namespace bacs::problem::single::test {

struct matcher_error : virtual error {};
struct matcher_not_set_error : virtual matcher_error {};

class matcher {
 public:
  explicit matcher(const TestQuery &query);
  ~matcher();

  bool operator()(const std::string &test_id) const;

 private:
  class impl;
  class id;
  class wildcard;
  class regex;

  std::shared_ptr<const impl> make_query(const TestQuery &query);

 private:
  std::shared_ptr<const impl> m_impl;
};

}  // namespace bacs::problem::single::test
