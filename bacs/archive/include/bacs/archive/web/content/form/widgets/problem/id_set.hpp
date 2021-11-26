#pragma once

#include <bacs/archive/problem.hpp>

#include <cppcms/form.h>

namespace bacs::archive::web::content::form::widgets::problem {

class id_set : public cppcms::widgets::text {
 public:
  archive::problem::IdSet value();
  void value(const archive::problem::IdSet &id_set_);

  bool validate() override;
};

}  // namespace bacs::archive::web::content::form::widgets::problem
