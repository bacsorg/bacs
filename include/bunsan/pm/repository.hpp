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
		void update_imports(const entry &package, std::set<entry> &updated, std::set<entry> &in);
		//void update_depends(const entry &package, std::map<entry, std::pair<int, bool> > &status, std::set<entry> &updated);
		void update_depends(const entry &package, std::map<entry, bool> &status, std::set<entry> &updated, std::set<entry> &in);
	};
}}

#endif //REPOSITORY_HPP

