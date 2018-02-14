#include <bacs/archive/repository.hpp>

// avoid windows.h, use separate file for asio
#include <boost/asio/io_service.hpp>

namespace bacs {
namespace archive {

void repository::post_import_(const problem::id &id) {
  m_io_service.post([this, id] { import(id); });
}

}  // namespace archive
}  // namespace bacs
