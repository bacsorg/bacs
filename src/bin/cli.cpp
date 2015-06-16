#include <bacs/archive/pb/convert.hpp>
#include <bacs/archive/pb/problem.pb.h>
#include <bacs/archive/repository.hpp>

#include <bunsan/application.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/logging/trivial.hpp>
#include <bunsan/property_tree/info_parser.hpp>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>

using namespace bunsan::application;

namespace
{
    class archive_application: public application
    {
    public:
        using application::application;

        void initialize_argument_parser(argument_parser &parser) override
        {
            application::initialize_argument_parser(parser);
            parser.add_positional(
                "argument",
                -1,
                value<std::vector<std::string>>(&arguments)
            ).add_options()
            (
                "config,c",
                value<std::string>(&config_file)
            )
            (
                "format,f",
                value<std::string>(&format)
            )
            (
                "insert",
                value<std::string>(&insert),
                "Insert problem from directory"
            )
            (
                "insert_all",
                value<std::string>(&insert_all),
                "Insert all problems from archive"
            )
            (
                "repack",
                "Insert all problems from archive"
            );
        }

        int main(const variables_map &variables) override
        {
            BUNSAN_LOG_DEBUG << "Parsing configuration at \""
                             << config_file << "\"";
            boost::property_tree::ptree config;
            bunsan::property_tree::read_info(config_file, config);
            bacs::archive::repository repo(config);
            boost::optional<bacs::archive::archiver_options> archiver_options;
            if (variables.count("format"))
            {
                archiver_options = bacs::archive::archiver_options();
                const std::string::size_type pos = format.find(':');
                archiver_options->type = format.substr(0, pos);
                if (pos != std::string::npos)
                    archiver_options->config.put("format",
                                                 format.substr(pos + 1));
            }
            if (variables.count("insert_all"))
            {
                BUNSAN_LOG_INFO << "Inserting all problems";
                const bacs::archive::problem::import_map map =
                    archiver_options ?
                        repo.insert_all(archiver_options.get(), insert_all) :
                        repo.insert_all(insert_all);
                bacs::archive::pb::problem::ImportMap pb_map;
                bacs::archive::pb::convert(map, pb_map);
                BUNSAN_LOG_TRACE << pb_map.DebugString();
            }
            else if (variables.count("insert"))
            {
                const boost::filesystem::path path =
                    boost::filesystem::absolute(insert);
                BUNSAN_LOG_INFO << "Inserting " << path;
                const bacs::archive::problem::import_info info =
                    repo.insert(path.filename().string(), path);
                bacs::archive::pb::problem::ImportInfo pb_info;
                bacs::archive::pb::convert(info, pb_info);
                BUNSAN_LOG_TRACE << pb_info.DebugString();
            }
            else if (variables.count("extract"))
            {
                // TODO
                BUNSAN_LOG_WARNING << "Extract is not implemented!";
                return exit_failure;
            }
            else if (variables.count("info"))
            {
                // TODO
                BUNSAN_LOG_WARNING << "Info is not implemented!";
                return exit_failure;
            }
            else if (variables.count("hash"))
            {
                // TODO
                BUNSAN_LOG_WARNING << "Hash is not implemented!";
                return exit_failure;
            }
            else if (variables.count("repack"))
            {
                BUNSAN_LOG_INFO << "Repacking";
                const bacs::archive::problem::id_set ids(
                    arguments.begin(),
                    arguments.end()
                );
                const bacs::archive::problem::import_map map =
                    repo.repack_all(ids);
                bacs::archive::pb::problem::ImportMap pb_map;
                bacs::archive::pb::convert(map, pb_map);
                BUNSAN_LOG_TRACE << pb_map.DebugString();
            }
            return exit_success;
        }

    private:
        std::string config_file;
        std::string insert, insert_all;
        std::string format;
        std::vector<std::string> arguments;
    };
}

int main(int argc, char *argv[])
{
    archive_application app(argc, argv);
    app.name("bacs::archive::cli");
    return app.exec();
}
