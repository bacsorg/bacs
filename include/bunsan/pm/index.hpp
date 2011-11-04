#ifndef BUNSAN_PM_INDEX_HPP
#define BUNSAN_PM_INDEX_HPP

namespace bunsan{namespace pm{namespace index
{
	constexpr const char *package = "package";
	namespace source
	{
		namespace import
		{
			constexpr const char *package = "source.import.package";
			constexpr const char *source = "source.import.source";
		}
		constexpr const char *self = "source.self";
	}
}}}

#endif //BUNSAN_PM_INDEX_HPP

