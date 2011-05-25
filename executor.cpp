#include "executor.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "util.hpp"

its::executor::executor(const std::string &command): positional(0), next_positional(0)
{
	SLOG("creating executor object from \""<<command<<"\"");
	substring str = command;
	argument arg;
	arg.push_back(str);
	arguments.push_back(arg);
}

void its::executor::process(argument &arg, const boost::property_tree::ptree::value_type &arg_value)
{
	//DLOG();
	if (arg_value.first=="t")
	{
		substring str = arg_value.second.get_value<std::string>();
		SLOG("text type \""<<str<<"\"");
		arg.push_back(str);
	}
	else if (arg_value.first=="p")
	{
		size_t ph = arg_value.second.get_value<size_t>();
		SLOG("positional placeholder \""<<ph<<"\"");
		substring str = ph;
		if (positional.size()<=ph)
			positional.resize(ph+1);
		arg.push_back(str);
	}
	else
	{
		SLOG("unknown type \""<<arg_value.first<<"\"");
		throw std::runtime_error("unknown ["+arg_value.first+"]");
	}
}

its::executor::executor(const boost::property_tree::ptree &command): next_positional(0)
{
	DLOG(creating executor object from boost::property_tree::ptree);
	BOOST_FOREACH(const boost::property_tree::ptree::value_type &arg_value, command)
	{
		argument arg;
		if (arg_value.first=="c")
		{
			BOOST_FOREACH(const boost::property_tree::ptree::value_type &arg_subvalue, arg_value.second)
			{
				process(arg, arg_subvalue);
			}
		}
		else
		{
			process(arg, arg_value);
		}
		arguments.push_back(arg);
	}
	DLOG(created);
}

void its::executor::operator()() const
{
	DLOG(trying to execute);
	int code = execute();
	if (code)
		throw its::return_code(code);
}

its::executor &its::executor::add_argument(const std::string &arg)
{
	SLOG("adding argument \""<<arg<<"\"");
	if (next_positional<positional.size())
	{
		positional[next_positional] = arg;
		++next_positional;
	}
	else
	{
		substring str = arg;
		argument arg;
		arg.push_back(str);
		arguments.push_back(arg);
	}
	return *this;
}

its::executor &its::executor::current_path(const boost::filesystem::path &cwd)
{
	SLOG("setting cwd to "<<cwd);
	current_path_ = cwd;
	return *this;
}

class args_visitor: public boost::static_visitor<std::string>
{
public:
	explicit args_visitor(const std::vector<boost::optional<std::string>> *positional_): positional(positional_){}
	std::string operator()(const std::string &str)
	{
		SLOG("returning std::string from std::string=\""<<str<<"\"");
		return str;
	}
	std::string operator()(size_t n)
	{
		SLOG("returning std::string=\""<<positional->at(n).get()<<"\" from size_t=\""<<n<<"\"");
		return positional->at(n).get();
	}
private:
	const std::vector<boost::optional<std::string>> *positional;
};

#include "impl/execute.hpp"

int its::executor::execute() const
{
	DLOG(trying to execute);
	std::vector<std::string> args(arguments.size());
	args_visitor visitor(&positional);
	for (size_t i = 0; i<arguments.size(); ++i)
	{
		for (size_t j = 0; j<arguments[i].size(); ++j)
		{
			args[i] = (boost::apply_visitor(visitor, arguments[i][j]));
		}
	}
	return its::impl::execute(current_path_.get_value_or(boost::filesystem::current_path()), args);
}

its::return_code::return_code(int code_): code(code_), std::runtime_error("command was finished with \""+boost::lexical_cast<std::string>(code_)+"\" return code"){}

its::return_code::operator int() const throw()
{
	return code;
}

