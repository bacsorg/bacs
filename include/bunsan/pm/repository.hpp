#pragma once

#include "bunsan/pm/entry.hpp"
#include "bunsan/pm/config.hpp"
#include "bunsan/pm/snapshot.hpp"
#include "bunsan/pm/error.hpp"

#include "bunsan/interprocess/sync/file_lock.hpp"

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

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
         * \param strip remove unnecesarry sources from packed source
         */
        void create(const boost::filesystem::path &source, bool strip=false);

        /*!
         * \brief Extract package data to destination.
         *
         * \todo We can do explicit update.
         */
        void extract(const entry &package, const boost::filesystem::path &destination);

        /*!
         * \brief Run clean up actions, may have negative effect on performance because of cleaning the cache, but can free disk space.
         *
         * \todo It seems that such method should be, but semantics can be different.
         */
        void clean();

        const pm::config &config() const;

        ~repository();

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
