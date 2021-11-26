#include <bunsan/serialization/chrono.hpp>
#include <bunsan/serialization/path.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <sstream>
#include <string>

template <typename T>
void test(const T &d) {
  std::stringstream buf;
  {
    boost::archive::text_oarchive oa(buf);
    oa << d;
  }
  {
    boost::archive::text_iarchive ia(buf);
    T b;
    ia >> b;
    assert(d == b);
  }
}

void test_path(const boost::filesystem::path &p) { test(p); }

int main() {
  for (int i = 0; i < 1000; ++i) {
    test(std::chrono::seconds(rand()));
  }
  test_path("");
  test_path(boost::filesystem::path());
  test_path("ssas/sasas");
  test_path("dnjasdnskdnasd//dsad");
  test_path("//");
  test_path("/sas//sa");
}
