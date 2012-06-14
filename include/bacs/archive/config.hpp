#ifndef BACS_ARCHIVE_CONFIG_HPP
#define BACS_ARCHIVE_CONFIG_HPP

namespace bacs{namespace archive{namespace config
{
    constexpr const char *lock = "lock";
    constexpr const char *resolver = "resolver";
    constexpr const char *tmpdir = "tmpdir";
    namespace problem
    {
        namespace archiver
        {
            constexpr const char *type = "problem.archiver.type";
            constexpr const char *format = "problem.archiver.format";
        }
    }
}}}

#endif //BACS_ARCHIVE_CONFIG_HPP

