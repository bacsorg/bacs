#include <bacs/archive/pb/convert.hpp>
#include <bacs/archive/pb/problem.pb.h>
#include <bacs/archive/repository.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/logging/legacy.hpp>
#include <bunsan/property_tree/info_parser.hpp>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>

int main(int argc, char **argv)
{
    boost::program_options::options_description vdesc(argv[0]);
    std::string config_file;
    std::string insert, insert_all;
    std::string format;
    std::string output;
    std::vector<std::string> arguments;
    try
    {
        vdesc.add_options()
            ("version,V", "Program version")
            ("config,c", boost::program_options::value<std::string>(&config_file))
            ("format,f", boost::program_options::value<std::string>(&format))
            ("output,o", boost::program_options::value<std::string>(&output), "Log file (stdout if not specified)")
            ("insert", boost::program_options::value<std::string>(&insert), "Insert problem from directory")
            ("insert_all", boost::program_options::value<std::string>(&insert_all), "Insert all problems from archive")
            ("repack", "Insert all problems from archive");

        boost::program_options::options_description desc(argv[0]);
        desc.add(vdesc);

        desc.add_options()
            ("argument,a", boost::program_options::value<std::vector<std::string>>(&arguments)->composing(), "arguments");

        boost::program_options::positional_options_description pdesc;
        pdesc.add("argument", -1);

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(pdesc).run(), vm);
        boost::program_options::notify(vm);

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
                    archiver_options ? repo.insert_all(archiver_options.get(), insert_all) : repo.insert_all(insert_all);
                bacs::archive::pb::problem::ImportMap pb_map;
                bacs::archive::pb::convert(map, pb_map);
                *out << pb_map.DebugString() << std::flush;
            }
            else if (vm.count("insert"))
            {
                const boost::filesystem::path path = boost::filesystem::absolute(insert);
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
            else if (vm.count("repack"))
            {
                const bacs::archive::problem::id_set ids(arguments.begin(), arguments.end());
                const bacs::archive::problem::import_map map = repo.repack_all(ids);
                bacs::archive::pb::problem::ImportMap pb_map;
                bacs::archive::pb::convert(map, pb_map);
                *out << pb_map.DebugString() << std::flush;
            }
            if (fout)
                fout->close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
    }
    catch (boost::program_options::error &e)
    {
        std::cerr << e.what() << "\n\n" << vdesc << std::endl;
        return 200;
    }
    catch (std::exception &e)
    {
        DLOG(Oops! An exception has occured);
        std::cerr << e.what() << std::endl;
        return 200;
    }
}
