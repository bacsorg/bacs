#ifndef SRC_BUILDERS_CMAKE_HPP
#define SRC_BUILDERS_CMAKE_HPP

#include <map>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "bunsan/utility/maker.hpp"

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
		struct generator
		{
			enum class type: std::size_t;
			const std::string m_id;
			const type m_type;
		};
	private:
		void setup_generator();
		void set_generator(const std::string &gen_id);
		const generator &get_generator() const;
		std::vector<std::string> argv_(
			const boost::filesystem::path &src) const;
	private:
		boost::optional<std::size_t> m_generator;
		std::map<std::string, std::string> m_cmake_defines;
		utility::config_type m_make_maker_config, m_install_maker_config;
		static const std::vector<generator> generators;
		static bool factory_reg_hook;
	};
}}}

#endif //SRC_BUILDERS_CMAKE_HPP

