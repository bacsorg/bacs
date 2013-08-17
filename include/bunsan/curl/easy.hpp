#pragma once

#include <curl/curl.h>

#include <string>

namespace bunsan{namespace curl
{
    class easy
    {
    public:
        /// Create invalid object.
        easy(std::nullptr_t) noexcept;

        /// Destroy current object making it invalid.
        easy &operator=(std::nullptr_t) noexcept;

        /// \note Takes ownership.
        explicit easy(CURL *const curl) noexcept;

        easy(const easy &)=delete;
        easy &operator=(const easy &)=delete;

        easy(easy &&) noexcept;
        easy &operator=(easy &&) noexcept;

        void swap(easy &) noexcept;

        explicit operator bool() const noexcept;

        easy();
        ~easy();

        std::string escape(const std::string &url);
        std::string unescape(const std::string &url);

        void perform();
        void pause(const int bitmask);

    private:
        void init() noexcept;

    private:
        /// \note implementation can use m_curl == nullptr internally
        CURL *m_curl;
    };

    inline void swap(easy &a, easy &b) noexcept
    {
        a.swap(b);
    }
}}
