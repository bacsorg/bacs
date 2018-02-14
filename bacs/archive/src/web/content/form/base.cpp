#include <bacs/archive/web/content/form/base.hpp>

#include <boost/lexical_cast.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

base::base() {
  response_.name("response");
  response_.message(cppcms::locale::translate("Response"));
  response_.add(cppcms::locale::translate("HTML"),
                boost::lexical_cast<std::string>(response_type::html));
  response_.add(cppcms::locale::translate("Protocol Buffer"),
                boost::lexical_cast<std::string>(response_type::protobuf));
  response_.selected_id(boost::lexical_cast<std::string>(response_type::html));
  response_.non_empty();
  add(response_);
}

base::response_type base::response() {
  return boost::lexical_cast<response_type>(response_.selected_id());
}

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
