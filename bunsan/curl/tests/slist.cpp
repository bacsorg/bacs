#define BOOST_TEST_MODULE slist
#include <boost/test/unit_test.hpp>

#include <bunsan/curl/detail/slist.hpp>
#include <bunsan/curl/detail/slist_iterator.hpp>
#include <bunsan/curl/detail/string_list.hpp>

BOOST_AUTO_TEST_SUITE(slist)

const char *objs[] = {"hello", "world", "of", "strings"};
constexpr std::size_t objs_size = sizeof(objs) / sizeof(objs[0]);
const auto objs_end = objs + objs_size;

BOOST_AUTO_TEST_CASE(raw) {
  bunsan::curl::detail::slist_ptr list;
  bunsan::curl::detail::slist::append(list, "hello");
  bunsan::curl::detail::slist::append(list, "world");

  {
    auto iter = list.get();
    BOOST_REQUIRE(iter);
    BOOST_CHECK_EQUAL(iter->data, "hello");

    iter = iter->next;
    BOOST_REQUIRE(iter);
    BOOST_CHECK_EQUAL(iter->data, "world");

    iter = iter->next;
    BOOST_REQUIRE(!iter);
  }
}

BOOST_AUTO_TEST_CASE(iterator) {
  bunsan::curl::detail::slist_ptr list;
  for (const char *const str : objs)
    bunsan::curl::detail::slist::append(list, str);

  auto iter = objs;
  const auto range = bunsan::curl::detail::make_slist_range(list);
  for (const char *const str : range) {
    BOOST_CHECK_EQUAL(str, *iter);
    ++iter;
  }
  BOOST_CHECK(iter == objs_end);

  iter = objs;
  const auto srange = bunsan::curl::detail::make_slist_string_range(list);
  for (const std::string &str : srange) {
    BOOST_CHECK_EQUAL(str, *iter);
    ++iter;
  }
  BOOST_CHECK(iter == objs_end);
}

BOOST_AUTO_TEST_CASE(string_list) {
  const auto check = [&](const bunsan::curl::detail::string_list &list) {
    BOOST_CHECK_EQUAL(list.size(), 4);

    auto iter = list.begin();
    BOOST_REQUIRE(iter != list.end());
    BOOST_CHECK_EQUAL(std::string(*iter), "hello");

    ++iter;
    BOOST_REQUIRE(iter != list.end());
    BOOST_CHECK_EQUAL(std::string(*iter), "world");

    ++iter;
    BOOST_REQUIRE(iter != list.end());
    BOOST_CHECK_EQUAL(std::string(*iter), "of");

    ++iter;
    BOOST_REQUIRE(iter != list.end());
    BOOST_CHECK_EQUAL(std::string(*iter), "strings");

    ++iter;
    BOOST_REQUIRE(iter == list.end());
  };

  bunsan::curl::detail::string_list list;
  for (const char *const obj : objs) list.append(obj);

  {
    const char **iter = objs;
    for (const char *const obj : list) {
      BOOST_CHECK_EQUAL(std::string(obj), *iter);
      ++iter;
    }
  }

  check(list);

  bunsan::curl::detail::string_list list2 = list;
  check(list2);

  BOOST_CHECK_NE(list.data(), list2.data());

  const auto ptr = list.data();
  bunsan::curl::detail::string_list list3 = std::move(list);
  check(list3);
  BOOST_CHECK_EQUAL(list3.data(), ptr);
  BOOST_CHECK(list.empty());

  list.assign(objs, objs_end);
  check(list);

  BOOST_CHECK(list == list3);
  BOOST_CHECK(list == list2);

  list.clear();
  BOOST_CHECK(list.empty());
  list2.clear();
  BOOST_CHECK(list2.empty());
  BOOST_CHECK(list == list2);
}

BOOST_AUTO_TEST_SUITE_END()  // slist
