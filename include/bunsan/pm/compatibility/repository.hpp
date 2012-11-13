#pragma once

#include <string>

#include <boost/noncopyable.hpp>

namespace bunsan{namespace pm
{
    class repository;
}}

namespace bunsan{namespace pm{namespace compatibility
{
    class repository: private boost::noncopyable
    {
    public:
        /// \param config path to configuration file
        explicit repository(const std::string &config);
        void create(const std::string &path, bool strip=false);
        void extract(const std::string &package, const std::string &path);
        void clean();
        ~repository();

    private:
        bunsan::pm::repository *m_repo;
    };
}}}
