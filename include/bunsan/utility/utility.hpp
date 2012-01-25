#ifndef BUNSAN_UTILITY_UTILITY_HPP
#define BUNSAN_UTILITY_UTILITY_HPP

#include <boost/property_tree/ptree.hpp>

namespace bunsan{namespace utility
{
	/*!
	 * \brief Abstract class that specifies the way
	 * of options transmission + virtual destructor
	 */
	class utility
	{
	public:
		typedef boost::property_tree::ptree config_type;
	public:
		/*!
		 * \brief Set up utility with various options
		 *
		 * \todo what will happen to previous settings?
		 *
		 * You can find options documentation in utility docs
		 */
		virtual void setup(const config_type &config)=0;
		/*!
		 * \brief Does nothing
		 *
		 * Utility can have own destructor,
		 * we will provide such virtual destructor support
		 */
		virtual ~utility();
	};
}}

#endif //BUNSAN_UTILITY_UTILITY_HPP

