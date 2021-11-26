#pragma once

#include <bacs/archive/problem.hpp>

#include <cppcms/form.h>

namespace bacs::archive::web::content::form::widgets::problem {

class id : public cppcms::widgets::text {
 public:
  archive::problem::id value();
  void value(const archive::problem::id &id_);

  bool validate() override;
};

}  // namespace bacs::archive::web::content::form::widgets::problem
