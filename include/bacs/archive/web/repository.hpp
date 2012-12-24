#pragma once

#include "bacs/archive/repository.hpp"

#include <cppcms/application.h>

#include <memory>

namespace bacs{namespace archive{namespace web
{
    class repository: public cppcms::application
    {
    public:
        repository(cppcms::service &srv,
                   const std::shared_ptr<archive::repository> &repository_);

    private:
        void insert();
        void extract();
        void rename();
        void existing();
        void available();
        void status();
        void has_flag();
        void set_flags();
        void unset_flags();
        void clear_flags();
        void ignore();
        void info();
        void hash();
        void repack();

    private:
        const std::shared_ptr<archive::repository> m_repository;
    };
}}}
