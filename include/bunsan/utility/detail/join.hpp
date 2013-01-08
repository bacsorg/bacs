#pragma once

#include <vector>
#include <utility>

#include <boost/optional.hpp>

namespace bunsan{namespace utility{namespace detail
{
    template <typename T>
    struct joiner
    {
        template <typename P>
        static void join(std::vector<T> &v, P &&obj)
        {
            v.emplace_back(std::forward<P>(obj));
        }

        template <typename P>
        static void join(std::vector<T> &v, boost::optional<P> &&obj)
        {
            if (obj)
                join(v, std::move(obj.get()));
        }

        template <typename P>
        static void join(std::vector<T> &v, const boost::optional<P> &obj)
        {
            if (obj)
                join(v, obj.get());
        }

        template <typename P>
        static void join(std::vector<T> &v, std::vector<P> &&lst)
        {
            for (P &obj: lst)
                join(v, std::move(obj));
        }

        template <typename P>
        static void join(std::vector<T> &v, const std::vector<P> &lst)
        {
            for (const P &obj: lst)
                join(v, obj);
        }

        template <typename Arg1, typename Arg2, typename ... Args>
        static void join(std::vector<T> &v, Arg1 &&arg1, Arg2 &&arg2, Args &&...args)
        {
            join(v, std::forward<Arg1>(arg1));
            join(v, std::forward<Arg2>(arg2), std::forward<Args>(args)...);
        }
    };

    template <typename T, typename ... Args>
    std::vector<T> join(Args &&...args)
    {
        std::vector<T> v;
        joiner<T>::join(v, std::forward<Args>(args)...);
        return v;
    }
}}}
