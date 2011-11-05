#ifndef BUNSAN_PM_ENTRY_HPP
#define BUNSAN_PM_ENTRY_HPP

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bunsan{namespace pm
{
	/// Represents \ref package_page "package" reference in repository.
	class entry
	{
	public:
		entry()=default;
		entry(const std::string &name_);
		entry(const char *name_);
		entry(const entry &e);
		entry &operator=(const entry &e);
		entry &operator=(const std::string &name_);
		bool operator==(const entry &e) const;
		bool operator<(const entry &e) const;
		boost::filesystem::path location() const;
		void swap(entry &e) throw();
		std::string name(char delimiter='/') const;
		std::vector<std::string> decomposition() const;
		boost::property_tree::ptree::path_type ptree_path() const;
		std::string remote_resource(const std::string &repository, const boost::filesystem::path &name=boost::filesystem::path()) const;
		boost::filesystem::path local_resource(const boost::filesystem::path &dir, const boost::filesystem::path &name=boost::filesystem::path()) const;
		static bool is_allowed_subpath(const std::string &subpath);
		static bool is_allowed_symbol(char c);
	private:
		void build(const std::string &name_);
		boost::filesystem::path location_;
		template <typename Char, typename Traits>
		friend inline std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &out, const entry &e);
	};
	inline void swap(entry &e1, entry &e2) throw()
	{
		e1.swap(e2);
	}
	template <typename Char, typename Traits>
	inline std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &out, const entry &e)
	{
		return out<<e.location_;
	}
}}

#endif //BUNSAN_PM_ENTRY_HPP

