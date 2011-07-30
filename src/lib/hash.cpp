#include "bunsan/pm/hash.hpp"

#include <sstream>

#include <boost/filesystem/fstream.hpp>
#include <boost/crc.hpp>

std::string bunsan::pm::hash(const boost::filesystem::path &file)
{
	boost::crc_32_type crc;
	boost::filesystem::ifstream in(file, std::ios_base::binary);// TODO error control
	constexpr size_t bufsize = 1024;
	char buf[bufsize];
	do
	{
		in.read(buf, bufsize);
		crc.process_bytes(buf, in.gcount());
	}
	while (in);
	std::stringstream out;
	out<<std::hex<<std::uppercase<<crc.checksum();
	return out.str();
}

