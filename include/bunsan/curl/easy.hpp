#pragma once

#include <curl/curl.h>

#include <boost/noncopyable.hpp>

#include <string>

namespace bunsan{namespace curl
{
    class easy: private boost::noncopyable // TODO really?
    {
    public:
        /// \note Takes ownership.
        explicit easy(CURL *const curl) noexcept;

        easy();
        ~easy();

        std::string escape(const std::string &url);
        std::string unescape(const std::string &url);

        void perform();
        void pause(const int bitmask);

    private:
        CURL *const m_curl;
    };
}}
