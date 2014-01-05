#pragma once

#include <bunsan/pm/config.hpp>
#include <bunsan/pm/entry.hpp>
#include <bunsan/pm/error.hpp>
#include <bunsan/pm/snapshot.hpp>

#include <bunsan/interprocess/sync/file_lock.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <ctime>

namespace bunsan{namespace pm
{
    /*!
     * \brief Class represents bunsan repository
     *
     * \todo Creation of two different instances of this class
     * linked to one physical repository
     * will cause undefined behavior.
     * Objects of this class are thread-safe.
     */
    class repository: private boost::noncopyable
    {
    public:
        explicit repository(const pm::config &config_);
        explicit repository(const boost::property_tree::ptree &config_);

        /*!
         * \brief Create source package from source
         *
         * \param source path to source
         * \param strip remove unnecessary sources from packed source
         */
        void create(const boost::filesystem::path &source, bool strip=false);

        /*!
         * \brief Create source packages from sources recursively starting from root.
         *
         * \see repository::create()
         */
        void create_recursively(const boost::filesystem::path &root, bool strip=false);

        /*!
         * \brief Extract package data to destination.
         *
         * \todo We can do explicit update.
         */
        void extract(const entry &package, const boost::filesystem::path &destination);

        /*!
         * \brief Install package to destination.
         */
        void install(const entry &package, const boost::filesystem::path &destination);

        /*!
         * \brief Update previously installed package.
         */
        void update(const entry &package, const boost::filesystem::path &destination);

        /*!
         * \brief Update previously installed package.
         *
         * Will not update package index tree if lifetime has not passed since previous update.
         */
        void update(const entry &package, const boost::filesystem::path &destination, const std::time_t &lifetime);

        /*!
         * \brief Check if actual update is needed.
         *
         * \note Does not block.
         */
        bool need_update(const entry &package, const boost::filesystem::path &destination, const std::time_t &lifetime);

        /*!
         * \brief Run clean up actions, may have negative effect on performance because of cleaning the cache, but can free disk space.
         *
         * \todo It seems that such method should be, but semantics can be different.
         */
        void clean();

        const pm::config &config() const;

        ~repository();

    public:
        /*!
         * \brief Library build version. Unspecified string.
         *
         * \note May be overridden by BUNSAN_PM_VERSION
         */
        static std::string version();

    private:
        /// update logic
        void update(const entry &package);

        /// updates package depends and imports "index" files tree
        void update_index_tree(const entry &package);

        /// dfs topological-sort order update algorithm
        enum class stage_type: int;
        typedef std::pair<entry, stage_type> stage;

        // FIXME maps should be unordered
        bool update_package_depends(
            const stage &stage_,
            std::map<stage, bool> &updated,
            std::set<stage> &in,
            snapshot &current_snapshot,
            std::map<stage, snapshot> &snapshot_cache);

    private:
        class native;
        native *ntv;
        std::unique_ptr<bunsan::interprocess::file_lock> m_flock;
        pm::config m_config;
    };
}}
