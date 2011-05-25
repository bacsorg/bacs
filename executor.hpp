#include <stdexcept>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/variant.hpp>
#include <boost/filesystem/path.hpp>

#include "util.hpp"
namespace its
{
	class return_code: public std::runtime_error
	{
	public:
		explicit return_code(int code_);
		//virtual const char *what() const throw();
		operator int() const throw();
	private:
		int code;
	};
	class executor
	{
	public:
		explicit executor(const std::string &command);
		explicit executor(const boost::property_tree::ptree &command);
		executor(const executor &)=default;
		executor(executor &&)=default;
		executor &operator=(const executor &)=default;
		executor &operator=(executor &&)=default;
		/*!
		 * \brief run command with specified arguments
		 * \throws return_code if return code is not zero
		 */
		void operator()() const;
		/*!
		 * \brief run command with specified arguments
		 * \return return code
		 */
		int execute() const;
		// \todo named argument support
		/*!
		 * \brief add positional argument
		 * \return *this
		 */
		executor &add_argument(const std::string &arg);
		/*!
		 * \brief set current work directory for process
		 */
		executor &current_path(const boost::filesystem::path &cwd);
		template <typename ... Args>
		executor &add_argument(const std::string &arg, const Args &...args)
		{
			add_argument(arg);
			return add_argument(args...);
		}
		template <typename ... Args>
		int execute(const Args &...args) const
		{
			executor tmp(*this);
			return tmp.add_argument(args...).execute();
		}
		template <typename ... Args>
		void operator()(const Args &...args) const
		{
			executor tmp(*this);
			tmp.add_argument(args...)();
		}
		template <typename T, typename ... Args>
		static int run_execute_from(const boost::filesystem::path &cwd, const T &command, const Args &...args)
		{
			executor exc(command);
			return exc.current_path(cwd).execute(args...);
		}
		template <typename T, typename ... Args>
		static int run_execute(const T &command, const Args &...args)
		{
			executor exc(command);
			return exc.execute(args...);
		}
		template <typename T, typename ... Args>
		static void exec_from(const boost::filesystem::path &cwd, const T &command, const Args &...args)
		{
			executor exc(command);
			exc.current_path(cwd)(args...);
		}
		template <typename T, typename ... Args>
		static void exec(const T &command, const Args &...args)
		{
			executor exc(command);
			exc(args...);
		}
	public: // TODO should be private
		typedef boost::optional<std::string> string_opt;
		typedef boost::variant<std::string, size_t> substring;
		typedef std::vector<substring> argument;
	private:
		std::vector<argument> arguments;
		std::vector<string_opt> positional;
		boost::optional<boost::filesystem::path> current_path_;
		size_t next_positional;
		void process(argument &arg, const boost::property_tree::ptree::value_type &arg_value);
	};
}

