#include <bunsan/pm/repository.hpp>

#include <bunsan/logging/legacy.hpp>
#include <bunsan/property_tree/info_parser.hpp>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
    std::string config_file;
    std::string extract_to;
    std::string install_to;
    std::string update_installation;
    std::string package;
    std::string create;
    std::string create_recursively;
    std::time_t lifetime;
    try
    {
        //command line parse
        boost::program_options::options_description desc(argv[0]);
        desc.add_options()
            ("help,h", "Print this information")
            ("version,V", "Program version")
            ("config,c", boost::program_options::value<std::string>(&config_file)->default_value("pm.rc"), "Configuration file")
            ("clean", "Clean repository")
            ("package,p", boost::program_options::value<std::string>(&package), "Package name")
            ("extract,e", boost::program_options::value<std::string>(&extract_to), "Extract package to location")
            ("install,i", boost::program_options::value<std::string>(&install_to), "Install package to location")
            ("update,u", boost::program_options::value<std::string>(&update_installation), "Update installed package")
            ("lifetime,l", boost::program_options::value<std::time_t>(&lifetime), "Only check for update if lifetime has passed since previous attempt")
            ("create,r", boost::program_options::value<std::string>(&create), "Create source package from source")
            ("create-recursively,R", boost::program_options::value<std::string>(&create_recursively), "Create source package from source")
            ("strip,s", "Strip source package from excess files.");
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
        if (vm.count("help"))
        {
            std::cerr << desc << std::endl;
            return 1;
        }
        if (vm.count("version"))
        {
            std::cerr << bunsan::pm::repository::version() << std::endl;
            return 0;
        }
        DLOG(config parse);
        boost::property_tree::ptree config;
        bunsan::property_tree::read_info(config_file, config);
        bunsan::pm::repository repo(config);
        if (vm.count("clean"))
        {
            std::cerr << "Attempt to clean repository" << std::endl;
            repo.clean();
        }
        else if (vm.count("package"))
        {
            if (vm.count("extract"))
            {//extracting
                std::cerr << "Attempt to extract \"" << package << "\" to \"" << extract_to << "\"" << std::endl;
                repo.extract(package, extract_to);
            }
            else if (vm.count("install"))
            {
                repo.install(package, install_to);
            }
            else if (vm.count("update"))
            {
                if (vm.count("lifetime"))
                    repo.update(package, update_installation, lifetime);
                else
                    repo.update(package, update_installation);
            }
            else
            {//package info
                std::cerr << "Package \"" << package << "\"" << std::endl;
            }
        }
        else if (vm.count("create"))
        {
            std::cerr << "Attempt to create source package from source \"" << create << "\"" << std::endl;
            repo.create(create, vm.count("strip"));
        }
        else if (vm.count("create-recursively"))
        {
            std::cerr << "Attempt to create source packages recursively from source starting from \"" << create_recursively << "\"" << std::endl;
            repo.create_recursively(create_recursively, vm.count("strip"));
        }
        //end parse
    }
    catch (std::exception &e)
    {
        DLOG(Oops! An exception has occured);
        std::cerr << e.what() << std::endl;
        return 200;
    }
}
