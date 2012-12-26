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

        template <typename ProtoBuf>
        void send_protobuf(const ProtoBuf &protobuf, const std::string &filename);

        void send_file(const boost::filesystem::path &path, const std::string &filename);

        void send_tempfile(bunsan::tempfile &&tmpfile, const std::string &filename);

    private:
        const std::shared_ptr<archive::repository> m_repository;
    };
}}}
