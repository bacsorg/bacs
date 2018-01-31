#include <bunsan/process/native_executor.hpp>

#include <bunsan/process/detail/context.hpp>
#include <bunsan/process/detail/execute.hpp>
#include <bunsan/process/path.hpp>

#include <bunsan/log/trivial.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/tempfile.hpp>

#include <boost/assert.hpp>
#include <boost/io/detail/quoted_manip.hpp>  // for logging
#include <boost/noncopyable.hpp>
#include <boost/variant/static_visitor.hpp>

#include <sstream>  // for logging

namespace bunsan {
namespace process {

namespace {
class file_action_visitor : boost::static_visitor<file::handle>,
                            private boost::noncopyable {
 public:
  using inherit_t = file::handle (*)();
  using open_default_t = file::handle (*)();
  using open_path_t = file::handle (*)(const boost::filesystem::path &path);

  file_action_visitor(const open_default_t open_default,
                      const inherit_t inherit, const open_path_t open_path)
      : m_open_default(open_default),
        m_inherit(inherit),
        m_open_path(open_path) {}

  file::handle operator()(file::do_default_type) { return m_open_default(); }
  file::handle operator()(file::inherit_type) { return m_inherit(); }
  file::handle operator()(file::suppress_type) {
    return file::handle::open_null();
  }
  file::handle operator()(file::redirect_to_stdout_type) {
    return file::handle::std_output();
  }
  file::handle operator()(const boost::filesystem::path &path) {
    return m_open_path(path);
  }
  file::handle operator()(const std::string &data) {
    m_tmp = tempfile::regular_file_in_tempdir();
    bunsan::filesystem::ofstream fout(m_tmp.path());
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
      fout << data;
    } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
    fout.close();
    return (*this)(m_tmp.path());
  }

 private:
  const open_default_t m_open_default;
  const inherit_t m_inherit;
  const open_path_t m_open_path;
  tempfile m_tmp;
};
}  // namespace

int native_executor::sync_execute(context ctx) {
  ctx.build();
  return sync_execute_impl(ctx);
}

int native_executor::sync_execute_impl(const context &ctx) {
  BUNSAN_LOG_TRACE << "Attempt to execute " << ctx.executable() << " in "
                   << ctx.current_path() << (ctx.use_path() ? " " : " without")
                   << " using path";

  detail::context ctx_;

  if (ctx.use_path()) {
    ctx_.executable = find_executable_in_path(ctx.executable());
  } else {
    ctx_.executable = ctx.executable();
  }

  ctx_.current_path = ctx.current_path();
  ctx_.arguments = ctx.arguments();

  file_action_visitor stdin_visitor(&file::handle::open_null,
                                    &file::handle::std_input,
                                    &file::handle::open_read);
  file_action_visitor stdout_visitor(&file::handle::std_output,
                                     &file::handle::std_output,
                                     &file::handle::open_write);
  file_action_visitor stderr_visitor(&file::handle::std_error,
                                     &file::handle::std_error,
                                     &file::handle::open_write);
  ctx_.stdin_file = boost::apply_visitor(stdin_visitor, ctx.stdin_data());
  ctx_.stdout_file = boost::apply_visitor(stdout_visitor, ctx.stdout_data());
  ctx_.stderr_file = boost::apply_visitor(stderr_visitor, ctx.stderr_data());

  {  // begin logging section
    std::ostringstream sout;
    sout << "Executing " << ctx_.executable << " in " << ctx_.current_path
         << " with arguments = [";
    for (std::size_t i = 0; i < ctx_.arguments.size(); ++i) {
      if (i) sout << ", ";
      sout << boost::io::quoted(ctx_.arguments[i]);
    }
    sout << ']';
    BUNSAN_LOG_TRACE << sout.str();
  }  // end logging section

  return detail::sync_execute(std::move(ctx_));
}

}  // namespace process
}  // namespace bunsan
