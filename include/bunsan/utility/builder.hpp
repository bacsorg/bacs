#ifndef BUNSAN_UTILITY_BUILDER_HPP
#define BUNSAN_UTILITY_BUILDER_HPP

#include "bunsan/factory_helper.hpp"
#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/utility.hpp"
#include "bunsan/utility/archiver.hpp"

namespace bunsan{namespace utility
{
	class builder: public utility
	BUNSAN_FACTORY_BEGIN(builder, const resolver &)
	public:
		/*!
		 * \brief Build and install package
		 *
		 * \param src source files
		 * \param bin temporary directory for package building
		 * \param root package will be installed into root directory
		 */
		virtual void install(
			const boost::filesystem::path &src,
			const boost::filesystem::path &bin,
			const boost::filesystem::path &root)=0;
		/*!
		 * \brief Build and pack package
		 *
		 * Default implementation will call install
		 * with root set to temporary directory inside bin
		 * and pack it content using archiver::pack_content
		 */
		virtual void pack(
			const boost::filesystem::path &src,
			const boost::filesystem::path &bin,
			const boost::filesystem::path &archive,
			const archiver_ptr &archiver_);
	BUNSAN_FACTORY_END(builder)
}}

#endif //BUNSAN_UTILITY_BUILDER_HPP

