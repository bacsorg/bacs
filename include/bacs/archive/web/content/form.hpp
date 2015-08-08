#pragma once

#include <bacs/archive/problem.hpp>

#include <bacs/archive/web/content/base_form.hpp>

#include <bacs/archive/web/content/form/list_query.hpp>
#include <bacs/archive/web/content/form/insert.hpp>
#include <bacs/archive/web/content/form/extract.hpp>
#include <bacs/archive/web/content/form/rename.hpp>
#include <bacs/archive/web/content/form/existing.hpp>
#include <bacs/archive/web/content/form/available.hpp>
// status
#include <bacs/archive/web/content/form/with_flag.hpp>
#include <bacs/archive/web/content/form/set_flags.hpp>
#include <bacs/archive/web/content/form/unset_flags.hpp>
// clear_flags
// ignore
// import_result
#include <bacs/archive/web/content/form/repack.hpp>

#include <bunsan/forward_constructor.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {

template <typename Form, typename Result>
struct basic_form : base_form {
  BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(basic_form, form)

  Form form;
  boost::optional<Result> result;
};

template <typename Form>
struct basic_form<Form, void> : base_form {
  BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(basic_form, form)

  Form form;
};

using insert = basic_form<form::insert, problem::StatusMap>;
using extract = basic_form<form::extract, void>;
using rename = basic_form<form::rename, problem::StatusResult>;
using existing = basic_form<form::existing, problem::IdSet>;
using available = basic_form<form::available, problem::IdSet>;

struct status : basic_form<form::list_query, problem::StatusMap> {
  status();
};

using with_flag = basic_form<form::with_flag, problem::IdSet>;
using set_flags = basic_form<form::set_flags, problem::IdSet>;
using unset_flags = basic_form<form::unset_flags, problem::IdSet>;

struct clear_flags : basic_form<form::list_query, problem::IdSet> {
  clear_flags();
};

struct ignore : basic_form<form::list_query, problem::IdSet> {
  ignore();
};

struct import_result : basic_form<form::list_query, problem::ImportMap> {
  import_result();
};

using repack = basic_form<form::repack, problem::StatusMap>;

}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
