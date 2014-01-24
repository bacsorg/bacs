#include <bunsan/pm/compatibility/repository.hpp>

#include <bunsan/pm/repository.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

bunsan::pm::compatibility::repository::repository(
    const std::string &config)
{
    boost::property_tree::ptree cfg;
    boost::property_tree::read_info(config, cfg);
    m_repo = new bunsan::pm::repository(cfg);
}

void bunsan::pm::compatibility::repository::extract(
    const std::string &package,
    const std::string &destination)
{
    m_repo->extract(package, destination);
}

void bunsan::pm::compatibility::repository::create(
    const std::string &path, bool strip)
{
    m_repo->create(path, strip);
}

void bunsan::pm::compatibility::repository::clean()
{
    m_repo->clean();
}

bunsan::pm::compatibility::repository::~repository()
{
    delete m_repo;
}
