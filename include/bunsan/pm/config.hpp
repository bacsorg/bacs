#pragma once

namespace bunsan{namespace pm{namespace config
{
    namespace lock
    {
        constexpr const char *global = "lock.global";   ///< global lock for synchronization
    }
    namespace dir
    {
        constexpr const char *tmp = "dir.tmp";          ///< directory for large temporary files
        constexpr const char *package = "dir.package";  ///< directory for built packages
        constexpr const char *source = "dir.source";    ///< directory for package sources
    }
    /// file and directory names, usually relative
    namespace name
    {
        namespace file
        {
            constexpr const char *tmp = "name.file.tmp";                                    ///< file mask for small temporary files,
                                                                                            ///< should be absolute
            constexpr const char *index = "name.file.index";                                ///< index file name
            constexpr const char *checksum = "name.file.checksum";                          ///< checksum file name
            constexpr const char *build_snapshot = "name.file.build_snapshot";              ///< build snapshot file name
            constexpr const char *installation_snapshot = "name.file.installation_snapshot";///< installation snapshot file name
            constexpr const char *build = "name.file.build";                                ///< build archive name
            constexpr const char *installation = "name.file.installation";                  ///< installation archive name
        }
        namespace dir
        {
            constexpr const char *source = "name.dir.source";             ///< subdirectory for package source
            constexpr const char *build = "name.dir.build";               ///< subdirectory for package building
            constexpr const char *installation = "name.dir.installation"; ///< subdirectory for package installation
        }
    }
    namespace suffix
    {
        constexpr const char *source_archive = "suffix.source_archive";   ///< suffix of archive files
    }
    namespace command
    {
        constexpr const char *resolver = "utility.resolver";
        namespace builder
        {
            constexpr const char *type = "utility.builder.type";
            constexpr const char *config = "utility.builder.config";
        }
        namespace fetcher
        {
            constexpr const char *type = "utility.fetcher.type";
            constexpr const char *config = "utility.fetcher.config";
        }
        namespace source_archiver
        {
            constexpr const char *type = "utility.source_archiver.type";
            constexpr const char *config = "utility.source_archiver.config";
        }
        namespace cache_archiver
        {
            constexpr const char *type = "utility.cache_archiver.type";
            constexpr const char *config = "utility.cache_archiver.config";
        }
    }
    constexpr const char *repository_url = "repository_url"; ///< repository location
}}}
