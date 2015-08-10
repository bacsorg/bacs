#pragma once

#include <bacs/archive/problem.hpp>

#include <bacs/archive/web/content/base_form.hpp>

#include <bacs/archive/web/content/form/list_query.hpp>
#include <bacs/archive/web/content/form/upload.hpp>
#include <bacs/archive/web/content/form/download.hpp>
#include <bacs/archive/web/content/form/rename.hpp>
#include <bacs/archive/web/content/form/existing.hpp>
#include <bacs/archive/web/content/form/available.hpp>
#include <bacs/archive/web/content/form/status.hpp>
#include <bacs/archive/web/content/form/with_flag.hpp>
#include <bacs/archive/web/content/form/set_flags.hpp>
#include <bacs/archive/web/content/form/unset_flags.hpp>
// clear_flags
// ignore
// get_import_result
#include <bacs/archive/web/content/form/import.hpp>

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

template <typename T>
struct basic_result : base_result {
  BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(basic_result, data)

  T data;
};

using id_set_result = basic_result<problem::IdSet>;
using import_map_result = basic_result<problem::ImportMap>;
using status_map_result = basic_result<problem::StatusMap>;
using status_result = basic_result<problem::StatusResult>;

using upload = basic_form<form::upload, status_map_result>;
using download = basic_form<form::download, void>;
using rename = basic_form<form::rename, status_result>;
using existing = basic_form<form::existing, id_set_result>;
using available = basic_form<form::available, id_set_result>;
using status = basic_form<form::status, status_map_result>;
using with_flag = basic_form<form::with_flag, id_set_result>;
using set_flags = basic_form<form::set_flags, status_map_result>;
using unset_flags = basic_form<form::unset_flags, status_map_result>;

struct clear_flags : basic_form<form::list_query, status_map_result> {
  clear_flags();
};

struct ignore : basic_form<form::list_query, status_map_result> {
  ignore();
};

struct get_import_result : basic_form<form::list_query, import_map_result> {
  get_import_result();
};

using import = basic_form<form::import, status_map_result>;

}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
