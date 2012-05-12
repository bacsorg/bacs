#ifndef SRC_FETCHERS_CURL_HPP
#define SRC_FETCHERS_CURL_HPP

#include "bunsan/utility/fetcher.hpp"

namespace bunsan{namespace utility{namespace fetchers
{
    class curl: public fetcher
    {
        const boost::filesystem::path m_exe;
    public:
        explicit curl(const boost::filesystem::path &exe);
        virtual void fetch(const std::string &uri, const boost::filesystem::path &dst);
    private:
        static bool factory_reg_hook;
    };
}}}

#endif //SRC_FETCHERS_CURL_HPP

