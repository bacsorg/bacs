#pragma once

#include <bunsan/config/input_archive.hpp>
#include <bunsan/config/output_archive.hpp>

namespace bunsan {
namespace config {

template <typename T, typename Ptree>
T load(const Ptree &ptree) {
  T obj;
  input_archive<Ptree> ia(ptree);
  ia >> obj;
  return obj;
}

template <typename Ptree, typename T>
Ptree save(const T &obj) {
  Ptree ptree;
  output_archive<Ptree> oa(ptree);
  oa << obj;
  return ptree;
}

}  // namespace config
}  // namespace bunsan
