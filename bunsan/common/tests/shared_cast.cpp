#define BOOST_TEST_MODULE shared_cast
#include <boost/test/unit_test.hpp>

#include <bunsan/shared_cast.hpp>

#include <turtle/mock.hpp>

BOOST_AUTO_TEST_SUITE(shared_cast)

MOCK_CLASS(mock_class) {
  MOCK_CONSTRUCTOR(mock_class, 0, (), constructor)
  MOCK_DESTRUCTOR(~mock_class, destructor)
};

BOOST_AUTO_TEST_CASE(std_to_boost) {
  MOCK_EXPECT(mock_class::constructor).once();
  const std::shared_ptr<mock_class> std_ptr = std::make_shared<mock_class>();
  MOCK_EXPECT(std_ptr->destructor).once();
  const boost::shared_ptr<mock_class> boost_ptr = bunsan::shared_cast(std_ptr);
  BOOST_CHECK_EQUAL(std_ptr.get(), boost_ptr.get());
}

BOOST_AUTO_TEST_CASE(boost_to_std) {
  MOCK_EXPECT(mock_class::constructor).once();
  const boost::shared_ptr<mock_class> boost_ptr =
      boost::make_shared<mock_class>();
  MOCK_EXPECT(boost_ptr->destructor).once();
  const std::shared_ptr<mock_class> std_ptr = bunsan::shared_cast(boost_ptr);
  BOOST_CHECK_EQUAL(std_ptr.get(), boost_ptr.get());
}

BOOST_AUTO_TEST_SUITE_END()  // shared_cast
