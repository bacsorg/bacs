#include <bacs/archive/revision.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace bacs {
namespace archive {

void revision::update() {
  m_uuid = m_generator();
}

std::string revision::get() const {
  return boost::lexical_cast<std::string>(m_uuid);
}

bool revision::is_equal(const std::string &revision) const {
  return get() == revision;
}

}  // namespace archive
}  // namespace bacs
