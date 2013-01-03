#include "bacs/archive/repository.hpp"
#include "bacs/archive/error.hpp"
#include "bacs/archive/problem/flags.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"
#include "bunsan/filesystem/operations.hpp"

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

        inline problem::hash_type read_hash(const boost::filesystem::path &path)
        {
            return read_binary(path);
        }

        inline problem::info_type read_info(const boost::filesystem::path &path)
        {
            return read_binary(path);
        }

        inline void write_hash(const boost::filesystem::path &path, const problem::hash_type &hash)
        {
            return write_binary(path, hash);
        }

        inline void write_info(const boost::filesystem::path &path, const problem::info_type &info)
        {
            return write_binary(path, info);
        }

        problem::hash_type compute_hash(const boost::filesystem::path &path)
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
            problem::hash_type hash(sizeof(value));
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
        problem::validate_id(id);
        // TODO
    }

    bool repository::extract(const problem::id &id, const boost::filesystem::path &location)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (exists(id))
            {
                // TODO
                return true;
            }
        }
        return false;
    }

    problem::id_set repository::existing()
    {
        problem::id_set set;
        // TODO
        return set;
    }

    problem::id_set repository::available()
    {
        problem::id_set set;
        // TODO
        return set;
    }

    bool repository::exists(const problem::id &id)
    {
        problem::validate_id(id);
        // FIXME
        return false;
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

    boost::optional<problem::status_type> repository::status(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (exists(id))
            {
                problem::status_type status;
                // TODO
                return status;
            }
        }
        return boost::optional<problem::status_type>();
    }

    bool repository::set_flag(const problem::id &id, const problem::flag &flag)
    {
        problem::validate_id(id);
        problem::validate_flag(flag);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id))
            {
                // TODO
                return true;
            }
        }
        return false;
    }

    bool repository::set_flags(const problem::id &id, const problem::flag_set &flags)
    {
        problem::validate_id(id);
        std::for_each(flags.begin(), flags.end(), problem::validate_flag);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id))
            {
                // TODO
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
            if (exists(id))
            {
                // TODO
                return true;
            }
        }
        return false;
    }

    bool repository::unset_flags(const problem::id &id, const problem::flag_set &flags)
    {
        problem::validate_id(id);
        std::for_each(flags.begin(), flags.end(), problem::validate_flag);
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id))
            {
                // TODO
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
            if (exists(id))
            {
                // TODO
            }
        }
        return false;
    }

    boost::optional<problem::info_type> repository::info(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (exists(id))
            {
                problem::info_type info;
                // TODO
                return info;
            }
        }
        return boost::optional<problem::info_type>();
    }

    bool repository::has_flag(const problem::id &id, const problem::flag &flag)
    {
        problem::validate_id(id);
        problem::validate_flag(flag);
        if (exists(id))
        {
            // TODO
        }
        return false;
    }

    boost::optional<problem::hash_type> repository::hash(const problem::id &id)
    {
        problem::validate_id(id);
        if (exists(id))
        {
            const shared_lock_guard lk(m_lock);
            if (exists(id))
            {
                // TODO
            }
        }
        return boost::optional<problem::hash_type>();
    }

    problem::import_info repository::rename(const problem::id &current, const problem::id &future)
    {
        problem::validate_id(current);
        problem::validate_id(future);
        problem::import_info info;
        // TODO work on error messages
        if (exists(current) && !exists(future))
        {
            const lock_guard lk(m_lock);
            if (exists(current))
            {
                // TODO
            }
        }
        return info;
    }

    problem::import_info repository::repack(const problem::id &id)
    {
        problem::validate_id(id);
        problem::import_info info;
        if (exists(id))
        {
            const lock_guard lk(m_lock);
            if (exists(id))
            {
                // TODO
            }
        }
        return info;
    }
}}
