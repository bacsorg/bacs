#include <bunsan/pm/compatibility/repository.hpp>

#include <boost/python.hpp>

using namespace boost::python;
using namespace bunsan::pm::compatibility;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(repository_overloads, repository::create,
                                       1, 2)

BOOST_PYTHON_MODULE(_pm) {
  class_<repository, boost::noncopyable>("Repository", init<std::string>())
      .def("create", &repository::create, repository_overloads())
      .def("extract", &repository::extract)
      .def("initialize_cache", &repository::initialize_cache)
      .staticmethod("initialize_cache")
      .def("clean_cache", &repository::clean_cache);
}
