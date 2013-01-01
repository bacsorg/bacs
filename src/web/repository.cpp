#include "bacs/archive/web/repository.hpp"
#include "bacs/archive/web/content/form.hpp"
#include "bacs/archive/web/content/error.hpp"
#include "bacs/archive/pb/convert.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_file.h>

#include <boost/assert.hpp>

namespace bacs{namespace archive{namespace web
{
    repository::repository(cppcms::service &srv,
                           const std::shared_ptr<archive::repository> &repository_):
        cppcms::application(srv), m_repository(repository_)
    {
        dispatcher().assign("/insert", &repository::insert, this);
        mapper().assign("insert", "/insert");

        dispatcher().assign("/extract", &repository::extract, this);
        mapper().assign("extract", "/extract");

        dispatcher().assign("/rename", &repository::rename, this);
        mapper().assign("rename", "/rename");

        dispatcher().assign("/existing", &repository::existing, this);
        mapper().assign("existing", "/existing");

        dispatcher().assign("/available", &repository::available, this);
        mapper().assign("available", "/available");

        dispatcher().assign("/status", &repository::status, this);
        mapper().assign("status", "/status");

        dispatcher().assign("/with_flag", &repository::with_flag, this);
        mapper().assign("with_flag", "/with_flag");

        dispatcher().assign("/set_flags", &repository::set_flags, this);
        mapper().assign("set_flags", "/set_flags");

        dispatcher().assign("/unset_flags", &repository::unset_flags, this);
        mapper().assign("unset_flags", "/unset_flags");

        dispatcher().assign("/clear_flags", &repository::clear_flags, this);
        mapper().assign("clear_flags", "/clear_flags");

        dispatcher().assign("/ignore", &repository::ignore, this);
        mapper().assign("ignore", "/ignore");

        dispatcher().assign("/info", &repository::info, this);
        mapper().assign("info", "/info");

        dispatcher().assign("/hash", &repository::hash, this);
        mapper().assign("hash", "/hash");

        dispatcher().assign("/repack", &repository::repack, this);
        mapper().assign("repack", "/repack");

        mapper().root("/repository");
    }

    template <typename Model, typename Result, typename ProtoResult, bool (repository::*Handler)(Model &, Result &)>
    void repository::handler_wrapper(const std::string &name)
    {
        bool do_render = true;
        Model data;
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                Result result;
                do_render = (this->*Handler)(data, result);
                switch (data.form.response())
                {
                case content::form::base::response_type::html:
                    data.result = result;
                    break;
                case content::form::base::response_type::protobuf:
                    {
                        ProtoResult proto_result;
                        pb::convert(result, proto_result);
                        send_protobuf(proto_result, name + ".pb.data");
                        do_render = false;
                    }
                    break;
                }
            }
            else
            {
                switch (data.form.response())
                {
                case content::form::base::response_type::html:
                    // form::validate() results error messages set for form parts
                    break;
                case content::form::base::response_type::protobuf:
                    {
                        response().status(cppcms::http::response::internal_server_error, "Invalid request");
                        content::error error;
                        error.brief = translate("Invalid request.");
                        error.message = translate("Form was not filled correctly.");
                        render("error", error);
                        do_render = false;
                    }
                    break;
                }
            }
        }
        if (do_render)
            render(name, data);
    }

#define DEFINE_HANDLER(NAME, RESULT, PROTO_RESULT) \
    void repository::NAME() \
    { \
        repository::handler_wrapper<content::NAME, RESULT, PROTO_RESULT, &repository::NAME>(#NAME); \
    } \
    bool repository::NAME(content::NAME &data, RESULT &result)

    DEFINE_HANDLER(insert, problem::import_map, pb::problem::ImportMap)
    {
        const bunsan::tempfile tmpfile = bunsan::tempfile::unique(); // FIXME use specified dir from config
        data.form.archive.value()->save_to(tmpfile.string());
        result = m_repository->insert_all(data.form.config.value(), tmpfile.path());
        return true;
    }

    void repository::extract()
    {
        content::extract data;
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                bunsan::tempfile tmpfile = m_repository->extract_all(data.form.ids.value(), data.form.config.value());
                const std::string filename = "archive." + data.form.config.type.value() +
                                             "." + data.form.config.format.value();
                send_tempfile(std::move(tmpfile), filename);
            }
        }
        render("extract", data);
    }

    /*DEFINE_HANDLER(extract, std::string, std::string)
    {
        bunsan::tempfile tmpfile = m_repository->extract_all(data.form.ids.value(), data.form.config.value());
        const std::string filename = "archive." + data.form.config.type.value() +
                                     "." + data.form.config.format.value();
        send_tempfile(std::move(tmpfile), filename);
        return false;
    }*/

    DEFINE_HANDLER(rename, problem::import_info, pb::problem::ImportInfo)
    {
        result = m_repository->rename(data.form.current.value(), data.form.future.value());
        return true;
    }

    DEFINE_HANDLER(existing, problem::id_set, pb::problem::IdSet)
    {
        const boost::optional<problem::id_set> ids = data.form.ids.value();
        if (ids)
            result = m_repository->existing(ids.get());
        else
            result = m_repository->existing();
        return true;
    }

    DEFINE_HANDLER(available, problem::id_set, pb::problem::IdSet)
    {
        const boost::optional<problem::id_set> ids = data.form.ids.value();
        if (ids)
            result = m_repository->available(ids.get());
        else
            result = m_repository->available();
        return true;
    }

    DEFINE_HANDLER(status, problem::status_map, pb::problem::StatusMap)
    {
        result = m_repository->status_all(data.form.ids.value());
        return true;
    }

    DEFINE_HANDLER(with_flag, problem::id_set, pb::problem::IdSet)
    {
        const boost::optional<problem::id_set> ids = data.form.ids.value();
        if (ids)
            result = m_repository->with_flag(ids.get(), data.form.flag.value());
        else
            result = m_repository->with_flag(data.form.flag.value());
        return true;
    }

    DEFINE_HANDLER(set_flags, problem::id_set, pb::problem::IdSet)
    {
        result = m_repository->set_flags_all(data.form.ids.value(), data.form.flags.value());
        return true;
    }

    DEFINE_HANDLER(unset_flags, problem::id_set, pb::problem::IdSet)
    {
        result = m_repository->unset_flags_all(data.form.ids.value(), data.form.flags.value());
        return true;
    }

    DEFINE_HANDLER(clear_flags, problem::id_set, pb::problem::IdSet)
    {
        result = m_repository->clear_flags_all(data.form.ids.value());
        return true;
    }

    DEFINE_HANDLER(ignore, problem::id_set, pb::problem::IdSet)
    {
        result = m_repository->ignore_all(data.form.ids.value());
        return true;
    }

    DEFINE_HANDLER(info, problem::info_map, pb::problem::InfoMap)
    {
        result = m_repository->info_all(data.form.ids.value());
        return true;
    }

    DEFINE_HANDLER(hash, problem::hash_map, pb::problem::HashMap)
    {
        result = m_repository->hash_all(data.form.ids.value());
        return true;
    }

    DEFINE_HANDLER(repack, problem::import_map, pb::problem::ImportMap)
    {
        result = m_repository->repack_all(data.form.ids.value());
        return true;
    }

    template <typename ProtoBuf>
    void repository::send_protobuf(const ProtoBuf &protobuf, const std::string &filename)
    {
        response().content_type("application/octet-stream");
        BOOST_ASSERT(filename.find('"') == std::string::npos);
        response().set_header("Content-Disposition", "filename=\"" + filename + "\"");
        protobuf.SerializeToOstream(&response().out());
    }

    // FIXME note: these implementations are workarounds, use xsendfile and xsendtempfile instead
    void repository::send_file(const boost::filesystem::path &path, const std::string &filename)
    {
        response().content_type("application/octet-stream");
        BOOST_ASSERT(filename.find('"') == std::string::npos);
        response().set_header("Content-Disposition", "filename=\"" + filename + "\"");
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            // FIXME be careful about large files
            bunsan::filesystem::ifstream fin(path);
            response().out() << fin.rdbuf();
            fin.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
    }

    void repository::send_tempfile(bunsan::tempfile &&tmpfile, const std::string &filename)
    {
        // FIXME be careful about large files
        send_file(tmpfile.path(), filename);
    }
}}}
