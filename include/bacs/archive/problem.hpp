#ifndef BACS_ARCHIVE_PROBLEM_HPP
#define BACS_ARCHIVE_PROBLEM_HPP

namespace bacs{namespace archive{namespace problem
{
    /// problem id
    typedef std::string id;
    typedef std::vector<unsigned char> binary;
    /// information about problem, see problem.xsd from BACS.XSD
    typedef binary info;
    /// package name
    typedef bunsan::pm::entry package;
    /// hash string
    typedef binary hash;
    typedef std::vector<id> id_list;
    typedef std::vector<info> info_list;
    typedef std::vector<package> package_list;
    typedef std::vector<hash> hash_list;
    struct import_info
    {
        bool ok;
        std::string error;
    };
    struct archive_format
    {
        std::string archiver;
        boost::optional<std::string> format;
    };
    typedef std::map<id, import_info> import_map;
}}}

#endif //BACS_ARCHIVE_PROBLEM_HPP

