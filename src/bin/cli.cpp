#include <bunsan/pm/repository.hpp>

#include <bunsan/application.hpp>
#include <bunsan/logging/legacy.hpp>
#include <bunsan/property_tree/info_parser.hpp>

#include <boost/property_tree/ptree.hpp>

#include <exception>
#include <iostream>

using namespace bunsan::application;

namespace
{
    class pm_application: public application
    {
    public:
        using application::application;

        void initialize_argument_parser(argument_parser &parser) override
        {
            application::initialize_argument_parser(parser);
            parser.add_options()
            (
                "config,c",
                value<std::string>(&config_file)->default_value("pm.rc"),
                "Configuration file"
            )
            ("clean", "Clean cache")
            (
                "package,p",
                value<std::string>(&package),
                "Package name"
            )
            (
                "extract,e",
                value<std::string>(&extract_to),
                "Extract package to location"
            )
            (
                "install,i",
                value<std::string>(&install_to),
                "Install package to location"
            )
            (
                "update,u",
                value<std::string>(&update_installation),
                "Update installed package"
            )
            (
                "lifetime,l",
                value<std::time_t>(&lifetime),
                "Only check for update if lifetime "
                "has passed since previous attempt"
            )
            (
                "create,r",
                value<std::string>(&create),
                "Create source package from source"
            )
            (
                "create-recursively,R",
                value<std::string>(&create_recursively),
                "Create source package from source"
            )
            ("initialize,I", "Initialize cache")
            ("strip,s", "Strip source package from excess files.");
        }

        void print_version() override
        {
            std::cerr << bunsan::pm::repository::version() << std::endl;
        }

        int main(const variables_map &variables) override
        {
            DLOG(config parse);
            boost::property_tree::ptree config;
            bunsan::property_tree::read_info(config_file, config);
            if (variables.count("initialize"))
            { // initialize before repository construction
                std::cerr << "Attempt to initialize repository" << std::endl;
                bunsan::pm::repository::initialize_cache(config);
            }
            bunsan::pm::repository repo(config);
            if (variables.count("clean"))
            { // clean before working with packages
                std::cerr << "Attempt to clean repository" << std::endl;
                repo.clean_cache();
            }
            if (variables.count("package"))
            {
                if (variables.count("extract"))
                { // extracting
                    std::cerr << "Attempt to extract \"" << package << "\""
                                 " to \"" << extract_to << "\"" << std::endl;
                    repo.extract(package, extract_to);
                }
                else if (variables.count("install"))
                {
                    repo.install(package, install_to);
                }
                else if (variables.count("update"))
                {
                    if (variables.count("lifetime"))
                        repo.update(package, update_installation, lifetime);
                    else
                        repo.update(package, update_installation);
                }
                else
                { // package info
                    std::cerr << "Package \"" << package << "\"" << std::endl;
                }
            }
            else if (variables.count("create"))
            {
                std::cerr << "Attempt to create source package "
                             "from source \"" << create << "\"" << std::endl;
                repo.create(create, variables.count("strip"));
            }
            else if (variables.count("create-recursively"))
            {
                std::cerr << "Attempt to create source packages recursively "
                             "from source starting from \"" <<
                             create_recursively << "\"" << std::endl;
                repo.create_recursively(
                    create_recursively,
                    variables.count("strip")
                );
            }
            return exit_success;
        }

    private:
        std::string config_file;
        std::string extract_to;
        std::string install_to;
        std::string update_installation;
        std::string package;
        std::string create;
        std::string create_recursively;
        std::time_t lifetime;
    };
}

int main(int argc, char **argv)
{
    pm_application app(argc, argv);
    app.name("bunsan::pm::cli");
    return app.exec();
}
