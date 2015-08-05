#pragma once

#include <bacs/problem/buildable.hpp>
#include <bacs/problem/problem.pb.h>

namespace bacs {
namespace problem {

class system_verifier : public buildable {
 public:
  explicit system_verifier(const System &system) : m_system(system) {}
  bool make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry &package,
                    const Revision &revision) const override;

 private:
  System m_system;
};

}  // namespace problem
}  // namespace bacs
