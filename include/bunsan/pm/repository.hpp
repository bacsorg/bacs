#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <mutex>

#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

#include "bunsan/pm/entry.hpp"

namespace bunsan{namespace pm
{
	class repository: private boost::noncopyable
	{
	public:
		explicit repository(const boost::property_tree::ptree &config_);
		/*!
		 * \brief Extract package data to destination.
		 *
		 * \todo We can do implicit update.
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
		static std::mutex slock;
		const boost::property_tree::ptree config;
		// private functions
		/// update logic
		void update(const entry &package);
		/// updates package depends and imports "index" files tree
		void update_index_tree(const entry &package);
		/*!
		 * \param supdated packages which sources were fetched
		 * \param pupdated packages which was checked with ntv->package_outdated(package)
		 */
		void update_source_imports(const entry &package, std::set<entry> &supdated, std::set<entry> &pupdated, std::set<entry> &sin, std::set<entry> &pin);
		/*
		 * \copydoc bunsan::pm::repository::update_source_imports
		 */
		void update_package_imports(const entry &package, std::set<entry> &supdated, std::set<entry> &pupdated, std::set<entry> &pin);
	};
}}

#endif //REPOSITORY_HPP

