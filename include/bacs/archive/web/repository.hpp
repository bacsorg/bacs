#pragma once

#include <bacs/archive/repository.hpp>
#include <bacs/archive/web/content/form.hpp>

#include <cppcms/application.h>

#include <memory>

namespace bacs {
namespace archive {
namespace web {

class repository : public cppcms::application {
 public:
  repository(cppcms::service &srv,
             const std::shared_ptr<archive::repository> &repository_);

  void main(std::string url) override;

 private:
  template <typename Model, typename Handler, typename Sender>
  void handler_wrapper(const std::string &name, const Handler &handler,
                       const Sender &sender);

  void upload();
  void download();
  void rename();
  void existing();
  void available();
  void status();
  void with_flag();
  void set_flags();
  void unset_flags();
  void clear_flags();
  void ignore();
  void import_result();
  void import();

  template <typename ProtoBuf>
  void send_protobuf(const ProtoBuf &protobuf, const std::string &filename);

  void send_file(const boost::filesystem::path &path,
                 const std::string &filename);

  void send_tempfile(bunsan::tempfile &&tmpfile, const std::string &filename);

 private:
  problem::StatusMap upload_(content::upload &data);
  // void download_(content::download &data);
  problem::StatusResult rename_(content::rename &data);
  problem::IdSet existing_(content::existing &data);
  problem::IdSet available_(content::available &data);
  problem::StatusMap status_(content::status &data);
  problem::IdSet with_flag_(content::with_flag &data);
  problem::StatusMap set_flags_(content::set_flags &data);
  problem::StatusMap unset_flags_(content::unset_flags &data);
  problem::StatusMap clear_flags_(content::clear_flags &data);
  problem::StatusMap ignore_(content::ignore &data);
  problem::ImportMap import_result_(content::import_result &data);
  problem::StatusMap import_(content::import &data);

 private:
  const std::shared_ptr<archive::repository> m_repository;
  const boost::filesystem::path m_upload_directory;
};

}  // namespace web
}  // namespace archive
}  // namespace bacs
