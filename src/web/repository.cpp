#include "bacs/archive/web/repository.hpp"

#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>

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

        dispatcher().assign("/has_flag", &repository::has_flag, this);
        mapper().assign("has_flag", "/has_flag");

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
    }

    void repository::insert()
    {
    }

    void repository::extract()
    {
    }

    void repository::rename()
    {
    }

    void repository::existing()
    {
    }

    void repository::available()
    {
    }

    void repository::status()
    {
    }

    void repository::has_flag()
    {
    }

    void repository::set_flags()
    {
    }

    void repository::unset_flags()
    {
    }

    void repository::clear_flags()
    {
    }

    void repository::ignore()
    {
    }

    void repository::info()
    {
    }

    void repository::hash()
    {
    }

    void repository::repack()
    {
    }
}}}
