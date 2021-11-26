#pragma once

#include <bacs/problem/single/test/list_storage.hpp>

namespace bacs::problem::single::drivers::polygon_codeforces_com {

class tests : public test::list_storage {
 public:
  explicit tests(const boost::filesystem::path &location);

  void add_test(const std::string &test_id, const boost::filesystem::path &in,
                const boost::filesystem::path &out);
};

}  // namespace bacs::problem::single::drivers::polygon_codeforces_com
