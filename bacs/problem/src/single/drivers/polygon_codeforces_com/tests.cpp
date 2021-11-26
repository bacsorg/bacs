#include "tests.hpp"

namespace bacs::problem::single::drivers::polygon_codeforces_com {

tests::tests(const boost::filesystem::path &location)
    : list_storage(location, list_storage::test_data_type::text,
                   "[polygon_codeforces_com tests generator]") {}

void tests::add_test(const std::string &test_id,
                     const boost::filesystem::path &in,
                     const boost::filesystem::path &out) {
  list_storage::add_test(test_id, {{"in", in}, {"out", out}});
}

}  // namespace bacs::problem::single::drivers::polygon_codeforces_com {
