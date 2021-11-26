#include <bacs/problem/system_verifier.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/protobuf/binary.hpp>

namespace bacs::problem {

bool system_verifier::make_package(const boost::filesystem::path &destination,
                                   const bunsan::pm::entry & /*package*/,
                                   const Revision & /*revision*/) const {
  bunsan::filesystem::reset_dir(destination);
  bunsan::pm::index index;
  index.source.import.source.push_back({".", "bacs/system/system_verifier"});
  index.source.self.push_back({".", "src"});
  boost::filesystem::create_directories(destination / "src" / "src");
  bunsan::filesystem::ofstream fout(destination / "src" / "src" /
                                    "system_verifier.cpp");
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
    fout << "#include <bacs/system/system_verifier.hpp>\n"
         << "\n"
         << "#include <bunsan/protobuf/binary.hpp>\n"
         << "\n"
         << "namespace bacs {\n"
         << "namespace system {\n"
         << "\n"
         << "class proto_system_verifier : public system_verifier {\n"
         << " public:\n"
         << "  proto_system_verifier();\n"
         << "  bool verify(const problem::System &request,\n"
         << "              problem::SystemResult &result) override;\n"
         << "  static system_verifier_uptr make_instance() {\n"
         << "    return std::make_unique<proto_system_verifier>();\n"
         << "  }\n"
         << "\n"
         << " private:\n"
         << "   const problem::System m_system;\n"
         << "   static const std::string system_encoded;\n"
         << "};\n"
         << "\n"
         << "BUNSAN_PLUGIN_AUTO_REGISTER(\n"
         << "    system_verifier, proto_system_verifier,\n"
         << "    proto_system_verifier::make_instance)\n"
         << "\n"
         << "proto_system_verifier::proto_system_verifier()\n"
         << "    : m_system(bunsan::protobuf::binary::parse_make<\n"
         << "          problem::System>(system_encoded)) {}\n"
         << "\n"
         << "bool proto_system_verifier::verify(\n"
         << "    const problem::System &request,\n"
         << "    problem::SystemResult &result) {\n"
         << "  if (request.revision().value() !=\n"
         << "      m_system.revision().value()) {\n"
         << "    result.set_status(problem::SystemResult::INVALID_REVISION);\n"
         << "    return false;\n"
         << "  }\n"
         << "  return true;\n"
         << "}\n"
         << "\n"
         << "const std::string proto_system_verifier::system_encoded = {\n";
    const std::string system_encoded =
        bunsan::protobuf::binary::to_string(m_system);
    for (const int c : system_encoded) {
      fout << "  " << c << ",\n";
    }
    fout << "};\n"
         << "\n"
         << "\n"
         << "}  // namespace system\n"
         << "}  // namespace bacs\n";
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
  fout.close();
  index.save(destination / "index");
  return true;
}

}  // namespace bacs::problem
