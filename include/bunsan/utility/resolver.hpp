#ifndef BUNSAN_UTILITY_RESOLVER_HPP
#define BUNSAN_UTILITY_RESOLVER_HPP

#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

namespace bunsan{namespace utility
{
	class resolver
	{
	public:
		/*!
		 * \brief Object will use os-specified algorithms
		 * for path resolving
		 */
		resolver();
		/*!
		 * \brief Object will use config-specified algorithms
		 * for path resolving
		 */
		explicit resolver(const boost::property_tree::ptree &config);
		resolver(const resolver &)=default;
		resolver(resolver &&) throw();
		resolver &operator=(const resolver &);
		resolver &operator=(resolver &&) throw();
		void swap(resolver &) throw();
		// interface
		boost::filesystem::path find_executable(
			const boost::filesystem::path &exe) const;
		boost::filesystem::path find_library(
			const boost::filesystem::path &lib) const;
	private:
		void apply_alias(boost::filesystem::path &name) const;
		void apply_absolute(boost::filesystem::path &name) const;
		void apply_path(boost::filesystem::path &name) const;
		struct config_type
		{
			boost::unordered_map<boost::filesystem::path, boost::filesystem::path>
				m_alias, m_absolute;
			boost::unordered_set<boost::filesystem::path> m_path;
		};
		static void swap(config_type &a, config_type &b) throw();
		boost::optional<config_type> m_config;
	};
	inline void swap(resolver &a, resolver &b) throw()
	{
		a.swap(b);
	}
}}

#endif //BUNSAN_UTILITY_RESOLVER_HPP

