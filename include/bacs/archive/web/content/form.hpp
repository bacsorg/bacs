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
// info
// hash
// repack

#include <bunsan/forward_constructor.hpp>

namespace bacs{namespace archive{namespace web{namespace content
{
    template <typename Form, typename Result>
    struct basic_form: base_form
    {
        BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(basic_form, form)

        Form form;
        boost::optional<Result> result;
    };

    template <typename Form>
    struct basic_form<Form, void>: base_form
    {
        BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(basic_form, form)

        Form form;
    };

    typedef basic_form<form::insert, problem::import_map> insert;
    typedef basic_form<form::extract, void> extract;
    typedef basic_form<form::rename, problem::import_info> rename;
    typedef basic_form<form::existing, problem::id_set> existing;
    typedef basic_form<form::available, problem::id_set> available;

    struct status: basic_form<form::list_query, problem::status_map>
    {
        status();
    };

    typedef basic_form<form::with_flag, problem::id_set> with_flag;
    typedef basic_form<form::set_flags, problem::id_set> set_flags;
    typedef basic_form<form::unset_flags, problem::id_set> unset_flags;

    struct clear_flags: basic_form<form::list_query, problem::id_set>
    {
        clear_flags();
    };

    struct ignore: basic_form<form::list_query, problem::id_set>
    {
        ignore();
    };

    struct info: basic_form<form::list_query, problem::info_map>
    {
        info();
    };

    struct hash: basic_form<form::list_query, problem::hash_map>
    {
        hash();
    };

    struct repack: basic_form<form::list_query, problem::import_map>
    {
        repack();
    };
}}}}
