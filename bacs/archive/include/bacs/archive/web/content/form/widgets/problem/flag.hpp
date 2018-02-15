#pragma once

#include <bacs/archive/problem.hpp>
#include <bacs/archive/web/content/form/widgets/problem/id.hpp>

#include <cppcms/form.h>

#include <type_traits>

namespace bacs::archive::web::content::form::widgets::problem {

static_assert(std::is_same<archive::problem::id, archive::problem::flag>::value,
              "Internal assert to use typedef here.");
using flag = id;

}  // namespace bacs::archive::web::content::form::widgets::problem
