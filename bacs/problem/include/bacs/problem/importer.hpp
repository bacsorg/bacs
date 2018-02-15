#pragma once

#include <bacs/problem/id.hpp>
#include <bacs/problem/problem.pb.h>

#include <bunsan/factory_helper.hpp>
#include <bunsan/pm/entry.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bacs::problem {

namespace importer_detail {
struct options {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(problem_dir);
    ar & BOOST_SERIALIZATION_NVP(destination);
    ar & BOOST_SERIALIZATION_NVP(root_package);
    ar & BOOST_SERIALIZATION_NVP(id);
    ar & BOOST_SERIALIZATION_NVP(revision);
  }

  boost::filesystem::path problem_dir;
  boost::filesystem::path destination;
  bunsan::pm::entry root_package;
  problem::id id;
  Revision revision;
};
}  // namespace importer_detail

struct importer_error : virtual error {};
struct importer_convert_error : virtual importer_error {
  using options =
      boost::error_info<struct tag_options, importer_detail::options>;
};
struct problem_format_error : virtual importer_error {
  using problem_format =
      boost::error_info<struct tag_problem_format, std::string>;
};
struct unknown_problem_format_error : virtual problem_format_error {};
struct empty_problem_format_error : virtual problem_format_error {};
struct problem_type_error : virtual problem_format_error {
  using problem_type = boost::error_info<struct tag_problem_type, std::string>;
};
struct unknown_problem_type_error : virtual problem_type_error,
                                    virtual unknown_problem_format_error {};
struct empty_problem_type_error : virtual problem_type_error {};

/*!
 * \brief Imports problems into internal storage.
 *
 * Use bunsan::factory to register problem types.
 */
class importer : private boost::noncopyable {
  BUNSAN_FACTORY_BODY(importer, const boost::property_tree::ptree &config)
 public:
  using options = importer_detail::options;

 public:
  /*!
   * \return importer that converts problems for registered types
   *
   * \param config per-implementation config, e.g.
   *
   * \code{.cpp}
   * bacs/problem/single
   * {
   *     generator
   *     {
   *         type internal0
   *         config
   *         {
   *         }
   *     }
   * }
   *
   * bacs/problem/.../another
   * {
   *     ; another configuration
   * }
   * \endcode
   */
  static importer_ptr instance(const boost::property_tree::ptree &config);

 public:
  /// It is safe to call this function from different threads.
  virtual Problem convert(const options &options_) = 0;

  virtual ~importer();

 protected:
  static std::string get_problem_format(
      const boost::filesystem::path &problem_dir);
  static std::string get_problem_type(
      const boost::filesystem::path &problem_dir);
};
BUNSAN_FACTORY_TYPES(importer)

}  // namespace bacs::problem

namespace boost {
std::string to_string(
    const bacs::problem::importer_convert_error::options &options);
}  // namespace boost
