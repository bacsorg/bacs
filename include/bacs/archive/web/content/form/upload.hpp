#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/archiver_config.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

struct upload : base {
  upload();

  widgets::archiver_config config;
  cppcms::widgets::file archive;
  cppcms::widgets::submit submit;
};

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
