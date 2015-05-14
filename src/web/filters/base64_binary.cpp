#include <bacs/archive/web/filters/base64_binary.hpp>

#include <bunsan/crypto/base64.hpp>

namespace cppcms{namespace filters
{
    void base64_binary::operator()(std::ostream &out) const
    {
        out << bunsan::crypto::base64::encode(m_binary);
    }
}}
