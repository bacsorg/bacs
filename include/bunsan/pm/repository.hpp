#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

#include "bunsan/pm/entry.hpp"

namespace bunsan{namespace pm
{
	/*!
	 * \brief Class represents bunsan repository
	 *
	 * Creation of two different instances of this class
	 * linked to one physical repository
	 * will cause undefined behavior.
	 * Objects of this class are thread-safe.
	 */
	class repository: private boost::noncopyable
	{
	public:
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
		~repository();
	private:
		class native;
		native *ntv;
		std::unique_ptr<boost::interprocess::file_lock> flock;
		boost::mutex slock;
		const boost::property_tree::ptree config;
		// private functions
		/// update logic
		void update(const entry &package);
		/// updates package depends and imports "index" files tree
		void update_index_tree(const entry &package);
		/// dfs topological-sort order update algorithm
		bool update_package_depends(
			const std::pair<entry, int> &package,
			std::map<std::pair<entry, int>, bool> &updated,
			std::set<std::pair<entry, int>> &in,
			std::map<entry, boost::property_tree::ptree> &snapshot,
			std::map<std::pair<entry, int>, std::map<entry, boost::property_tree::ptree>> &snapshot_cache);
	};
}}

#endif //REPOSITORY_HPP

