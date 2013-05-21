#pragma once

#include "bunsan/pm/repository.hpp"

#include <cppcms/application.h>

#include <boost/filesystem/path.hpp>

#include <memory>

namespace bacs{namespace statement_provider{namespace web
{
    class server: public cppcms::application
    {
    public:
        server(cppcms::service &srv,
               const std::shared_ptr<bunsan::pm::repository> &repository);

    private:
        void get_index(std::string referrer, std::string query);
        void get(std::string referrer, std::string query, std::string path);

    private:
        const std::shared_ptr<bunsan::pm::repository> m_repository;
        const boost::filesystem::path m_server_root;
    };
}}}
