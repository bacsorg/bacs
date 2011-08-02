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
	/// check system directories existance and create them if they are missing
	void check_dirs();
	void clean();
private:
	void unpack_import(const entry &package, const boost::filesystem::path &destination, boost::property_tree::ptree &snapshot);
	void unpack(const entry &package, const boost::filesystem::path &build_dir);
	void configure(const entry &package, const boost::filesystem::path &build_dir);
	void compile(const entry &package, const boost::filesystem::path &build_dir);
	void pack(const entry &package, const boost::filesystem::path &build_dir);
	std::string remote_resource(const entry &package, const std::string &name);
	boost::filesystem::path local_resource(const entry &package, const std::string &name);
	std::string value(const std::string &key);
	void read_index(const entry &package, boost::property_tree::ptree &ptree);
	void read_checksum(const entry &package, boost::property_tree::ptree &ptree);
public:
	std::map<std::string, entry> depends(const entry &package);
	std::multimap<boost::filesystem::path, entry> imports(const entry &package);
	bool package_outdated(const entry &package);
	void extract(const entry &package, const boost::filesystem::path &destination);
private:
	const boost::property_tree::ptree &config;
};

#endif //REPOSITORY_NATIVE_HPP

