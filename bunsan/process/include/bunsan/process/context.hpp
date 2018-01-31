#pragma once

#include <bunsan/process/context_forward.hpp>
#include <bunsan/process/file/action.hpp>

#include <bunsan/get.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/operators.hpp>
#include <boost/optional.hpp>

#include <string>
#include <vector>

namespace bunsan {
namespace process {

class context : public boost::equality_comparable<context> {
 public:
  context() = default;
  context(const context &) = default;

  context(context &&ctx) noexcept {
    swap(ctx);
    ctx.reset();
  }

  context &operator=(const context &ctx) {
    context ctx_(ctx);
    swap(ctx_);
    return *this;
  }

  context &operator=(context &&ctx) noexcept {
    swap(ctx);
    ctx.reset();
    return *this;
  }

  bool operator==(const context &ctx) const {
    return m_current_path == ctx.m_current_path &&
           m_executable == ctx.m_executable && m_arguments == ctx.m_arguments &&
           m_use_path == ctx.m_use_path && m_stdin_data == ctx.m_stdin_data &&
           m_stdout_data == ctx.m_stdout_data &&
           m_stderr_data == ctx.m_stderr_data;
  }

  void reset() noexcept {
    m_current_path.reset();
    m_executable.reset();
    m_arguments.clear();
    m_use_path.reset();
    m_stdin_data = file::do_default;
    m_stdout_data = file::do_default;
    m_stderr_data = file::do_default;
  }

  void swap(context &ctx) noexcept {
    using boost::swap;
    swap(m_current_path, ctx.m_current_path);
    swap(m_executable, ctx.m_executable);
    swap(m_arguments, ctx.m_arguments);
    swap(m_use_path, ctx.m_use_path);
    swap(m_stdin_data, ctx.m_stdin_data);
    swap(m_stdout_data, ctx.m_stdout_data);
    swap(m_stderr_data, ctx.m_stderr_data);
  }

  // current path
  context &current_path(const boost::filesystem::path &current_path_) {
    m_current_path = current_path_;
    return *this;
  }
  const boost::filesystem::path &current_path() const {
    return get(m_current_path, "current_path member was not initialized");
  }

  // executable
  context &executable(const boost::filesystem::path &executable_) {
    m_executable = executable_;
    return *this;
  }
  const boost::filesystem::path &executable() const {
    return get(m_executable, "executable member was not initialized");
  }

  // arguments
  context &arguments(std::vector<std::string> &&arguments_) {
    m_arguments = std::move(arguments_);
    return *this;
  }
  context &arguments(std::vector<std::string> &arguments_) {
    m_arguments = arguments_;
    return *this;
  }
  context &arguments(const std::vector<std::string> &arguments_) {
    m_arguments = arguments_;
    return *this;
  }
  template <typename Arg, typename... Args>
  context &arguments(Arg &&arg, Args &&... args) {
    m_arguments.clear();
    arguments_append(std::forward<Arg>(arg), std::forward<Args>(args)...);
    return *this;
  }
  // append
  template <typename Arg1, typename Arg2, typename... Args>
  context &arguments_append(Arg1 &&arg1, Arg2 &&arg2, Args &&... args) {
    arguments_append(std::forward<Arg1>(arg1));
    arguments_append(std::forward<Arg2>(arg2), std::forward<Args>(args)...);
    return *this;
  }
  context &arguments_append(const char *const obj) {
    m_arguments.emplace_back(obj);
    return *this;
  }
  context &arguments_append(std::string &&obj) {
    m_arguments.emplace_back(std::move(obj));
    return *this;
  }
  context &arguments_append(const std::string &obj) {
    m_arguments.emplace_back(obj);
    return *this;
  }
  context &arguments_append(boost::filesystem::path &&obj) {
    arguments_append(obj.string());
    return *this;
  }
  context &arguments_append(const boost::filesystem::path &obj) {
    arguments_append(obj.string());
    return *this;
  }
  template <typename T>
  context &arguments_append(boost::optional<T> &&obj) {
    if (obj) arguments_append(std::move(obj.get()));
    return *this;
  }
  template <typename T>
  context &arguments_append(const boost::optional<T> &obj) {
    if (obj) arguments_append(obj.get());
    return *this;
  }
  template <typename T>
  context &arguments_append(std::vector<T> &&lst) {
    for (T &obj : lst) arguments_append(std::move(obj));
    return *this;
  }
  template <typename T>
  context &arguments_append(const std::vector<T> &lst) {
    for (const T &obj : lst) arguments_append(obj);
    return *this;
  }
  // end of append
  const std::vector<std::string> &arguments() const { return m_arguments; }
  std::vector<std::string> &arguments() { return m_arguments; }

  // use path
  context &use_path(bool use_path_) {
    m_use_path = use_path_;
    return *this;
  }
  bool use_path() const {
    return get(m_use_path, "use_path member was not initialized");
  }

  // stdin data
  context &stdin_inherit() {
    m_stdin_data = file::inherit;
    return *this;
  }
  context &stdin_suppress() {
    m_stdin_data = file::suppress;
    return *this;
  }
  context &stdin_data(const std::string &data) {
    m_stdin_data = data;
    return *this;
  }
  context &stdin_file(const boost::filesystem::path &path) {
    m_stdin_data = path;
    return *this;
  }
  const file::stdin_data_type &stdin_data() const { return m_stdin_data; }

  // stdout data
  context &stdout_inherit() {
    m_stdout_data = file::inherit;
    return *this;
  }
  context &stdout_suppress() {
    m_stdout_data = file::suppress;
    return *this;
  }
  context &stdout_file(const boost::filesystem::path &path) {
    m_stdout_data = path;
    return *this;
  }
  const file::stdout_data_type &stdout_data() const { return m_stdout_data; }

  // stderr data
  context &stderr_inherit() {
    m_stderr_data = file::inherit;
    return *this;
  }
  context &stderr_suppress() {
    m_stderr_data = file::suppress;
    return *this;
  }
  context &stderr_redirect_to_stdout() {
    m_stderr_data = file::redirect_to_stdout;
    return *this;
  }
  context &stderr_file(const boost::filesystem::path &path) {
    m_stderr_data = path;
    return *this;
  }
  const file::stderr_data_type &stderr_data() const { return m_stderr_data; }

  // build functions
  /*!
   * \brief prepare context to execution
   *
   * \post all properties are initialized
   */
  void build() {
    context ctx(*this);
    ctx.build_();
    swap(ctx);
  }

  context built() const {
    context ctx(*this);
    ctx.build_();
    return ctx;
  }

 private:
  void build_();

  friend std::string boost::to_string(const bunsan::process::context &context);

  boost::optional<boost::filesystem::path> m_current_path;
  boost::optional<boost::filesystem::path> m_executable;
  std::vector<std::string> m_arguments;
  boost::optional<bool> m_use_path;
  file::stdin_data_type m_stdin_data = file::do_default;
  file::stdout_data_type m_stdout_data = file::do_default;
  file::stderr_data_type m_stderr_data = file::do_default;
};

inline void swap(context &a, context &b) noexcept { a.swap(b); }

}  // namespace process
}  // namespace bunsan
