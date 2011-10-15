#include "bunsan/pm/checksum.hpp"

#include <sstream>
#include <iomanip>

#include <boost/filesystem/fstream.hpp>

#include <boost/crc.hpp>

#include "bunsan/system_error.hpp"

namespace
{
	std::string crc32_checksum(const boost::filesystem::path &file)
	{
		boost::crc_32_type crc;
		boost::filesystem::ifstream in(file, std::ios_base::binary);
		if (!in.is_open())
			bunsan::system_error("open file: \""+file.native()+"\"");
		constexpr size_t bufsize = 1024;
		char buf[bufsize];
		do
		{
			in.read(buf, bufsize);
			crc.process_bytes(buf, in.gcount());
		}
		while (in);
		if (in.bad())
			bunsan::system_error("reading from file: \""+file.native()+"\"");
		std::stringstream out;
		out<<std::hex<<std::uppercase<<crc.checksum();
		return out.str();
	}
}

#include <cryptopp/sha.h>

namespace
{
	template <typename HASH>
	std::string CryptoPP_checksum(const boost::filesystem::path &file)
	{
		constexpr size_t bufsize = 1024;
		byte buf[bufsize];
		static_assert(sizeof(byte)==sizeof(char), "size of byte have to be equal size of char");
		HASH hash;
		boost::filesystem::ifstream in(file, std::ios_base::binary);
		if (!in.is_open())
			bunsan::system_error("open file: \""+file.native()+"\"");
		do
		{
			in.read(reinterpret_cast<char *>(buf), bufsize);
			hash.Update(buf, in.gcount());
		}
		while (in);
		if (in.bad())
			bunsan::system_error("reading from file: \""+file.native()+"\"");
		byte out[HASH::DIGESTSIZE];
		hash.Final(out);
		std::stringstream sout;
		static_assert(sizeof(byte)<=sizeof(int), "size of byte have to be not greater than size of int");
		for (byte i: out)
			sout<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<int(i);
		return sout.str();
	}
}

std::string bunsan::pm::checksum(const boost::filesystem::path &file)
{
	return CryptoPP_checksum<CryptoPP::SHA512>(file);
}

