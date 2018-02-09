#pragma once

#include <string>
#include <typeinfo>

namespace bunsan::runtime {

std::string demangle(const char *const name);
std::string demangle(const std::string &name);

template <typename T>
std::string type_name(const T &obj) {
  return demangle(typeid(obj).name());
}

}  // namespace bunsan::runtime
