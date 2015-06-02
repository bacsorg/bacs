#pragma once

#include <bunsan/utility/resolver.hpp>

#include <turtle/mock.hpp>

namespace bunsan{namespace utility
{
    MOCK_BASE_CLASS(mock_resolver, resolver)
    {
        MOCK_METHOD(find_executable, 1,
                    boost::filesystem::path (
                        const boost::filesystem::path &exe))
        MOCK_METHOD(find_library, 1,
                    boost::filesystem::path (
                        const boost::filesystem::path &lib))

        MOCK_METHOD(clone, 0,
                    std::unique_ptr<resolver> ())
    };
}}
