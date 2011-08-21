#ifndef BUNSAN_PM_REPOSITORY_CONFIG_HPP
#define BUNSAN_PM_REPOSITORY_CONFIG_HPP

namespace
{
	constexpr const char *child_source = "source";
	constexpr const char *child_import_source = "import.source";
	constexpr const char *child_import_package = "import.package";

	constexpr const char *dir_tmp = "dir.tmp";
	constexpr const char *dir_package = "dir.package";
	constexpr const char *dir_source = "dir.source";

	constexpr const char *name_file_tmp = "name.file.tmp";
	constexpr const char *name_file_index = "name.file.index";
	constexpr const char *name_file_checksum = "name.file.checksum";
	constexpr const char *name_file_snapshot = "name.file.snapshot";
	constexpr const char *name_file_pkg = "name.file.pkg";

	constexpr const char *name_dir_pkg = "name.dir.pkg";
	constexpr const char *name_dir_source = "name.dir.source";
	constexpr const char *name_dir_build = "name.dir.build";

	constexpr const char *suffix_src = "suffix.src";
	constexpr const char *suffix_pkg = "suffix.pkg";

	constexpr const char *command_fetch = "command.fetch";
	constexpr const char *command_unpack = "command.unpack";
	constexpr const char *command_configure = "command.configure";
	constexpr const char *command_compile = "command.compile";
	constexpr const char *command_pack = "command.pack";
	constexpr const char *command_extract = "command.extract";

	constexpr const char *repository_url = "repository";
}

#endif //BUNSAN_PM_REPOSITORY_CONFIG_HPP

