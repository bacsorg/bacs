#include <bacs/archive/repository.hpp>
#include <bacs/archive/error.hpp>
#include <bacs/archive/problem/flags.hpp>
#include <bacs/archive/pb/problem.pb.h>
#include <bacs/archive/pb/convert.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_lock_guard.hpp>
#include <boost/crc.hpp>
#include <boost/assert.hpp>

namespace bacs{namespace archive
{
    typedef boost::lock_guard<boost::shared_mutex> lock_guard;
    typedef boost::shared_lock_guard<boost::shared_mutex> shared_lock_guard;

    namespace
    {
        void touch(const boost::filesystem::path &path)
        {
            BUNSAN_EXCEPTIONS_WRAP_BEGIN()
            {
                bunsan::filesystem::ofstream fout(path);
                fout.close();
            }
            BUNSAN_EXCEPTIONS_WRAP_END()
            BOOST_ASSERT(boost::filesystem::exists(path));
        }

        problem::binary read_binary(const boost::filesystem::path &path)
        {
            problem::binary bin;
            BUNSAN_EXCEPTIONS_WRAP_BEGIN()
            {
                bunsan::filesystem::ifstream fin(path, std::ios_base::binary);
                char buf[BUFSIZ];
                do
                {
                    fin.read(buf, BUFSIZ);
                    bin.insert(bin.end(), buf, buf + fin.gcount());
                }
                while (fin);
                fin.close();
            }
            BUNSAN_EXCEPTIONS_WRAP_END()
            return bin;
        }

        void write_binary(const boost::filesystem::path &path, const problem::binary &bin)
        {
            BUNSAN_EXCEPTIONS_WRAP_BEGIN()
            {
                bunsan::filesystem::ofstream fout(path, std::ios_base::binary);
                fout.write(reinterpret_cast<const char *>(bin.data()), bin.size());
                fout.close();
            }
            BUNSAN_EXCEPTIONS_WRAP_END()
        }

        problem::hash compute_hash(const boost::filesystem::path &path)
        {
            boost::crc_32_type crc;
            BUNSAN_EXCEPTIONS_WRAP_BEGIN()
            {
                bunsan::filesystem::ifstream fin(path, std::ios_base::binary);
                char buf[BUFSIZ];
                do
                {
                    fin.read(buf, BUFSIZ);
                    crc.process_bytes(buf, fin.gcount());
                }
                while (fin);
                fin.close();
            }
            BUNSAN_EXCEPTIONS_WRAP_END()
            auto value = crc.checksum();
            problem::hash hash(sizeof(value));
            for (std::size_t i = 0; i < sizeof(value); ++i, value >>= 8)
                hash[i] = value & 0xFF;
            return hash;
        }
    }

    /// entry names
    namespace ename
    {
        const boost::filesystem::path hash = "hash";
        const boost::filesystem::path info = "info";
        const boost::filesystem::path flags = "flags";
    }

    problem::import_info repository::insert(const problem::id &id, const boost::filesystem::path &location)
    {
        enum error_type
        {
            ok,
            problem_is_locked
        } error = ok;
        problem::validate_id(id);
        problem::import_info import_info;
        if (!is_locked(id))
        {
            const lock_guard lk(m_lock);
            if (!is_locked(id))
            {
                if (boost::filesystem::exists(m_location.repository_root / id))
                {
                    boost::filesystem::remove(m_location.repository_root / id / m_problem.data.filename);
                    boost::filesystem::remove(m_location.repository_root / id / ename::hash);
                    boost::filesystem::remove(m_location.repository_root / id / ename::info);
                }
                else
                {
                    BOOST_VERIFY(boost::filesystem::create_directory(m_location.repository_root / id));
                    BOOST_VERIFY(boost::filesystem::create_directory(m_location.repository_root / id / ename::flags));
                }
                const bunsan::utility::archiver_ptr archiver = m_problem_archiver_factory(m_resolver);
                BOOST_ASSERT(archiver);
                archiver->pack_contents(m_location.repository_root / id / m_problem.data.filename, location);
                const problem::hash hash = compute_hash(m_location.repository_root / id / m_problem.data.filename);
                write_hash_(id, hash);
                import_info = repack_(id, hash, location);
            }
            else
            {
                error = problem_is_locked;
            }
        }
        else
        {
            error = problem_is_locked;
        }
        switch (error)
        {
        case ok:
            /* nothing to do */
            break;
        case problem_is_locked:
            import_info.error = "problem is locked";
            break;
        }
        return import_info;
    }

    bool repository::extract(const problem::id &id, const boost::filesystem::path &location)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (exists(id))
            {
                extract_(id, location);
                return true;
            }
        }
        return false;
    }

    void repository::extract_(const problem::id &id, const boost::filesystem::path &location)
    {
        bunsan::filesystem::reset_dir(location);
        const bunsan::utility::archiver_ptr archiver = m_problem_archiver_factory(m_resolver);
        BOOST_ASSERT(archiver);
        archiver->unpack(m_location.repository_root / id / m_problem.data.filename, location);
    }

    problem::id_set repository::existing()
    {
        const shared_lock_guard lk(m_lock);
        problem::id_set set;
        for (boost::filesystem::directory_iterator i(m_location.repository_root), end; i != end; ++i)
        {
            const problem::id id = i->path().filename().string();
            problem::validate_id(id);
            set.insert(id);
        }
        return set;
    }

    problem::id_set repository::available()
    {
        const shared_lock_guard lk(m_lock);
        problem::id_set set;
        for (boost::filesystem::directory_iterator i(m_location.repository_root), end; i != end; ++i)
        {
            const problem::id id = i->path().filename().string();
            problem::validate_id(id);
            if (is_available_(id))
                set.insert(id);
        }
        return set;
    }

    bool repository::exists(const problem::id &id)
    {
        problem::validate_id(id);
        return boost::filesystem::exists(m_location.repository_root / id);
    }

    bool repository::is_available(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            return is_available_(id);
        }
        return false;
    }

    bool repository::is_available_(const problem::id &id)
    {
        return exists(id) && !has_flag(id, problem::flags::ignore);
    }

    boost::optional<problem::status> repository::status(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (exists(id))
                return status_(id);
        }
        return boost::optional<problem::status>();
    }

    problem::status repository::status_(const problem::id &id)
    {
        problem::status status;
        status.hash = read_hash_(id);
        status.flags = flags_(id);
        return status;
    }

    problem::flag_set repository::flags_(const problem::id &id)
    {
        problem::flag_set flags;
        for (boost::filesystem::directory_iterator i(m_location.repository_root / id / ename::flags), end; i != end; ++i)
        {
            const problem::flag flag = i->path().filename().string();
            problem::validate_flag(flag);
            flags.insert(flag);
        }
        return flags;
    }

    bool repository::set_flag(const problem::id &id, const problem::flag &flag)
    {
        problem::validate_id(id);
        problem::validate_flag(flag);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id) && !is_read_only(id))
            {
                set_flag_(id, flag);
                return true;
            }
        }
        return false;
    }

    void repository::set_flag_(const problem::id &id, const problem::flag &flag)
    {
        touch(m_location.repository_root / id / ename::flags / flag);
    }

    bool repository::set_flags(const problem::id &id, const problem::flag_set &flags)
    {
        problem::validate_id(id);
        std::for_each(flags.begin(), flags.end(), problem::validate_flag);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id) && !is_read_only(id))
            {
                for (const problem::flag &flag: flags)
                    set_flag_(id, flag);
                return true;
            }
        }
        return false;
    }

    bool repository::unset_flag(const problem::id &id, const problem::flag &flag)
    {
        problem::validate_id(id);
        problem::validate_flag(flag);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id) && !is_read_only(id))
            {
                if (flag != problem::flags::ignore) // it is not possible to remove ignore flag
                    unset_flag_(id, flag);
                return true;
            }
        }
        return false;
    }

    void repository::unset_flag_(const problem::id &id, const problem::flag &flag)
    {
        boost::filesystem::remove(m_location.repository_root / id / ename::flags / flag);
    }

    bool repository::unset_flags(const problem::id &id, const problem::flag_set &flags)
    {
        problem::validate_id(id);
        std::for_each(flags.begin(), flags.end(), problem::validate_flag);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id) && !is_read_only(id))
            {
                for (const problem::flag &flag: flags)
                    if (flag != problem::flags::ignore) // it is not possible to remove ignore flag
                        unset_flag_(id, flag);
                return true;
            }
        }
        return false;
    }

    bool repository::clear_flags(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id) && !is_read_only(id))
            {
                for (boost::filesystem::directory_iterator i(m_location.repository_root / id / ename::flags), end; i != end; ++i)
                {
                    const problem::flag flag = i->path().filename().string();
                    if (flag != problem::flags::ignore) // it is not possible to clear ignore flag
                        BOOST_VERIFY(boost::filesystem::remove(*i));
                }
                return true;
            }
        }
        return false;
    }

    boost::optional<problem::info> repository::info(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (is_available_(id))
                return read_info_(id);
        }
        return boost::optional<problem::info>();
    }

    problem::info repository::read_info_(const problem::id &id)
    {
        problem::info info;
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream fin(m_location.repository_root / id / ename::info);
            if (!info.ParseFromIstream(&fin))
                BOOST_THROW_EXCEPTION(protobuf_parsing_error());
            fin.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
        return info;
    }

    void repository::write_info_(const problem::id &id, const problem::info &info)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ofstream fout(m_location.repository_root / id / ename::info);
            if (!info.SerializeToOstream(&fout))
                BOOST_THROW_EXCEPTION(protobuf_serialization_error());
            fout.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
    }

    bool repository::has_flag(const problem::id &id, const problem::flag &flag)
    {
        problem::validate_id(id);
        problem::validate_flag(flag);
        return boost::filesystem::exists(m_location.repository_root / id / ename::flags / flag);
    }

    boost::optional<problem::hash> repository::hash(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (is_available_(id))
                return read_hash_(id);
        }
        return boost::optional<problem::hash>();
    }

    problem::hash repository::read_hash_(const problem::id &id)
    {
        return read_binary(m_location.repository_root / id / ename::hash);
    }

    void repository::write_hash_(const problem::id &id, const problem::hash &hash)
    {
        write_binary(m_location.repository_root / id / ename::hash, hash);
    }

    problem::import_info repository::rename(const problem::id &current, const problem::id &future)
    {
        problem::validate_id(current);
        problem::validate_id(future);
        enum error_type
        {
            ok,
            current_does_not_exist,
            current_is_locked,
            future_exists
        } error = ok;
        const auto rename_status =
            [this, &current, &future]() -> error_type
            {
                if (!exists(current))
                    return current_does_not_exist;
                if (is_locked(current))
                    return current_is_locked;
                if (exists(future))
                    return future_exists;
                return ok;
            };
        problem::import_info import_info;
        error = rename_status();
        if (error == ok)
        {
            const lock_guard lk(m_lock);
            error = rename_status();
            if (error == ok)
            {
                import_info.error = "Not implemented";
            }
        }
        switch (error)
        {
        case ok:
            /* nothing to do */
            break;
        case current_does_not_exist:
            import_info.error = "$current problem does not exist";
            break;
        case current_is_locked:
            import_info.error = "$current problem is locked";
            break;
        case future_exists:
            import_info.error = "$future problem exists";
            break;
        }
        return import_info;
    }

    problem::import_info repository::repack(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id))
                return repack_(id);
        }
        problem::import_info import_info;
        import_info.error = "Problem does not exist";
        return import_info;
    }

    problem::import_info repository::repack_(const problem::id &id)
    {
        BOOST_ASSERT(exists(id));
        return repack_(id, read_hash_(id));
    }

    problem::import_info repository::repack_(const problem::id &id, const problem::hash &hash)
    {
        const bunsan::tempfile tmpdir = bunsan::tempfile::in_dir(m_location.tmpdir);
        extract_(id, tmpdir.path());
        return repack_(id, hash, tmpdir.path());
    }
}}
