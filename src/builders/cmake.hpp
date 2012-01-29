#ifndef SRC_BUILDERS_CMAKE_HPP
#define SRC_BUILDERS_CMAKE_HPP

#include "conf_make_install.hpp"

namespace bunsan{namespace utility{namespace builders
{
	class cmake: public conf_make_install
	{
		const resolver m_resolver;
		const boost::filesystem::path m_cmake_exe;
	public:
		explicit cmake(const resolver &resolver_);
		virtual void setup(const utility::config_type &config);
	protected:
		std::vector<std::string> argv_(
			const boost::filesystem::path &src);
		virtual void configure_(
			const boost::filesystem::path &src,
			const boost::filesystem::path &bin);
		virtual void make_(
			const boost::filesystem::path &src,
			const boost::filesystem::path &bin);
		virtual void install_(
			const boost::filesystem::path &src,
			const boost::filesystem::path &bin,
			const boost::filesystem::path &root);
	private:
		static bool factory_reg_hook;
	};
}}}

#endif //SRC_BUILDERS_CMAKE_HPP

