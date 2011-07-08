#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>

#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

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
		void extract(const std::string &package, const boost::filesystem::path &destination);
		/*!
		 * \brief Run clean up actions, may have negative effect on performance because of cleaning the cache, but can free disk space.
		 *
		 * \todo It seems that such method should be, but semantics can be different.
		 */
		void clean();
		/*!
		 * \brief Checks package name, if it is not valid std::runtime_error will be thrown
		 */
		static void check_package_name(const std::string &package);
	private:
		class native;
		std::unique_ptr<boost::interprocess::file_lock> flock;
		static std::mutex slock;
		const boost::property_tree::ptree config;
		// private functions
		/// check system directories existance and create them if they are missing
		void check_dirs();
		/// update logic
		void update(const std::string &package);
		/*!
		 * \brief check cycle dependencies
		 * \throw std::runtime_error if found
		 */
		void check_cycle(const std::string &package);
		/*!
		 * \brief parallel update implementation
		 * \return true if package was updated
		 */
		bool dfs(const std::string &package, std::map<std::string, bool> &status);
	};
}}

#endif //REPOSITORY_HPP

