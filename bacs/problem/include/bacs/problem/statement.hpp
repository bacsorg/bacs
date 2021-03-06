#pragma once

#include <bacs/problem/buildable.hpp>
#include <bacs/problem/id.hpp>
#include <bacs/problem/problem.pb.h>

#include <bunsan/factory_helper.hpp>
#include <bunsan/forward_constructor.hpp>

#include <boost/filesystem/path.hpp>

#include <unordered_set>

namespace bacs::problem {

struct statement_error : virtual buildable_error {};
struct statement_version_error : virtual statement_error {};
struct statement_version_make_package_error
    : virtual statement_version_error,
      virtual buildable_make_package_error {
  using resources_package =
      boost::error_info<struct tag_resources_package, bunsan::pm::entry>;
};
struct invalid_statement_language_error : virtual invalid_id_error,
                                          virtual statement_error {
  using language = boost::error_info<struct tag_language, std::string>;
};
struct invalid_statement_format_error : virtual invalid_id_error,
                                        virtual statement_error {
  using format = boost::error_info<struct tag_format, std::string>;
};

class statement : public buildable {
 public:
  class version : private boost::noncopyable {
    BUNSAN_FACTORY_BODY(version, const boost::filesystem::path &location,
                        const boost::property_tree::ptree &config)
   public:
    /// Built statement version.
    class built {
     public:
      explicit built(const boost::filesystem::path &package_root);

      const boost::filesystem::path &package_root() const {
        return m_package_root;
      }

      const Statement::Version::Manifest &manifest() const {
        return m_manifest;
      }

      boost::filesystem::path data_root() const;

      boost::filesystem::path index() const;

     private:
      const boost::filesystem::path m_package_root;
      const Statement::Version::Manifest m_manifest;
    };

   public:
    /// \note config_location.parent_path() is location
    static version_ptr instance(const boost::filesystem::path &config_location);

   public:
    version(const std::string &language, const std::string &format);

    virtual ~version();

    virtual void make_package(const boost::filesystem::path &destination,
                              const bunsan::pm::entry &package,
                              const bunsan::pm::entry &resources_package,
                              const Revision &revision) const = 0;

    /// \warning package name is relative to statement version package
    virtual Statement::Version info() const;

   public:
    virtual std::string language() const;
    virtual std::string format() const;

    /// relative unique entry name for this statement version
    virtual bunsan::pm::entry subpackage() const;

   protected:
    friend class built;
    static const boost::filesystem::path manifest_path, data_path;

   private:
    std::string m_language;
    std::string m_format;
  };
  BUNSAN_FACTORY_TYPES(version)

  using statement_ptr = std::shared_ptr<statement>;

 public:
  static statement_ptr instance(const boost::filesystem::path &location);

 protected:
  statement(const boost::filesystem::path &location,
            const std::vector<version_ptr> &versions);

 private:
  void update_info();

 public:
  bool make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry &package,
                    const Revision &revision) const override;

  /// \warning package names are relative to statement package
  const Statement &info() const;

 private:
  const boost::filesystem::path m_location;
  std::vector<version_ptr> m_versions;
  Statement m_info;
};

using statement_ptr = statement::statement_ptr;

}  // namespace bacs::problem
