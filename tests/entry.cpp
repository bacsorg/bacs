#include <sstream>
#include <cassert>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "bunsan/pm/entry.hpp"

int main()
{
	using bunsan::pm::entry;
	std::string name = "some/long/name";
	entry e(name);
	assert(e.name()==name);
	assert(e.location().string()==name);
	assert(e.remote_resource("/repo", "res")=="/repo/"+name+"/res");
	assert(e.remote_resource("repo", "res")=="repo/"+name+"/res");
	assert(e.remote_resource("", "res")==name+"/res");
	assert(e.remote_resource("repo")=="repo/"+name);
	assert(e.remote_resource("")==name);
	boost::filesystem::path pname(name);
	assert(e.local_resource("")==pname);
	assert(e.local_resource("repo")=="repo"/pname);
	assert(e.local_resource("/repo")=="/repo"/pname);
	assert(e.local_resource("/repo", "o_O")=="/repo"/pname/"o_O");
	entry a("some//long/name");
	assert(a==e);
	using std::stringstream;
	using boost::property_tree::read_info;
	using boost::property_tree::ptree;
	stringstream in("some\n"
			"{\n"
			"\tlong\n"
			"\t{\n"
			"\t\tname \"hello world\""
			"\t}\n"
			"}\n");
	ptree pt;
	read_info(in, pt);
	assert(pt.get<std::string>(e.ptree_path())=="hello world");
}

