#include <bunsan/web/mime_file.hpp>

#include <bunsan/filesystem/fstream.hpp>

#include <boost/assert.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace bunsan {
namespace web {

void mime_file::swap(mime_file &m) noexcept {
  using std::swap;

  swap(m_extension2mime_type, m.m_extension2mime_type);
}

namespace {
struct mime_file_line {
  std::string mime_type;
  std::vector<std::string> extensions;

  void setup(mime_file &m) const { m.set_all(mime_type, extensions); }
};
}  // namespace

}  // namespace web
}  // namespace bunsan

BOOST_FUSION_ADAPT_STRUCT(bunsan::web::mime_file_line,
  (std::string, mime_type)
  (std::vector<std::string>, extensions)
)

namespace bunsan {
namespace web {

void mime_file::load(std::istream &in) {
  mime_file data;
  boost::optional<mime_file_line> mime_exts;

  namespace qi = boost::spirit::qi;
  namespace phoenix = boost::phoenix;
  using namespace qi::labels;
  using namespace phoenix;
  using iterator = std::string::const_iterator;

  qi::rule<iterator, boost::optional<mime_file_line>(), qi::blank_type>
      line_parser;
  qi::rule<iterator, mime_file_line(), qi::blank_type> data_parser;
  qi::rule<iterator, void(), qi::blank_type> comment_parser;
  qi::rule<iterator, std::string()> mime_type_parser, extension_parser;
  qi::rule<iterator, std::vector<std::string>(), qi::blank_type>
      extension_list_parser;

  mime_type_parser = +(qi::graph - '#');
  extension_parser = +(qi::graph - '#');
  comment_parser = qi::char_('#') >> *qi::char_;

  extension_list_parser = *extension_parser;
  data_parser = mime_type_parser >> extension_list_parser;
  line_parser = -data_parser >> -comment_parser;

  std::string line;
  std::size_t lineno = 0;
  while (std::getline(in, line)) {
    ++lineno;
    iterator cur = line.begin();
    const iterator end = line.end();
    if (qi::phrase_parse(cur, end, line_parser, qi::blank, mime_exts)) {
      if (cur != end) {
        BOOST_THROW_EXCEPTION(
            mime_file_format_error()
            << mime_file_format_error::row(lineno)
            << mime_file_format_error::column(1 + (cur - line.begin())));
      } else {
        if (mime_exts) mime_exts->setup(data);
        mime_exts.reset();
      }
    } else {
      BOOST_THROW_EXCEPTION(mime_file_format_error()
                            << mime_file_format_error::row(lineno));
    }
  }
  swap(data);
}

void mime_file::load(const boost::filesystem::path &path) {
  mime_file data;
  bunsan::filesystem::ifstream fin(path);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) { data.load(fin); }
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
  fin.close();
  swap(data);
}

std::string mime_file::mime_type_by_extension(
    const std::string &extension) const {
  const auto iter = m_extension2mime_type.find(norm_extension(extension));
  if (iter == m_extension2mime_type.end())
    return fallback_mime_type;
  else
    return iter->second;
}

std::string mime_file::mime_type_by_name(
    const boost::filesystem::path &name) const {
  return mime_type_by_extension(name.extension().string());
}

void mime_file::set(const std::string &mime_type,
                    const std::string &extension) {
  m_extension2mime_type[extension] = mime_type;
}

void mime_file::put(const std::string &mime_type,
                    const std::string &extension) {
  const std::string ext = norm_extension(extension);
  const auto iter = m_extension2mime_type.find(ext);
  if (iter != m_extension2mime_type.end())
    BOOST_THROW_EXCEPTION(
        mime_file_extension_conflict_error()
        << mime_file_extension_conflict_error::mime_type(mime_type)
        << mime_file_extension_conflict_error::extension(ext));
  m_extension2mime_type[extension] = mime_type;
}

const std::string mime_file::fallback_mime_type = "application/octet-stream";

std::string mime_file::norm_extension(const std::string &extension) {
  return !extension.empty() && extension[0] == '.' ? extension.substr(1)
                                                   : extension;
}

mime_file load_mime_file(const boost::filesystem::path &path) {
  mime_file data;
  data.load(path);
  return data;
}

}  // namespace web
}  // namespace bunsan
