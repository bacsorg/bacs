#pragma once

#include "bacs/archive/repository.hpp"
#include "bacs/archive/web/content/form.hpp"

#include <cppcms/application.h>

#include <memory>

namespace bacs{namespace archive{namespace web
{
    class repository: public cppcms::application
    {
    public:
        repository(cppcms::service &srv,
                   const std::shared_ptr<archive::repository> &repository_);

        void main(std::string url) override;

    private:
        template <typename Model>
        class void_result;

        template <typename Model, typename Result, typename ProtoResult>
        class convertible_result;

        template <typename Model>
        friend class repository::void_result;

        template <typename Model, typename Result, typename ProtoResult>
        friend class repository::convertible_result;

        template <typename Model, typename Result>
        void handler_wrapper(const std::string &name, Result &result);

        void insert();
        void extract();
        void rename();
        void existing();
        void available();
        void status();
        void with_flag();
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
        problem::import_map insert_(content::insert &data);
        //void extract_(content::extract &data);
        problem::import_info rename_(content::rename &data);
        problem::id_set existing_(content::existing &data);
        problem::id_set available_(content::available &data);
        problem::status_map status_(content::status &data);
        problem::id_set with_flag_(content::with_flag &data);
        problem::id_set set_flags_(content::set_flags &data);
        problem::id_set unset_flags_(content::unset_flags &data);
        problem::id_set clear_flags_(content::clear_flags &data);
        problem::id_set ignore_(content::ignore &data);
        problem::info_map info_(content::info &data);
        problem::hash_map hash_(content::hash &data);
        problem::import_map repack_(content::repack &data);

    private:
        const std::shared_ptr<archive::repository> m_repository;
        const boost::filesystem::path m_upload_directory;
    };
}}}
