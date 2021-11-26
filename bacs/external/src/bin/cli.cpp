#include <bacs/external/judge.hpp>

#include <bunsan/application.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/protobuf/initialization.hpp>
#include <bunsan/protobuf/text.hpp>

#include <iostream>
#include <streambuf>

using namespace bunsan::application;

namespace {
class bacs_application : public application {
 public:
  using application::application;

  void initialize_argument_parser(argument_parser &parser) override {
    parser.add_positional("root", 1, value<std::string>(&root)->required())
        .add_positional("method", 1, value<std::string>(&method)->required())
        .add_positional("argument", 1,
                        value<std::string>(&argument)->required())
        .add_options()("source", value<std::string>(&source_path),
                       "Load source from file");
  }

  int main(const variables_map &variables) override {
    bacs::external::Judge judge(root);
    if (method == "send") {
      bacs::external::Submit submit;
      if (variables.count("source")) {
        bunsan::protobuf::text::parse_partial(submit, argument);
        bunsan::filesystem::ifstream fin(source_path);
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
          submit.mutable_source()->mutable_data()->assign(
              std::istreambuf_iterator<char>(fin),
              std::istreambuf_iterator<char>());
        } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
        fin.close();
        bunsan::protobuf::check_initialized(submit);
      } else {
        bunsan::protobuf::text::parse(submit, argument);
      }
      judge.Send(submit).PrintDebugString();
    } else if (method == "rejudge") {
      const auto id =
          bunsan::protobuf::text::parse_make<bacs::external::Submit::Id>(
              argument);
      judge.Rejudge(id).PrintDebugString();
    } else if (method == "fetch_result") {
      const auto id =
          bunsan::protobuf::text::parse_make<bacs::external::SubmitResult::Id>(
              argument);
      judge.FetchResult(id).PrintDebugString();
    } else if (method == "fetch_latest_result") {
      const auto id =
          bunsan::protobuf::text::parse_make<bacs::external::Submit::Id>(
              argument);
      judge.FetchLatestResult(id).PrintDebugString();
    } else if (method == "get_result_revisions") {
      const auto id =
          bunsan::protobuf::text::parse_make<bacs::external::Submit::Id>(
              argument);
      judge.GetResultRevisions(id).PrintDebugString();
    } else {
      std::cerr << "Invalid method = " << method << std::endl;
      return 1;
    }
    return 0;
  }

 private:
  std::string root;
  std::string method;
  std::string argument;
  std::string source_path;
};
}  // namespace

int main(int argc, char *argv[]) {
  bacs_application app(argc, argv);
  app.name("bacs::external");
  return app.exec();
}
