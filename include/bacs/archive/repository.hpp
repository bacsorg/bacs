#pragma once

#include <bacs/archive/config.hpp>
#include <bacs/archive/importer.hpp>
#include <bacs/archive/problem.hpp>

#include <bunsan/interprocess/sync/file_guard.hpp>
#include <bunsan/pm/repository.hpp>
#include <bunsan/tempfile.hpp>
#include <bunsan/utility/archiver.hpp>
#include <bunsan/utility/custom_resolver.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <functional>
#include <string>

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
 * *_all() functions are wrappers for their counterparts
 * without "_all" suffix. These functions return maps, with values
 * returned by functions counterparts.
 *
 * There are three kinds of locks:
 *     - exclusive-lock -- single operation at time, no other locks (except
 *lock-free)
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
   * \brief Insert problems from archive into repository.
   *
   * \return import information
   *
   * Not atomic.
   *
   * \see repository::insert
   */
  problem::ImportMap insert_all(const boost::filesystem::path &location);

  /*!
   * \copydoc insert_all()
   */
  problem::ImportMap insert_all(const archiver_options &archiver_options_,
                                const boost::filesystem::path &archive);

  /*!
   * \brief Insert particular problem into repository.
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
  problem::ImportInfo insert(const problem::id &id,
                             const boost::filesystem::path &location);

  /// repository::insert(location.filename().string(), location)
  problem::ImportInfo insert(const boost::filesystem::path &location);

  /*!
   * \brief Extract problems from repository.
   *
   * Ignores nonexistent problems.
   *
   * Not atomic.
   *
   * \see repository::exists
   * \see repository::extract
   */
  void extract_all(const problem::IdSet &id_set,
                   const boost::filesystem::path &location);

  /*!
   * \copydoc extract_all()
   *
   * \return archive with problems
   */
  void extract_all(const problem::IdSet &id_set,
                   const archiver_options &archiver_options_,
                   const boost::filesystem::path &archive);

  /*!
   * \copydoc extract_all()
   */
  bunsan::tempfile extract_all(const problem::IdSet &id_set,
                               const archiver_options &archiver_options_);

  /*!
   * \brief Extract problem from repository.
   *
   * \return false if problem does not exist
   *
   * Atomic, shared-lock.
   *
   * \see repository::exists
   */
  bool extract(const problem::id &id, const boost::filesystem::path &location);

  /*!
   * \brief Change problem id and repack.
   *
   * If problem with id=future exists or repack fails functions does nothing.
   *
   * Atomic, shared-lock, exclusive-lock.
   *
   * \todo full locking documentation
   *
   * \see repository::repack
   */
  problem::ImportInfo rename(const problem::id &current,
                             const problem::id &future);

  /*!
   * \brief Set of existing problems.
   *
   * Not atomic, lock-free.
   *
   * \see repository::exists
   */
  problem::IdSet existing();

  /*!
   * \brief Set of available problems.
   *
   * \see repository::is_available
   */
  problem::IdSet available();

  /*!
   * \brief Check problem for existence.
   *
   * Albeit problem exists, it may be ignored
   * by some functions.
   *
   * Atomic, lock-free.
   *
   * \see repository::is_available
   */
  bool exists(const problem::id &id);

  /*!
   * \brief Get existing problems from list.
   *
   * Not atomic.
   *
   * \see repository::is_available
   */
  problem::IdSet existing(const problem::IdSet &id_set);

  /*!
   * \brief Check problem for availability.
   *
   * \return true if problem exists and
   * is not marked by problem::flag::ignore.
   *
   * Atomic, shared-lock.
   *
   * \see repository::exists
   * \see repository::has_flag
   */
  bool is_available(const problem::id &id);

  /*!
   * \brief Get available problems from list.
   *
   * Not atomic.
   *
   * \see repository::is_available
   */
  problem::IdSet available(const problem::IdSet &id_set);

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
  problem::ImportInfo status(const problem::id &id);

  /*!
   * \brief Get problems status.
   *
   * Not atomic.
   *
   * \see repository::status
   */
  problem::ImportMap status_all(const problem::IdSet &id_set);

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
  auto has_flag(const problem::id &id, const Flag &flag) {
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
  auto with_flag(const problem::IdSet &id_set, const Flag &flag) {
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
  auto with_flag(const Flag &flag) {
    return with_flag(problem::flag_to_string(flag));
  }

  /*!
   * \brief Set problem flag.
   *
   * \return false if problem does not exist or is_read_only
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::is_read_only
   */
  bool set_flag(const problem::id &id, const problem::flag &flag);

  template <typename Flag>
  auto set_flag(const problem::id &id, const Flag &flag) {
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
  bool set_flags(const problem::id &id, const problem::FlagSet &flags);

  /*!
   * \brief Set problems flags.
   *
   * Not atomic.
   *
   * \see repository::set_flags
   */
  problem::IdSet set_flags_all(const problem::IdSet &id_set,
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
  bool unset_flag(const problem::id &id, const problem::flag &flag);

  template <typename Flag>
  auto unset_flag(const problem::id &id, const Flag &flag) {
    return unset_flag(id, problem::flag_to_string(flag));
  }

  /*!
   * \brief Unset problem flags.
   *
   * \return false if problem does not exist or is_read_only
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
  bool unset_flags(const problem::id &id, const problem::FlagSet &flags);

  /*!
   * \brief Unset problems flags.
   *
   * Not atomic.
   *
   * \see repository::unset_flags
   */
  problem::IdSet unset_flags_all(const problem::IdSet &id_set,
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
  bool clear_flags(const problem::id &id);

  /*!
   * \brief Clear problems flags.
   *
   * \return Set of cleared problems.
   *
   * Not atomic.
   *
   * \see repository::clear_flags
   */
  problem::IdSet clear_flags_all(const problem::IdSet &id_set);

  /*!
   * \brief Mark problems with problem::flag::ignore.
   *
   * If problem does not exists id is ignored.
   *
   * \return set of marked problems
   *
   * Not atomic.
   *
   * \see repository::ignore
   */
  problem::IdSet ignore_all(const problem::IdSet &id_set);

  /*!
   * \brief Alias for repository::set_flag(id, #problem::flag::ignore).
   *
   * \return false if problem does not exists or is_read_only
   *
   * \note It is not possible to remove ignore flag manually.
   *
   * \see repository::set_flag
   * \see repository::exists
   * \see repository::is_read_only
   */
  bool ignore(const problem::id &id);

  /* info */
  /*!
   * \brief Get problem info.
   *
   * \return error if problem is not available
   *
   * \see repository::is_available
   */
  problem::Info info(const problem::id &id);

  /*!
   * \brief Get info map for given problems.
   *
   * \see repository::is_available
   * \see repository::info
   */
  problem::InfoMap info_all(const problem::IdSet &id_set);

  /* repack */
  /*!
   * \brief Repack problem.
   *
   * Repacking is similar to import
   * except using internal copy of problem.
   *
   * Can be useful if problem conversion utility is changed
   * or index is corrupted.
   *
   * If repack is not successful problem is marked by problem::flag::ignore.
   * Otherwise, problem::flag::ignore is unset.
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::exists
   * \see repository::set_flag
   */
  problem::ImportInfo repack(const problem::id &id);

  /*!
   * \brief Repack all given problems.
   *
   * Not atomic.
   *
   * \see repository::repack
   */
  problem::ImportMap repack_all(const problem::IdSet &id_set);

  /*!
   * \brief Repack all problems.
   *
   * Not atomic.
   *
   * \see repository::repack_all
   */
  problem::ImportMap repack_all();

  /*!
   * \brief Schedules repack for future execution with post.
   *
   * Atomic, exclusive-lock.
   *
   * \see repository::repack
   */
  problem::ImportInfo schedule_repack(const problem::id &id);

  /*!
   * \brief Schedules repack for all given problems.
   *
   * Not atomic.
   *
   * \see repository::schedule_repack
   */
  problem::ImportMap schedule_repack_all(const problem::IdSet &id_set);

  /*!
   * \brief Schedules repack for all problems.
   *
   * Not atomic.
   *
   * \see repository::schedule_repack_all
   */
  problem::ImportMap schedule_repack_all();

  /*!
   * \brief Schedules repack for all pending problems.
   *
   * Not atomic.
   *
   * \see repository::schedule_repack_all
   */
  problem::ImportMap schedule_repack_all_pending();

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

  /// \warning requires at least shared lock and problem existence
  void extract_(const problem::id &id, const boost::filesystem::path &location);

  /// \warning requires at least shared lock
  bool is_available_(const problem::id &id);

  /// \warning requires at least shared lock and problem existence
  problem::Status status_(const problem::id &id);

  /// \warning requires at least shared lock and problem existence
  problem::FlagSet flags_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  void set_flag_(const problem::id &id, const problem::flag &flag);

  template <typename Flag>
  auto set_flag_(const problem::id &id, const Flag &flag) {
    return set_flag_(id, problem::flag_to_string(flag));
  }

  /// \warning requires unique lock and problem existence
  void unset_flag_(const problem::id &id, const problem::flag &flag);

  template <typename Flag>
  auto unset_flag_(const problem::id &id, const Flag &flag) {
    return unset_flag_(id, problem::flag_to_string(flag));
  }

  /// \warning requires at least shared lock and problem existence
  problem::Revision read_revision_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  void write_revision_(const problem::id &id,
                       const problem::Revision &revision);

  /// \warning requires at least shared lock and problem existence
  problem::Info read_info_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  void write_info_(const problem::id &id, const problem::Info &info);

  /// \warning requires unique lock and problem existence
  problem::ImportInfo repack_(const problem::id &id);

  /// \warning requires unique lock and problem existence
  problem::ImportInfo repack_(const problem::id &id,
                              const problem::Revision &revision);

  /// \warning requires unique lock and problem existence
  problem::ImportInfo repack_(const problem::id &id,
                              const problem::Revision &revision,
                              const boost::filesystem::path &problem_location);

  /// \return true if repack should be scheduled
  bool prepare_repack(const problem::id &id);

 private:
  boost::shared_mutex m_lock;
  boost::asio::io_service &m_io_service;
  const bunsan::interprocess::file_guard m_flock;
  bunsan::utility::custom_resolver m_resolver;
  const location_config m_location;
  /// internal problem storage packing
  const bunsan::utility::archiver::factory_type m_problem_archiver_factory;
  const problem_config m_problem;
  importer m_importer;
  bunsan::pm::repository m_repository;
};

}  // namespace archive
}  // namespace bacs
