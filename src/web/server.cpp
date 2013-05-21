#include "bacs/statement_provider/web/server.hpp"

#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_response.h>
#include <cppcms/service.h>

namespace bacs{namespace statement_provider{namespace web
{
    server::server(cppcms::service &srv,
                   const std::shared_ptr<bunsan::pm::repository> &repository):
        cppcms::application(srv),
        m_repository(repository),
        m_server_root(srv.settings().get<std::string>("statement_provider.server_root"))
    {
        dispatcher().assign("/([^/]+)/get/([^/]+)", &server::get_index, this, 1, 2);
        mapper().assign("get_index", "/{1}/get/{2}");

        dispatcher().assign("/([^/]+)/get/([^/]+)/(.*)", &server::get, this, 1, 2, 3);
        mapper().assign("get", "/{1}/get/{2}/{3}");

        mapper().root("/statement");
    }

    void server::get_index(std::string referrer, std::string query)
    {
        response().status(cppcms::http::response::temporary_redirect);
        response().set_redirect_header(url("get", referrer, query, ""));
    }

    void server::get(std::string referrer, std::string query, std::string path)
    {
    }
}}}
