#include <bacs/archive/repository.hpp>

#include <bacs/archive/error.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/log/trivial.hpp>
#include <bunsan/protobuf/binary.hpp>

#include <boost/assert.hpp>
#include <boost/crc.hpp>
#include <boost/scope_exit.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_lock_guard.hpp>

namespace bacs {
namespace archive {

using lock_guard = boost::lock_guard<boost::upgrade_mutex>;
using shared_lock_guard = boost::shared_lock_guard<boost::upgrade_mutex>;
using upgrade_lock_guard = boost::upgrade_lock<boost::upgrade_mutex>;
using upgrade_to_unique_lock_guard =
    boost::upgrade_to_unique_lock<boost::upgrade_mutex>;

namespace {
void touch(const boost::filesystem::path &path) {
  bunsan::filesystem::ofstream fout(path);
  fout.close();
  BOOST_ASSERT(boost::filesystem::exists(path));
}  // namespace

problem::Revision compute_hash_revision(const boost::filesystem::path &path) {
  boost::crc_32_type crc;
  bunsan::filesystem::ifstream fin(path, std::ios_base::binary);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
    char buf[BUFSIZ];
    do {
      fin.read(buf, BUFSIZ);
      crc.process_bytes(buf, fin.gcount());
    } while (fin);
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
  fin.close();
  auto value = crc.checksum();
  problem::Revision revision;
  revision.mutable_value()->resize(sizeof(value));
  for (std::size_t i = 0; i < sizeof(value); ++i, value >>= 8)
    (*revision.mutable_value())[i] = value & 0xFF;
  return revision;
}

problem::StatusResult status_error(const std::string &reason) {
  problem::StatusResult status_result;
  status_result.mutable_error()->set_reason(reason);
  return status_result;
}

problem::StatusResult status_error(const problem::Error::Code code) {
  problem::StatusResult status_result;
  status_result.mutable_error()->set_code(code);
  return status_result;
}

problem::StatusResult status_not_found() {
  return status_error(problem::Error::NOT_FOUND);
}

problem::StatusResult status_not_implemented() {
  return status_error("Not implemented");
}

problem::StatusResult status_status(problem::Status status) {
  problem::StatusResult status_result;
  *status_result.mutable_status() = std::move(status);
  return status_result;
}

problem::ImportResult import_error(const std::string &reason) {
  problem::ImportResult import_result;
  import_result.mutable_error()->set_reason(reason);
  return import_result;
}

problem::ImportResult import_error(const problem::Error::Code code) {
  problem::ImportResult import_result;
  import_result.mutable_error()->set_code(code);
  return import_result;
}

problem::ImportResult import_not_found() {
  return import_error(problem::Error::NOT_FOUND);
}

problem::ImportResult import_problem(bacs::problem::Problem problem) {
  problem::ImportResult import_result;
  *import_result.mutable_problem() = std::move(problem);
  return import_result;
}
}  // namespace

/// entry names
namespace ename {
const boost::filesystem::path flags = "flags";
const boost::filesystem::path import_result = "import_result";
const boost::filesystem::path revision = "revision";
}  // namespace ename

problem::StatusResult repository::upload(
    const problem::id &id, const boost::filesystem::path &location) {
  problem::validate_id(id);
  if (!is_locked(id)) {
    const lock_guard lk(m_lock);
    if (!is_locked(id)) {
      if (boost::filesystem::exists(m_location.repository_root / id)) {
        boost::filesystem::remove(m_location.repository_root / id /
                                  m_problem.data.filename);
        boost::filesystem::remove(m_location.repository_root / id /
                                  ename::revision);
        boost::filesystem::remove(m_location.repository_root / id /
                                  ename::import_result);
      } else {
        BOOST_VERIFY(boost::filesystem::create_directory(
            m_location.repository_root / id));
        BOOST_VERIFY(boost::filesystem::create_directory(
            m_location.repository_root / id / ename::flags));
      }
      const bunsan::utility::archiver_ptr archiver =
          m_problem_archiver_factory(m_resolver);
      BOOST_ASSERT(archiver);
      archiver->pack_contents(
          m_location.repository_root / id / m_problem.data.filename, location);
      const problem::Revision revision = compute_hash_revision(
          m_location.repository_root / id / m_problem.data.filename);
      write_revision_(id, revision);
      return schedule_import(id);
    }
  }
  return status_error(problem::Error::LOCKED);
}

bool repository::download(const problem::id &id,
                          const boost::filesystem::path &location) {
  problem::validate_id(id);
  if (exists(id)) {
    const shared_lock_guard lk(m_lock);
    if (exists(id)) {
      download_(id, location);
      return true;
    }
  }
  return false;
}

void repository::download_(const problem::id &id,
                           const boost::filesystem::path &location) {
  bunsan::filesystem::reset_dir(location);
  const bunsan::utility::archiver_ptr archiver =
      m_problem_archiver_factory(m_resolver);
  BOOST_ASSERT(archiver);
  archiver->unpack(m_location.repository_root / id / m_problem.data.filename,
                   location);
}

problem::IdSet repository::existing() {
  const shared_lock_guard lk(m_lock);
  return existing_();
}

problem::IdSet repository::existing_() {
  problem::IdSet set;
  for (boost::filesystem::directory_iterator i(m_location.repository_root), end;
       i != end; ++i) {
    const problem::id id = i->path().filename().string();
    problem::validate_id(id);
    set.add_id(id);
  }
  return set;
}

bool repository::exists(const problem::id &id) {
  problem::validate_id(id);
  return boost::filesystem::exists(m_location.repository_root / id);
}

problem::StatusResult repository::status(const problem::id &id) {
  problem::validate_id(id);
  if (exists(id)) {
    const shared_lock_guard lk(m_lock);
    return status_result_(id);
  }
  return status_not_found();
}

problem::StatusMap repository::status_all(const problem::IdSet &id_set) {
  problem::StatusMap status_map;
  const shared_lock_guard lk(m_lock);
  return status_all_(id_set);
}

problem::StatusMap repository::status_all() {
  problem::StatusMap status_map;
  const shared_lock_guard lk(m_lock);
  return status_all_(existing_());
}

problem::StatusMap repository::status_all_(const problem::IdSet &id_set) {
  BUNSAN_LOG_INFO << "Reading status of { " << id_set.ShortDebugString()
                  << " }";
  problem::StatusMap status_map;
  for (const problem::id &id : id_set.id()) {
    (*status_map.mutable_entry())[id] = status_result_(id);
  }
  return status_map;
}

problem::StatusResult repository::status_result_(const problem::id &id) {
  if (exists(id)) return status_status(status_(id));
  return status_not_found();
}

problem::Status repository::status_(const problem::id &id) {
  problem::Status status;
  *status.mutable_revision() = read_revision_(id);
  *status.mutable_flags() = flags_(id);
  return status;
}

problem::FlagSet repository::flags_(const problem::id &id) {
  problem::FlagSet flags;
  const boost::filesystem::path flags_dir =
      m_location.repository_root / id / ename::flags;
  for (boost::filesystem::directory_iterator i(flags_dir), end; i != end; ++i) {
    const problem::flag flag = i->path().filename().string();
    problem::validate_flag(flag);
    *flags.add_flag() = problem::flag_cast(flag);
  }
  return flags;
}

problem::StatusResult repository::set_flag(const problem::id &id,
                                           const problem::flag &flag) {
  problem::validate_id(id);
  problem::validate_flag(flag);
  if (problem::flag_equal(flag, problem::Flag::PENDING_IMPORT)) {
    BOOST_THROW_EXCEPTION(problem::flag_cant_be_set_error()
                          << problem::flag_cant_be_set_error::flag(flag));
  }
  if (exists(id)) {
    const lock_guard lk(m_lock);
    if (exists(id)) {
      if (is_read_only(id)) return status_error(problem::Error::READ_ONLY);
      set_flag_(id, flag);
      return status_status(status_(id));
    }
  }
  return status_not_found();
}

void repository::set_flag_(const problem::id &id, const problem::flag &flag) {
  touch(m_location.repository_root / id / ename::flags / flag);
}

problem::StatusResult repository::set_flags(const problem::id &id,
                                            const problem::FlagSet &flags) {
  problem::validate_id(id);
  problem::validate_flag_set(flags);
  if (exists(id)) {
    const lock_guard lk(m_lock);
    if (exists(id)) {
      if (is_read_only(id)) return status_error(problem::Error::READ_ONLY);
      for (const problem::Flag &flag : flags.flag()) set_flag_(id, flag);
      return status_status(status_(id));
    }
  }
  return status_not_found();
}

problem::StatusResult repository::unset_flag(const problem::id &id,
                                             const problem::flag &flag) {
  problem::validate_id(id);
  problem::validate_flag(flag);
  if (exists(id)) {
    const lock_guard lk(m_lock);
    if (exists(id)) {
      if (is_read_only(id)) return status_error(problem::Error::READ_ONLY);
      // it is not possible to remove ignore flag
      if (problem::flag_equal(flag, problem::Flag::IGNORE)) {
        unset_flag_(id, flag);
      }
      return status_status(status_(id));
    }
  }
  return status_not_found();
}

void repository::unset_flag_(const problem::id &id, const problem::flag &flag) {
  boost::filesystem::remove(m_location.repository_root / id / ename::flags /
                            flag);
}

problem::StatusResult repository::unset_flags(const problem::id &id,
                                              const problem::FlagSet &flags) {
  problem::validate_id(id);
  problem::validate_flag_set(flags);
  if (exists(id)) {
    const lock_guard lk(m_lock);
    if (exists(id)) {
      if (is_read_only(id)) return status_error(problem::Error::READ_ONLY);
      for (const problem::Flag &flag : flags.flag()) {
        // it is not possible to unset IGNORE and PENDING_IMPORT flags
        if (!problem::flag_equal(flag, problem::Flag::IGNORE) &&
            !problem::flag_equal(flag, problem::Flag::PENDING_IMPORT)) {
          unset_flag_(id, flag);
        }
      }
      return status_status(status_(id));
    }
  }
  return status_not_found();
}

problem::StatusResult repository::clear_flags(const problem::id &id) {
  problem::validate_id(id);
  if (exists(id)) {
    const lock_guard lk(m_lock);
    if (exists(id)) {
      if (is_read_only(id)) return status_error(problem::Error::READ_ONLY);
      const boost::filesystem::path flags_dir =
          m_location.repository_root / id / ename::flags;
      for (boost::filesystem::directory_iterator i(flags_dir), end; i != end;
           ++i) {
        const problem::flag flag = i->path().filename().string();
        // it is not possible to clear IGNORE and PENDING_IMPORT flags
        if (!problem::flag_equal(flag, problem::Flag::IGNORE) &&
            !problem::flag_equal(flag, problem::Flag::PENDING_IMPORT)) {
          BOOST_VERIFY(boost::filesystem::remove(*i));
        }
      }
      return status_status(status_(id));
    }
  }
  return status_not_found();
}

problem::ImportResult repository::import_result(const problem::id &id) {
  problem::validate_id(id);
  if (exists(id)) {
    const shared_lock_guard lk(m_lock);
    if (exists(id)) {
      if (has_flag(id, problem::Flag::PENDING_IMPORT)) {
        return import_error(problem::Error::PENDING_IMPORT);
      } else {
        return read_import_result_(id);
      }
    }
  }
  return import_not_found();
}

problem::ImportResult repository::read_import_result_(const problem::id &id) {
  return bunsan::protobuf::binary::parse_make<problem::ImportResult>(
      m_location.repository_root / id / ename::import_result);
}

void repository::write_import_result_(
    const problem::id &id, const problem::ImportResult &import_result) {
  bunsan::protobuf::binary::serialize(
      import_result, m_location.repository_root / id / ename::import_result);
}

bool repository::has_flag(const problem::id &id, const problem::flag &flag) {
  problem::validate_id(id);
  problem::validate_flag(flag);
  return boost::filesystem::exists(m_location.repository_root / id /
                                   ename::flags / flag);
}

problem::Revision repository::read_revision_(const problem::id &id) {
  return bunsan::protobuf::binary::parse_make<problem::Revision>(
      m_location.repository_root / id / ename::revision);
}

void repository::write_revision_(const problem::id &id,
                                 const problem::Revision &revision) {
  bunsan::protobuf::binary::serialize(
      revision, m_location.repository_root / id / ename::revision);
}

problem::StatusResult repository::rename(const problem::id &current,
                                         const problem::id &future) {
  problem::validate_id(current);
  problem::validate_id(future);
  enum error_type {
    ok,
    current_does_not_exist,
    current_is_locked,
    future_exists
  } error = ok;
  const auto rename_status = [this, &current, &future]() -> error_type {
    if (!exists(current)) return current_does_not_exist;
    if (is_locked(current)) return current_is_locked;
    if (exists(future)) return future_exists;
    return ok;
  };
  error = rename_status();
  if (error == ok) {
    const lock_guard lk(m_lock);
    error = rename_status();
    if (error == ok) {
      // TODO
      return status_not_implemented();
    }
  }
  switch (error) {
    case ok:
      BOOST_ASSERT_MSG(false, "Impossible");
      return status_not_implemented();
    case current_does_not_exist:
      return status_error("$current problem does not exist");
    case current_is_locked:
      return status_error("$current problem is locked");
    case future_exists:
      return status_error("$future problem exists");
  }
  BOOST_ASSERT_MSG(false, "Impossible to get here");
  return status_not_implemented();
}

bool repository::prepare_import(const problem::id &id) {
  BUNSAN_LOG_DEBUG << "Preparing " << id << " for import";
  if (exists(id)) {
    const lock_guard lk(m_lock);
    if (exists(id)) {
      set_flag_(id, problem::Flag::PENDING_IMPORT);
      return true;
    }
  }
  return false;
}

problem::StatusResult repository::schedule_import(const problem::id &id) {
  BUNSAN_LOG_INFO << "Scheduling " << id << " for import";
  problem::validate_id(id);
  bool schedule = false;
  BOOST_SCOPE_EXIT_ALL(&) {
    // lock should not be held during execution of this code
    if (schedule) m_io_service.post([this, id] { import(id); });
  };
  schedule = prepare_import(id);
  return status(id);
}

problem::StatusMap repository::schedule_import_all(
    const problem::IdSet &id_set) {
  BUNSAN_LOG_INFO << "Scheduling { " << id_set.ShortDebugString()
                  << " } for import";
  std::unordered_set<problem::id> schedule;
  BOOST_SCOPE_EXIT_ALL(&) {
    BUNSAN_LOG_DEBUG << "Posting scheduled { " << id_set.ShortDebugString()
                     << " } for execution";
    for (const auto &id : schedule) {
      m_io_service.post([this, id] { import(id); });
    }
  };
  for (const auto &id : id_set.id()) {
    if (prepare_import(id)) schedule.insert(id);
  }
  return status_all(id_set);
}

problem::StatusMap repository::schedule_import_all_pending() {
  BUNSAN_LOG_INFO << "Scheduling all pending problems for import";
  return schedule_import_all(with_flag(problem::Flag::PENDING_IMPORT));
}

problem::StatusResult repository::import(const problem::id &id) {
  problem::validate_id(id);
  BUNSAN_LOG_INFO << "Importing " << id;
  // no parallel imports permitted
  const boost::lock_guard<boost::mutex> lk(m_import_lock);
  if (exists(id)) {
    upgrade_lock_guard lk(m_lock);
    if (exists(id)) {
      BOOST_ASSERT(exists(id));
      problem::Revision revision;
      try {
        revision = read_revision_(id);
      } catch (std::exception &) {
        BUNSAN_LOG_WARNING << "Revision unreadable for problem = " << id
                           << ", regenerated";
        revision = compute_hash_revision(m_location.repository_root / id /
                                         m_problem.data.filename);
        upgrade_to_unique_lock_guard ulk(lk);
        write_revision_(id, revision);
      }
      const bunsan::tempfile tmpdir =
          bunsan::tempfile::directory_in_directory(m_location.tmpdir);
      download_(id, tmpdir.path());
      lk.unlock();
      // import_() uses downloaded source to perform operations
      // all required locks are handled inside
      return import_(id, revision, tmpdir.path());
    }
  }
  return status_not_found();
}

problem::StatusResult repository::import_(
    const problem::id &id, const problem::Revision &revision,
    const boost::filesystem::path &problem_location) {
  // this code does not lock storage and may be executed in parallel
  problem::StatusResult status_result;
  problem::ImportResult import_result;
  bool ok = false;
  try {
    bacs::problem::importer::options options;
    options.problem_dir = problem_location;
    options.destination =
        m_location.pm_repository_root / m_problem.root_package.location() / id;
    options.root_package = m_problem.root_package / id;
    options.id = id;
    options.revision = revision;
    import_result = import_problem(m_importer.convert(options));
    pm_create_recursively(options.destination);
    ok = true;
  } catch (std::exception &e) {
    import_result = import_error(e.what());
    status_result = status_error(e.what());
  }
  // end of non-locking section
  {
    const lock_guard lk(m_lock);
    if (!exists(id)) return status_error("Problem was removed during import");
    const problem::Revision current_revision = read_revision_(id);
    if (current_revision.value() != revision.value()) {
      return status_error("Problem was changed during import");
    }
    write_import_result_(id, import_result);
    if (ok) {
      unset_flag_(id, problem::Flag::IGNORE);
    } else {
      set_flag_(id, problem::Flag::IGNORE);
    }
    unset_flag_(id, problem::Flag::PENDING_IMPORT);
    if (ok) {  // should be last block since it reads
      problem::Status &status = *status_result.mutable_status();
      *status.mutable_revision() = revision;
      *status.mutable_flags() = flags_(id);
    }
  }
  return status_result;
}

void repository::pm_create_recursively(const boost::filesystem::path &path) {
  const boost::lock_guard<boost::mutex> lk(m_pm_lock);
  m_repository.create_recursively(path, m_problem.strip);
}

}  // namespace archive
}  // namespace bacs
