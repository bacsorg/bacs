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
		 *
		 * Default implementation will call
		 * setarg(i.first, i.second.get_value<std::string>())
		 * for every config child
		 */
		virtual void setup(const config_type &config);
		/*!
		 * \brief Set up utility option
		 *
		 * Will override option value with same key
		 */
		virtual void setarg(const std::string &key, const std::string &value)=0;
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

