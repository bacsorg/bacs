#define BOOST_TEST_MODULE forward_constructor
#include <boost/test/unit_test.hpp>

#include <bunsan/forward_constructor.hpp>

BOOST_AUTO_TEST_SUITE(forward_constructor)

enum noexcept_true_type { noexcept_true };

enum noexcept_false_type { noexcept_false };

BOOST_AUTO_TEST_SUITE(default_)

class parent {
 public:
  parent() = default;
  parent(const parent &) = default;
  parent(parent &&) = default;
  parent &operator=(const parent &) = default;
  parent &operator=(parent &&) = default;
  explicit parent(noexcept_true_type) noexcept(true) {}
  explicit parent(noexcept_false_type) noexcept(false) {}
};

class child : public parent {
 public:
  BUNSAN_INHERIT_EXPLICIT_CONSTRUCTOR(child, parent)
};

BOOST_AUTO_TEST_CASE(tests) {
  parent p, pr;
  static_assert(noexcept(parent()), "");
  static_assert(noexcept(parent(p)), "");
  static_assert(noexcept(parent(std::move(p))), "");
  static_assert(noexcept(p = pr), "");
  static_assert(noexcept(p = std::move(pr)), "");
  static_assert(noexcept(parent(noexcept_true)), "");
  static_assert(!noexcept(parent(noexcept_false)), "");
  child c, cr;
  static_assert(noexcept(child()), "");
  static_assert(noexcept(child(c)), "");
  static_assert(noexcept(child(std::move(c))), "");
  static_assert(noexcept(c = cr), "");
  static_assert(noexcept(c = std::move(cr)), "");
  static_assert(noexcept(child(noexcept_true)), "");
  static_assert(!noexcept(child(noexcept_false)), "");
}

BOOST_AUTO_TEST_SUITE_END()  // default_

BOOST_AUTO_TEST_SUITE(non_default)

class parent {
 public:
  parent() {}
  parent(const parent &) {}
  parent(parent &&) {}
  parent &operator=(const parent &) { return *this; }
  parent &operator=(parent &&) { return *this; }
  explicit parent(noexcept_true_type) noexcept(true) {}
  explicit parent(noexcept_false_type) noexcept(false) {}
};

class child : public parent {
 public:
  BUNSAN_INHERIT_EXPLICIT_CONSTRUCTOR(child, parent)
};

BOOST_AUTO_TEST_CASE(tests) {
  parent p, pr;
  static_assert(!noexcept(parent()), "");
  static_assert(!noexcept(parent(p)), "");
  static_assert(!noexcept(parent(std::move(p))), "");
  static_assert(!noexcept(p = pr), "");
  static_assert(!noexcept(p = std::move(pr)), "");
  static_assert(noexcept(parent(noexcept_true)), "");
  static_assert(!noexcept(parent(noexcept_false)), "");
  child c, cr;
  static_assert(!noexcept(child()), "");
  static_assert(!noexcept(child(c)), "");
  static_assert(!noexcept(child(std::move(c))), "");
  static_assert(!noexcept(c = cr), "");
  static_assert(!noexcept(c = std::move(cr)), "");
  static_assert(noexcept(child(noexcept_true)), "");
  static_assert(!noexcept(child(noexcept_false)), "");
}

BOOST_AUTO_TEST_SUITE_END()  // non_default

BOOST_AUTO_TEST_SUITE(member)

class member {
 public:
  explicit member(noexcept_true_type) noexcept(true) {}
  explicit member(noexcept_false_type) noexcept(false) {}
};

class cls {
 public:
  BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(cls, m_member)

 private:
  member m_member;
};

BOOST_AUTO_TEST_CASE(test) {
  static_assert(noexcept(member(noexcept_true)), "");
  static_assert(!noexcept(member(noexcept_false)), "");
  static_assert(!noexcept(cls(noexcept_true)), "");
  static_assert(!noexcept(cls(noexcept_false)), "");
}

BOOST_AUTO_TEST_SUITE_END()  // member

BOOST_AUTO_TEST_SUITE(protected_)

class parent {
 public:
  parent() = default;

 protected:
  explicit parent(noexcept_true_type) noexcept(true) {}
  explicit parent(noexcept_false_type) noexcept(false) {}
};

class child : public parent {
 public:
  BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(child, parent)
};

BOOST_AUTO_TEST_CASE(tests) {
  parent p, pr;
  static_assert(noexcept(parent()), "");
  child c, cr;
  static_assert(!noexcept(child(noexcept_true)), "");
  static_assert(!noexcept(child(noexcept_false)), "");
}

BOOST_AUTO_TEST_SUITE_END()  // protected_

BOOST_AUTO_TEST_SUITE_END()  // forward_constructor
