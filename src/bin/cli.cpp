#include "bacs/archive/repository.hpp"
#include "bacs/archive/pb/problem.pb.h"
#include "bacs/archive/pb/convert.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/property_tree/info_parser.hpp"
#include "bunsan/logging/legacy.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <memory>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

int main(int argc, char **argv)
{
    std::string config_file;
    std::string format;
    std::string input, output;
    try
    {
        boost::program_options::options_description desc(argv[0]);
        desc.add_options()
            ("help,h", "Print this information")
            ("version,V", "Program version")
            ("config,c", boost::program_options::value<std::string>(&config_file))
            ("format,f", boost::program_options::value<std::string>(&format))
            ("input,i", boost::program_options::value<std::string>(&input))
            ("output,o", boost::program_options::value<std::string>(&output))
            ("insert", "Insert problem from directory")
            ("insert_all", "Insert all problems from archive");
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
            std::cerr << "It is too early to announce project version" << std::endl;
            return 1;
        }
        DLOG(config parse);
        boost::property_tree::ptree config;
        bunsan::property_tree::read_info(config_file, config);
        bacs::archive::repository repo(config);
        boost::optional<bacs::archive::archiver_options> archiver_options;
        if (vm.count("format"))
        {
            archiver_options = bacs::archive::archiver_options();
            const std::string::size_type pos = format.find(':');
            archiver_options->type = format.substr(0, pos);
            if (pos != std::string::npos)
                archiver_options->config.put("format", format.substr(pos + 1));
        }
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            std::ostream *out = &std::cout;
            std::unique_ptr<bunsan::filesystem::ofstream> fout;
            if (vm.count("output"))
            {
                fout.reset(new bunsan::filesystem::ofstream(output, std::ios_base::binary));
                out = fout.get();
            }
            if (vm.count("insert_all"))
            {
                const bacs::archive::problem::import_map map =
                    archiver_options ? repo.insert_all(archiver_options.get(), input) : repo.insert_all(input);
                bacs::archive::pb::problem::ImportMap pb_map;
                bacs::archive::pb::convert(map, pb_map);
                *out << pb_map.DebugString() << std::flush;
            }
            else if (vm.count("insert"))
            {
                const boost::filesystem::path path = boost::filesystem::absolute(input);
                const bacs::archive::problem::import_info info = repo.insert(path.filename().string(), path);
                bacs::archive::pb::problem::ImportInfo pb_info;
                bacs::archive::pb::convert(info, pb_info);
                *out << pb_info.DebugString() << std::flush;
            }
            else if (vm.count("extract"))
            {
                // TODO
            }
            else if (vm.count("info"))
            {
                // TODO
            }
            else if (vm.count("hash"))
            {
                // TODO
            }
            if (fout)
                fout->close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
    }
    catch (std::exception &e)
    {
        DLOG(Oops! An exception has occured);
        std::cerr << e.what() << std::endl;
        return 200;
    }
}
