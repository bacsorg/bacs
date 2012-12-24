#include "bacs/archive/web/repository.hpp"

#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        cppcms::service srv(argc, argv);
        srv.applications_pool().mount(
            cppcms::applications_factory<bacs::archive::web::repository>()
        );
        srv.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
