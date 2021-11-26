#pragma once

#include <bacs/archive/problem.hpp>

#include <cppcms/form.h>

namespace bacs::archive::web::content::form::widgets::problem {

class flag_set : public cppcms::widgets::text {
 public:
  archive::problem::FlagSet value();
  void value(const archive::problem::FlagSet &flag_set_);

  bool validate() override;
};

}  // namespace bacs::archive::web::content::form::widgets::problem
