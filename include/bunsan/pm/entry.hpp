#ifndef BUNSAN_PM_ENTRY_HPP
#define BUNSAN_PM_ENTRY_HPP

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>

#include "bunsan/pm/error.hpp"

namespace bunsan{namespace pm
{
    struct invalid_entry_name: virtual error
    {
        // tags
        typedef boost::error_info<struct tag_entry_name, std::string> entry_name;
    };
    /// Represents \ref package_page "package" reference in repository.
    class entry
    {
    public:
        static constexpr char def_delim = '/';
    public:
        // construction
        entry()=default;
        entry(const entry &)=default;
        entry(entry &&)=default;
        entry &operator=(const entry &e);
        entry &operator=(entry &&e) noexcept;
        void swap(entry &e) noexcept;
        // conversions
        entry(const std::string &name_, char delim=def_delim);
        entry(const char *name_, char delim=def_delim);
        entry &operator=(const std::string &name_);
        // comparisons
        bool operator==(const entry &e) const;
        bool operator<(const entry &e) const;
        // concatenation
        entry operator/(const entry &e) const;
        // entry conversion getters
        boost::filesystem::path location() const;
        std::string name(char delim=def_delim) const;
        std::string name(const std::string &delim) const;
        const std::vector<std::string> &decomposition() const;
        boost::property_tree::ptree::path_type ptree_path() const;
        // resource getters
        /// \returns repository/name('/')/name_
        std::string remote_resource(
            const std::string &repository,
            const boost::filesystem::path &name_=boost::filesystem::path()) const;
        /// \returns dir/name('/')/name_
        boost::filesystem::path local_resource(
            const boost::filesystem::path &dir,
            const boost::filesystem::path &name_=boost::filesystem::path()) const;
        // static helpers
        static bool is_allowed_subpath(const std::string &subpath);
        static bool is_allowed_symbol(char c);
    private:
        void build(const std::string &name_, char delim=def_delim);
        std::vector<std::string> m_location;
    };
    inline void swap(entry &e1, entry &e2) noexcept
    {
        e1.swap(e2);
    }
    template <typename Char, typename Traits>
    inline std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &out, const entry &e)
    {
        return out<<e.location();
    }
}}

#endif //BUNSAN_PM_ENTRY_HPP

