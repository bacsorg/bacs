#include <bacs/archive/repository.hpp>
#include <bacs/archive/web/repository.hpp>

#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>
#include <memory>

#include <cstdlib>

int main(int argc, char *argv[])
{
    try
    {
        cppcms::service srv(argc, argv);
        const std::string cfg = srv.settings().get<std::string>("repository.config");
        boost::property_tree::ptree config;
        boost::property_tree::read_info(cfg, config);
        const auto repository = std::make_shared<bacs::archive::repository>(config);
        srv.applications_pool().mount(
            cppcms::applications_factory<bacs::archive::web::repository>(repository)
        );
        srv.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
