#ifndef BUNSAN_PM_CONFIG_HPP
#define BUNSAN_PM_CONFIG_HPP

namespace bunsan{namespace pm{namespace config
{
	namespace dir
	{
		constexpr const char *tmp = "dir.tmp";
		constexpr const char *package = "dir.package";
		constexpr const char *source = "dir.source";
	}
	namespace name
	{
		namespace file
		{
			constexpr const char *tmp = "name.file.tmp";
			constexpr const char *index = "name.file.index";
			constexpr const char *checksum = "name.file.checksum";
			constexpr const char *snapshot = "name.file.snapshot";
			constexpr const char *pkg = "name.file.pkg";
		}
		namespace dir
		{
			constexpr const char *pkg = "name.dir.pkg";
			constexpr const char *source = "name.dir.source";
			constexpr const char *build = "name.dir.build";
		}
	}
	namespace suffix
	{
		constexpr const char *src = "suffix.src";
		constexpr const char *pkg = "suffix.pkg";
	}
	namespace command
	{
		constexpr const char *fetch = "command.fetch";
		constexpr const char *unpack = "command.unpack";
		constexpr const char *configure = "command.configure";
		constexpr const char *compile = "command.compile";
		constexpr const char *pack = "command.pack";
		constexpr const char *extract = "command.extract";
		constexpr const char *create = "command.create";
	}
	constexpr const char *repository_url = "repository";
}}}

#endif //BUNSAN_PM_CONFIG_HPP

