#include "bacs/statement_provider/web/server.hpp"

#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include <iostream>
#include <memory>
#include <cstdlib>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

int main(int argc, char *argv[])
{
    try
    {
        cppcms::service srv(argc, argv);
        const std::string cfg = srv.settings().get<std::string>("statement_provider.cache.config");
        boost::property_tree::ptree config;
        boost::property_tree::read_info(cfg, config);
        const std::shared_ptr<bunsan::pm::cache> cache(new bunsan::pm::cache(config));
        srv.applications_pool().mount(
            cppcms::applications_factory<bacs::statement_provider::web::server>(cache)
        );
        srv.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
