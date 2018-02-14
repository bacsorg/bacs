#pragma once

#include <bacs/archive/problem.hpp>

#include <cppcms/form.h>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {
namespace widgets {
namespace problem {

class flag_set : public cppcms::widgets::text {
 public:
  archive::problem::FlagSet value();
  void value(const archive::problem::FlagSet &flag_set_);

  bool validate() override;
};

}  // namespace problem
}  // namespace widgets
}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
