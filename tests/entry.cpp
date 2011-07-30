#include <cassert>

#include "bunsan/pm/entry.hpp"

int main()
{
	using bunsan::pm::entry;
	std::string name = "some/long/name";
	entry e(name);
	assert(e.name()==name);
	assert(e.location().generic_string()==name);
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
}

