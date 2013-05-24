#pragma once

#include "bunsan/pm/cache.hpp"

#include <cppcms/application.h>

#include <boost/filesystem/path.hpp>

#include <memory>

#include <ctime>

namespace bacs{namespace statement_provider{namespace web
{
    class server: public cppcms::application
    {
    public:
        server(cppcms::service &srv,
               const std::shared_ptr<bunsan::pm::cache> &cache);

    private:
        void get_index(std::string referrer, std::string request);
        void get(std::string referrer, std::string request, std::string path);

        /// \return false if instant return is required (response() was initialized)
        bool get_(const std::string &referrer,
                  const std::string &request,
                  bunsan::pm::cache::entry *cache_entry=nullptr,
                  boost::filesystem::path *const index=nullptr);

    private:
        const std::shared_ptr<bunsan::pm::cache> m_cache;
    };
}}}
