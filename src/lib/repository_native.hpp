#ifndef REPOSITORY_NATIVE_HPP
#define REPOSITORY_NATIVE_HPP

#include "bunsan/pm/repository.hpp"

class bunsan::pm::repository::native: private boost::noncopyable
{
public:
	explicit native(const boost::property_tree::ptree &config_);
	void fetch_source(const entry &package);
	void update_index(const entry &package);
	/*!
	 * \brief unpack, configure, compile, pack
	 */
	void build(const entry &package);
private:
	void unpack(const entry &package, const boost::filesystem::path &build_dir);
	void configure(const entry &package, const boost::filesystem::path &build_dir);
	void compile(const entry &package, const boost::filesystem::path &build_dir);
	void pack(const entry &package, const boost::filesystem::path &build_dir);
public:
	std::map<std::string, entry> depends(const entry &package);
	std::multimap<boost::filesystem::path, entry> imports(const entry &package);
	bool package_outdated(const entry &package);
	void extract(const entry &package, const boost::filesystem::path &destination);
private:
	const boost::property_tree::ptree &config;
};

#endif //REPOSITORY_NATIVE_HPP

