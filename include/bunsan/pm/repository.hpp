#ifndef BUNSAN_PM_REPOSITORY_HPP
#define BUNSAN_PM_REPOSITORY_HPP

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
#include "bunsan/pm/error.hpp"

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
		/// Extract value from config as Ret
		template <typename Ret, typename Path, typename ... Args>
		Ret config_get(const Path &path, Args &&...args) const;
		template <typename Ret, typename ... Args>
		boost::optional<Ret> config_get_optional(Args &...args)
		{
			return config.get_optional<Ret>(std::forward<Args>(args)...);
		}
		/// Extract value from config as boost::property_tree::ptree
		template <typename Path, typename ... Args>
		boost::property_tree::ptree config_get_child(const Path &path, Args &&...args) const;
		template <typename ... Args>
		boost::optional<boost::property_tree::ptree> config_get_child(Args &&...args) const
		{
			return config.get_child_optional(std::forward<Args>(args)...);
		}
		~repository();
	private:
		template <typename Path>
		void config_get_wrap_exception(const Path &path)
		{
			BOOST_THROW_EXCEPTION(
				invalid_configuration_path()
				<<invalid_configuration_path::path(path));
		}
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
		enum class stage_type: int;
		typedef std::pair<entry, stage_type> stage;
		bool update_package_depends(
			const stage &stage_,
			std::map<stage, bool> &updated,
			std::set<stage> &in,
			std::map<entry, boost::property_tree::ptree> &snapshot,
			std::map<stage, std::map<entry, boost::property_tree::ptree>> &snapshot_cache);
	};

	template <typename Ret, typename Path, typename ... Args>
	Ret config_get(const Path &path, Args &&...args) const
	{
		try
		{
			return config.get<Ret>(std::forward<Args>(args)...);
		}
		catch (boost::property_tree::ptree_bad_path &)
		{
			config_get_wrap_exception(path);
		}
	}
	template <typename Path, typename ... Args>
	boost::property_tree::ptree config_get_child(const Path &path, Args &&...args) const
	{
		try
		{
			return config.get_child(std::forward<Args>(args)...);
		}
		catch (boost::property_tree::ptree_bad_path &)
		{
			config_get_wrap_exception(path);
		}
	}
}}

#endif //BUNSAN_PM_REPOSITORY_HPP

