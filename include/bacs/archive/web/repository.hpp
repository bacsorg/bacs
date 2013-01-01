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

    private:
        template <typename Model, typename Result, typename ProtoResult, bool (repository::*Handler)(Model &, Result &)>
        void handler_wrapper(const std::string &name);

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
        bool insert(content::insert &data, problem::import_map &result);
        bool extract(content::extract &data, std::string &result);
        bool rename(content::rename &data, problem::import_info &result);
        bool existing(content::existing &data, problem::id_set &result);
        bool available(content::available &data, problem::id_set &result);
        bool status(content::status &data, problem::status_map &result);
        bool with_flag(content::with_flag &data, problem::id_set &result);
        bool set_flags(content::set_flags &data, problem::id_set &result);
        bool unset_flags(content::unset_flags &data, problem::id_set &result);
        bool clear_flags(content::clear_flags &data, problem::id_set &result);
        bool ignore(content::ignore &data, problem::id_set &result);
        bool info(content::info &data, problem::info_map &result);
        bool hash(content::hash &data, problem::hash_map &result);
        bool repack(content::repack &data, problem::import_map &result);

    private:
        const std::shared_ptr<archive::repository> m_repository;
    };
}}}
