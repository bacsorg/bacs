#include "bunsan/pm/checksum.hpp"

#include "bunsan/system_error.hpp"

#include <sstream>
#include <iomanip>

#include <cstdio>

#include <boost/filesystem/fstream.hpp>
#include <boost/crc.hpp>

namespace
{
    std::string crc32_checksum(const boost::filesystem::path &file)
    {
        boost::crc_32_type crc;
        boost::filesystem::ifstream in(file, std::ios_base::binary);
        if (!in.is_open())
            BOOST_THROW_EXCEPTION(bunsan::system_error("open") << bunsan::error::message(file.string()));
        char buf[BUFSIZ];
        do
        {
            in.read(buf, BUFSIZ);
            crc.process_bytes(buf, in.gcount());
        }
        while (in);
        if (in.bad())
            BOOST_THROW_EXCEPTION(bunsan::system_error("read") << bunsan::error::message(file.string()));
        std::stringstream out;
        out<<std::hex<<std::uppercase<<crc.checksum();
        return out.str();
    }
}

#include "cryptopp/sha.h"

namespace
{
    template <typename HASH>
    std::string CryptoPP_checksum(const boost::filesystem::path &file)
    {
        byte buf[BUFSIZ];
        static_assert(sizeof(byte)==sizeof(char), "size of byte have to be equal size of char");
        HASH hash;
        boost::filesystem::ifstream in(file, std::ios_base::binary);
        if (!in.is_open())
            BOOST_THROW_EXCEPTION(bunsan::system_error("open") << bunsan::error::message(file.string()));
        do
        {
            in.read(reinterpret_cast<char *>(buf), BUFSIZ);
            hash.Update(buf, in.gcount());
        }
        while (in);
        if (in.bad())
            BOOST_THROW_EXCEPTION(bunsan::system_error("read") << bunsan::error::message(file.string()));
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
