#pragma once

#include <bacs/archive/archive.pb.h>
#include <bacs/archive/config.hpp>
#include <bacs/archive/importer.hpp>
#include <bacs/archive/problem.hpp>
#include <bacs/archive/revision.hpp>

#include <bunsan/interprocess/sync/file_guard.hpp>
#include <bunsan/pm/repository.hpp>
#include <bunsan/tempfile.hpp>
#include <bunsan/utility/archiver.hpp>
#include <bunsan/utility/custom_resolver.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/version.hpp>

#include <functional>
#include <string>

namespace boost {
namespace asio {
#if BOOST_VERSION < 106600
class io_service;
#else
class io_context;
using io_service = io_context;
#endif
}  // namespace asio
}  // namespace boost

namespace bacs {
namespace archive {

/*!
 * \brief Problems repository.
 *
 * Member functions has lock-related documentation.
 * User should not rely on it, it may change in the future.
 *
 * \warning User should not create multiple instances bind to single repository
 * (nor in process scope, nor in thread scope). Exception will be thrown.
 *
 * *_all() functions are wrappers for their counterparts without "_all" suffix.
 * These functions return maps, with values returned by functions counterparts.
 *
 * There are three kinds of locks:
 *     - exclusive-lock -- single operation at time,
 *                         no other locks (except lock-free)
 *     - shared-lock -- multiple operations at time, no exclusive locks
 *     - lock-free -- multiple operations at time
 */
class repository : private boost::noncopyable {
 public:
  explicit repository(boost::asio::io_service &io_service,
                      const config &config_);
  explicit repository(boost::asio::io_service &io_service,
                      const boost::property_tree::ptree &tree);

  explicit repository(const config &config_);
  explicit repository(const boost::property_tree::ptree &tree);

  /* container */
  /*!
   * \brief Upload problems from archive into repository.
   *
   * \return import information
   *
   * Not atomic.
   *
   * \see repository::upload
   */
  problem::StatusMap upload_all(const boost::filesystem::path &location);

  /*!
   * \copydoc upload_all()
   */
  problem::StatusMap upload_all(const archiver_options &archiver_options_,
                                const boost::filesystem::path &archive);

  /*!
   * \brief Upload particular problem into repository.
   *
   * \param location -- directory with problem
   *
   * If conversion is successful, problem flags are reset and data is updated.
   * Otherwise does nothing.
   *
   * Atomic; exclusive-lock on success, lock-free otherwise.
   *
   * \see repository::reset_flags
   */
  problem::StatusResult upload(const problem::id &id,
                               const boost::filesystem::path &location);

  /// repository::upload(location.filename().string(), location)
  problem::StatusResult upload(const boost::filesystem::path &location);

  /*!
   * \brief Download problems from repository.
   *
   * Ignores nonexistent problems.
   *
   * Not atomic.
   *
   * \see repository::exists
   * \see repository::download
   */
  void download_all(const problem::IdSet &id_set,
                    const boost::filesystem::path &location);

  /*!
   * \copydoc download_all()
   *
   * \return archive with problems
   */
  void download_all(const problem::IdSet &id_set,
                    const archiver_options &archiver_options_,
                    const boost::filesystem::path &archive);

  /*!
   * \copydoc download_all()
   */
  bunsan::tempfile download_all(const problem::IdSet &id_set,
                                const archiver_options &archiver_options_);

  /*!
   * \brief Download problem from repository.
   *
   * \return false if problem does not exist
   *
   * Atomic, shared-lock.
   *
   * \see repository::exists
   */
  bool download(const problem::id &id, const boost::filesystem::path &location);

  /*!
   * \brief Change problem id and import.
   *
   * If problem with id=future exists or import fails functions does nothing.
   *
   * Atomic, shared-lock, exclusive-lock.
   *
   * \todo full locking documentation
   *
   * \see repository::import
   */
  problem::StatusResult rename(const problem::id &current,
                               const problem::id &future);

  /*!
   * \brief Get all existing problems.
   *
   * Not atomic, lock-free.
   *
   * \see repository::exists
   */
  problem::IdSet existing();

  /*!
   * \brief Check problem for existence.
   *
   * Atomic, lock-free.
   *
   * \see repository::existing
   */
  bool exists(const problem::id &id);

  /*!
   * \brief Get existing problems from list.
   *
   * Not atomic.
   *
   * \see repository::exists
   */
  problem::IdSet existing(const problem::IdSet &id_set);

  /*!
   * \brief Check if problem is locked or read only.
   *
   * Not atomic, lock-free.
   */
  bool is_locked(const problem::id &id);

  /*!
   * \brief Check if problem is read only.
   *
   * Atomic, lock-free.
   */
  bool is_read_only(const problem::id &id);

  /* flags */
  /*!
   * \brief Get problem status.
   *
   * \return error if problem does not exist
   *
   * Atomic, lock-free.
   */
  problem::StatusResult status(const problem::id &id);

  /*!
   * \brief Get problems status.
   *
   * Not atomic.
   *
   * \see repository::status
   */
  problem::StatusMap status_all(const problem::IdSet &id_set);

  /*!
   * \brief Get all problems status.
   *
   * Not atomic.
   *
   * \see repository::status_all
   */
  problem::StatusMap status_all();

  /*!
   * \brief Cached version of status_all().
   *
   * If revision is unchanged returns CachedStatusMap with only revision set.
   * If revision is changed triggers status_all() execution.
   */
  CachedStatusMap status_all_if_changed(const ArchiveRevision &revision);

  /*!
   * \brief Check problem for flag.
   *
   * \return false if problem does not exist
   * or problem is not marked by flag
   *
   * Atomic, lock-free.
   */
  bool has_flag(const problem::id &id, const problem::flag &flag);

  template <typename Flag>
  decltype(auto) has_flag(const problem::id &id, const Flag &flag) {
    return has_flag(id, problem::flag_to_string(flag));
  }

  /*!
   * \brief Return problems with specified flag.
   *
   * Not atomic.
   *
   * \see repository::has_flag
   */
  problem::IdSet with_flag(const problem::IdSet &id_set,
                           const problem::flag &flag);

  template <typename Flag>
  decltype(auto) with_flag(const problem::IdSet &id_set, const Flag &flag) {
    return with_flag(id_set, problem::flag_to_string(flag));
  }

  /*!
   * \brief Set of problems with flag.
   *
   * Not atomic.
   *
   * \see repository::with_flag
   */
  problem::IdSet with_flag(const problem::flag &flag);

  template <typename Flag>
  decltype(auto) with_flag(const Flag &flag) {
    return with_flag(problem::flag_to_string(flag));
  }

  /*!
   * \brief Set problem flag.
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::is_read_only
   */
  problem::StatusResult set_flag(const problem::id &id,
                                 const problem::flag &flag);

  template <typename Flag>
  decltype(auto) set_flag(const problem::id &id, const Flag &flag) {
    return set_flag(id, problem::flag_to_string(flag));
  }

  /*!
   * \brief Set problem flags.
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::is_read_only
   */
  problem::StatusResult set_flags(const problem::id &id,
                                  const problem::FlagSet &flags);

  /*!
   * \brief Set problems flags.
   *
   * Not atomic.
   *
   * \see repository::set_flags
   */
  problem::StatusMap set_flags_all(const problem::IdSet &id_set,
                                   const problem::FlagSet &flags);

  /*!
   * \brief Unset problem flag.
   *
   * \return false if problem does not exist or is_read_only
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::is_read_only
   */
  problem::StatusResult unset_flag(const problem::id &id,
                                   const problem::flag &flag);

  template <typename Flag>
  decltype(auto) unset_flag(const problem::id &id, const Flag &flag) {
    return unset_flag(id, problem::flag_to_string(flag));
  }

  /*!
   * \brief Unset problem flags.
   *
   * \note Since problem::flags::ignore may not be removed,
   * if flag == problem::flags::ignore this call is equivalent
   * to atomic repository::exists(id) && !repository::is_read_only(id) call.
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::is_read_only
   */
  problem::StatusResult unset_flags(const problem::id &id,
                                    const problem::FlagSet &flags);

  /*!
   * \brief Unset problems flags.
   *
   * Not atomic.
   *
   * \see repository::unset_flags
   */
  problem::StatusMap unset_flags_all(const problem::IdSet &id_set,
                                     const problem::FlagSet &flags);

  /*!
   * \brief Clear problem flags.
   *
   * \return false if problem does not exist or is_read_only
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::is_read_only
   */
  problem::StatusResult clear_flags(const problem::id &id);

  /*!
   * \brief Clear problems flags.
   *
   * \return Set of cleared problems.
   *
   * Not atomic.
   *
   * \see repository::clear_flags
   */
  problem::StatusMap clear_flags_all(const problem::IdSet &id_set);

  /* import result */
  /*!
   * \brief Get problem import result.
   *
   * \return error if problem does not exist
   *
   * \see repository::exists
   */
  problem::ImportResult import_result(const problem::id &id);

  /*!
   * \brief Get problems import result.
   *
   * \see repository::exists
   * \see repository::import_result
   */
  problem::ImportMap import_result_all(const problem::IdSet &id_set);

  /* import */
  /*!
   * \brief Import problem.
   *
   * Importing is similar to upload
   * except using internal copy of problem.
   *
   * Can be useful if problem conversion utility is changed
   * or index is corrupted.
   *
   * If import is not successful problem is marked by problem::flag::ignore.
   * Otherwise, problem::flag::ignore is unset.
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::set_flag
   */
  problem::StatusResult import(const problem::id &id);

  /*!
   * \brief Import all given problems.
   *
   * Not atomic.
   *
   * \see repository::import
   */
  problem::StatusMap import_all(const problem::IdSet &id_set);

  /*!
   * \brief Import all problems.
   *
   * Not atomic.
   *
   * \see repository::import_all
   */
  problem::StatusMap import_all();

  /*!
   * \brief Schedules import for future execution with post.
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::import
   */
  problem::StatusResult schedule_import(const problem::id &id);

  /*!
   * \brief Schedules import for all given problems.
   *
   * Not atomic.
   *
   * \see repository::schedule_import
   */
  problem::StatusMap schedule_import_all(const problem::IdSet &id_set);

  /*!
   * \brief Schedules import for all problems.
   *
   * Not atomic.
   *
   * \see repository::schedule_import_all
   */
  problem::StatusMap schedule_import_all();

  /*!
   * \brief Schedules import for all pending problems.
   *
   * Not atomic.
   *
   * \see repository::schedule_import_all
   */
  problem::StatusMap schedule_import_all_pending();

 private:
  /*!
   * \brief Check problem for validity.
   *
   * \return true if problem exists and problem index has valid format
   *
   * Check basic format conformance, it does not check
   * whether problem is importable
   *
   * Atomic, shared-lock.
   *
   * \see repository::exists
   */
  bool valid(const problem::id &id);

  /* not synchronized function versions for internal usage */

  /// \warning requires at least shared lock
  problem::IdSet existing_();

  /// \warning requires at least shared lock
  problem::StatusMap status_all_(const problem::IdSet &id_set);

  /// \warning requires at least shared lock and problem existence
  void download_(const problem::id &id, const boost::filesystem::path &location);

  /// \warning requires at least shared lock
  problem::StatusResult status_result_(const problem::id &id);

  /// \warning requires at least shared lock and problem existence
  problem::Status status_(const problem::id &id);

  /// \warning requires at least shared lock and problem existence
  problem::FlagSet flags_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  void set_flag_(const problem::id &id, const problem::flag &flag);

  template <typename Flag>
  decltype(auto) set_flag_(const problem::id &id, const Flag &flag) {
    return set_flag_(id, problem::flag_to_string(flag));
  }

  /// \warning requires unique lock and problem existence
  void unset_flag_(const problem::id &id, const problem::flag &flag);

  template <typename Flag>
  decltype(auto) unset_flag_(const problem::id &id, const Flag &flag) {
    return unset_flag_(id, problem::flag_to_string(flag));
  }

  /// \warning requires at least shared lock and problem existence
  problem::Revision read_revision_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  void write_revision_(const problem::id &id,
                       const problem::Revision &revision);

  /// \warning requires at least shared lock and problem existence
  problem::ImportResult read_import_result_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  void write_import_result_(const problem::id &id,
                            const problem::ImportResult &import_result);

  /// \warning requires unique lock and problem existence
  problem::StatusResult import_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  problem::StatusResult import_(const problem::id &id,
                                const problem::Revision &revision);

  /// \warning requires unique lock and problem existence
  problem::StatusResult import_(
      const problem::id &id, const problem::Revision &revision,
      const boost::filesystem::path &problem_location);

  /// \warning requires unique lock and problem existence
  void prepare_import_(const problem::id &id);
  /// \return true if import should be scheduled
  bool prepare_import(const problem::id &id);

  void post_import_(const problem::id &id);
  void post_import(const problem::id &id);

  void pm_create_recursively(const boost::filesystem::path &path);

  /// \warning requires unique lock
  void update_archive_revision_();

  /// \warning requires shared lock
  bool is_equal_revision_(const ArchiveRevision &revision);

 private:
  boost::mutex m_import_lock;
  boost::upgrade_mutex m_lock;
  boost::mutex m_pm_lock;
  boost::asio::io_service &m_io_service;
  const bunsan::interprocess::file_guard m_flock;
  bunsan::utility::custom_resolver m_resolver;
  const location_config m_location;
  /// internal problem storage packing
  const bunsan::utility::archiver::factory_type m_problem_archiver_factory;
  const problem_config m_problem;
  importer m_importer;
  bunsan::pm::repository m_repository;
  revision m_revision;
};

}  // namespace archive
}  // namespace bacs
