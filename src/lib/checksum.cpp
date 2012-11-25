#include "bunsan/pm/checksum.hpp"
#include "bunsan/filesystem/fstream.hpp"
#include "bunsan/enable_error_info.hpp"

#include <sstream>
#include <iomanip>

#include <cstdio>

#include <boost/crc.hpp>

namespace
{
    std::string crc32_checksum(const boost::filesystem::path &file)
    {
        boost::crc_32_type crc;
        std::stringstream out;
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream in(file, std::ios_base::binary);
            char buf[BUFSIZ];
            do
            {
                in.read(buf, BUFSIZ);
                crc.process_bytes(buf, in.gcount());
            }
            while (in);
            in.close();
            out << std::hex << std::uppercase << crc.checksum();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
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
        static_assert(sizeof(byte) == sizeof(char), "size of byte have to be equal size of char");
        HASH hash;
        std::stringstream sout;
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream in(file, std::ios_base::binary);
            do
            {
                in.read(reinterpret_cast<char *>(buf), BUFSIZ);
                hash.Update(buf, in.gcount());
            }
            while (in);
            in.close();
            byte out[HASH::DIGESTSIZE];
            hash.Final(out);
            static_assert(sizeof(byte) <= sizeof(int), "size of byte have to be not greater than size of int");
            for (const byte i: out)
                sout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << int(i);
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
        return sout.str();
    }
}

std::string bunsan::pm::checksum(const boost::filesystem::path &file)
{
    return CryptoPP_checksum<CryptoPP::SHA512>(file);
}
