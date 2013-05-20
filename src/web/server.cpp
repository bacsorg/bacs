#include "bacs/statement_provider/web/server.hpp"

#include <cppcms/service.h>

namespace bacs{namespace statement_provider{namespace web
{
    server::server(cppcms::service &srv,
                   const std::shared_ptr<bunsan::pm::repository> &repository):
        cppcms::application(srv),
        m_repository(repository),
        m_server_root(srv.settings().get<std::string>("statement_provider.server_root"))
    {
    }
}}}
