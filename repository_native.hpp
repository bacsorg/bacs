#ifndef REPOSITORY_NATIVE_HPP
#define REPOSITORY_NATIVE_HPP

#include "bunsan/pm/repository.hpp"

class bunsan::pm::repository::native: private boost::noncopyable
{
public:
	explicit native(const boost::property_tree::ptree &config_);
	void fetch_source(const std::string &package);
	bool update_meta(const std::string &package);
	/*!
	 * \brief unpack, configure, compile, pack
	 */
	void build(const std::string &package);
private:
	void unpack(const std::string &package, const boost::filesystem::path &build_dir);
	void configure(const std::string &package, const boost::filesystem::path &build_dir);
	void compile(const std::string &package, const boost::filesystem::path &build_dir);
	void pack(const std::string &package, const boost::filesystem::path &build_dir);
public:
	std::vector<std::string> depends(const std::string &package);
	std::map<std::string, std::string> depend_keys(const std::string &package);
	bool source_outdated(const std::string &package);
	bool package_outdated(const std::string &package);
	void extract(const std::string &package, const boost::filesystem::path &destination);
private:
	const boost::property_tree::ptree &config;
};

#endif //REPOSITORY_NATIVE_HPP

