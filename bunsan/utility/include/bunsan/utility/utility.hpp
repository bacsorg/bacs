#pragma once

#include <boost/property_tree/ptree.hpp>

namespace bunsan::utility {

/*!
 * \brief Abstract class that specifies the way
 * of options transmission + virtual destructor
 */
class utility {
 public:
  /*!
   * \brief Does nothing
   *
   * Utility can have own destructor,
   * we will provide such virtual destructor support
   */
  virtual ~utility();
};

using utility_config = boost::property_tree::ptree;

}  // namespace bunsan::utility
