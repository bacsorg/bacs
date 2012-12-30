#include "bacs/archive/web/repository.hpp"
#include "bacs/archive/web/content/form.hpp"

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

    void repository::insert()
    {
        content::insert data;
        data.title = translate("Insert");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                const bunsan::tempfile tmpfile = bunsan::tempfile::unique(); // FIXME use specified dir from config
                data.form.archive.value()->save_to(tmpfile.string());
                data.result = m_repository->insert_all(data.form.config.value(), tmpfile.path());
            }
        }
        render("insert", data);
    }

    void repository::extract()
    {
        content::extract data;
        data.title = translate("Extract problems");
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

    void repository::rename()
    {
        content::rename data;
        data.title = translate("Rename problem");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->rename(data.form.current.value(), data.form.future.value());
            }
        }
        render("rename", data);
    }

    void repository::existing()
    {
        content::existing data;
        data.title = translate("Existing problems");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                const boost::optional<problem::id_set> ids = data.form.ids.value();
                if (ids)
                    data.result = m_repository->existing(ids.get());
                else
                    data.result = m_repository->existing();
            }
        }
        render("existing", data);
    }

    void repository::available()
    {
        content::available data;
        data.title = translate("Available problems");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                const boost::optional<problem::id_set> ids = data.form.ids.value();
                if (ids)
                    data.result = m_repository->available(ids.get());
                else
                    data.result = m_repository->available();
            }
        }
        render("available", data);
    }

    void repository::status()
    {
        content::status data;
        data.title = translate("Get problems status");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->status_all(data.form.ids.value());
            }
        }
        render("status", data);
    }

    void repository::with_flag()
    {
        content::with_flag data;
        data.title = translate("With flag");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                const boost::optional<problem::id_set> ids = data.form.ids.value();
                if (ids)
                    data.result = m_repository->with_flag(ids.get(), data.form.flag.value());
                else
                    data.result = m_repository->with_flag(data.form.flag.value());
            }
        }
        render("with_flag", data);
    }

    void repository::set_flags()
    {
        content::set_flags data;
        data.title = translate("Set flags");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->set_flags_all(data.form.ids.value(), data.form.flags.value());
            }
        }
        render("set_flags", data);
    }

    void repository::unset_flags()
    {
        content::unset_flags data;
        data.title = translate("Unset flags");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->unset_flags_all(data.form.ids.value(), data.form.flags.value());
            }
        }
        render("unset_flags", data);
    }

    void repository::clear_flags()
    {
        content::clear_flags data;
        data.title = translate("Clear problems flags");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->clear_flags_all(data.form.ids.value());
            }
        }
        render("clear_flags", data);
    }

    void repository::ignore()
    {
        content::ignore data;
        data.title = translate("Ignore problems");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->ignore_all(data.form.ids.value());
            }
        }
        render("ignore", data);
    }

    void repository::info()
    {
        content::info data;
        data.title = translate("Get problems info");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->info_all(data.form.ids.value());
            }
        }
        render("info", data);
    }

    void repository::hash()
    {
        content::hash data;
        data.title = translate("Get problems hash");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->hash_all(data.form.ids.value());
            }
        }
        render("hash", data);
    }

    void repository::repack()
    {
        content::repack data;
        data.title = translate("Repack problems");
        if (request().request_method() == "POST")
        {
            data.form.load(context());
            if (data.form.validate())
            {
                data.result = m_repository->repack_all(data.form.ids.value());
            }
        }
        render("repack", data);
    }

    template <typename ProtoBuf>
    void repository::send_protobuf(const ProtoBuf &protobuf, const std::string &filename)
    {
        response().content_type("application/octet-stream");
        BOOST_ASSERT(filename.find('"') == std::string::npos);
        response().set_header("Content-Disposition", "filename=\"" + filename + "\"");
        protobuf.SerializeToOstream(response().out());
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
