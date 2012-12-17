#include "bunsan/utility/executor.hpp"

#include "bunsan/logging/legacy.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

bunsan::utility::executor::executor(const std::string &command): positional(0), next_positional(0)
{
    SLOG("creating executor object from \"" << command << "\"");
    arguments.push_back(string(1, token(command)));
}

// TODO delegating constructor

bunsan::utility::executor::executor(const boost::filesystem::path &command): positional(0), next_positional(0)
{
    SLOG("creating executor object from " << command);
    arguments.push_back(string(1, token(command.string())));
}

bunsan::utility::executor::executor(const char *command): positional(0), next_positional(0)
{
    SLOG("creating executor object from " << command);
    arguments.push_back(string(1, token(std::string(command))));
}

void bunsan::utility::executor::process(string &arg, const boost::property_tree::ptree::value_type &arg_value)
{
    token tk;
    if (arg_value.first == "t" || arg_value.first == "text")
    {
        std::string txt = arg_value.second.get_value<std::string>();
        tk = txt;
        SLOG("text type \"" << txt << "\"");
    }
    else if (arg_value.first == "p" || arg_value.first == "positional")
    {
        size_t ph = arg_value.second.get_value<size_t>();
        SLOG("positional placeholder \"" << ph << "\"");
        reference pos = ph;
        if (positional.size() <= ph)
            positional.resize(ph + 1);
        tk = pos;
    }
    else if (arg_value.first == "n" || arg_value.first == "named")
    {
        std::string ph = arg_value.second.get_value<std::string>();
        SLOG("named placeholder \"" << ph << "\"");
        reference nmd = ph;
        tk = nmd;
    }
    else
    {
        SLOG("unknown type \"" << arg_value.first << "\"");
        BOOST_THROW_EXCEPTION(executor_unknown_placeholder_type_error() <<
                              executor_unknown_placeholder_type_error::type(arg_value.first));
    }
    arg.push_back(tk);
}

bunsan::utility::executor::executor(const boost::property_tree::ptree &command): next_positional(0)
{
    DLOG(creating executor object from boost::property_tree::ptree);
    for (const auto &arg_value: command)
    {
        string arg;
        if (arg_value.first == "c" || arg_value.first == "complex")
        {
            DLOG(complex type);
            for (const auto &arg_subvalue: arg_value.second)
            {
                process(arg, arg_subvalue);
            }
            DLOG(end of complex type);
        }
        else if (arg_value.first == "d" || arg_value.first == "definition")
        {
            for (const auto &arg_subvalue: arg_value.second)
            {
                named[arg_subvalue.first] = arg_subvalue.second.get_value<std::string>();
            }
            continue;
        }
        else
        {
            process(arg, arg_value);
        }
        arguments.push_back(arg);
    }
    DLOG(created);
}

void bunsan::utility::executor::operator()() const
{
    DLOG(trying to execute);
    bunsan::process::check_sync_execute(context());
}

bunsan::utility::executor &bunsan::utility::executor::add_argument(const std::string &arg)
{
    SLOG("adding argument \"" << arg << "\"");
    if (next_positional < positional.size())
        positional[next_positional++] = arg;
    else
        arguments.push_back(string(1, token(arg)));
    return *this;
}

bunsan::utility::executor &bunsan::utility::executor::set_argument(const std::string &key, const std::string &arg)
{
    SLOG("setting named argument \"" << key << "\" to \"" << arg << "\"");
    named[key] = arg;
    return *this;
}

bunsan::utility::executor &bunsan::utility::executor::current_path(const boost::filesystem::path &cwd)
{
    SLOG("setting cwd to " << cwd);
    named[current_path_key] = cwd.string();
    return *this;
}

bunsan::utility::executor &bunsan::utility::executor::executable(const boost::filesystem::path &exec_)
{
    SLOG("setting executable to " << exec_);
    named[executable_key] = exec_.string();
    return *this;
}

class bunsan::utility::executor::ref_visitor: public boost::static_visitor<std::string>
{
public:
    ref_visitor(const std::vector<string_opt> *positional_, const dict *named_): positional(positional_), named(named_){}
    std::string operator()(const std::string &key)
    {
        SLOG("named: returning std::string=\"" << named->at(key) << "\" from std::string=\"" << key << "\"");
        return named->at(key);
    }
    std::string operator()(size_t index)
    {
        SLOG("positional: returning std::string=\"" << positional->at(index).get() << "\" from size_t=\"" << index << "\"");
        return positional->at(index).get();
    }
private:
    const std::vector<string_opt> *positional;
    const dict *named;
};

class bunsan::utility::executor::token_visitor: public boost::static_visitor<std::string>
{
public:
    token_visitor(const std::vector<string_opt> *positional_, const dict *named_): positional(positional_), named(named_), rvisitor(positional_, named_){}
    std::string operator()(const std::string &str)
    {
        SLOG("text: returning std::string from std::string=\"" << str << "\"");
        return str;
    }
    std::string operator()(const reference &ref)
    {
        return boost::apply_visitor(rvisitor, ref);
    }
private:
    const std::vector<string_opt> *positional;
    const dict *named;
    ref_visitor rvisitor;
};

void bunsan::utility::executor::prepare(std::vector<std::string> &args, token_visitor &visitor) const
{
    for (size_t i = 0; i < arguments.size(); ++i)
    {
        args[i].clear();
        for (const auto &j: arguments[i])
        {
            args[i] += boost::apply_visitor(visitor, j);
        }
    }
}

bunsan::process::context bunsan::utility::executor::context() const
{
    DLOG(trying to create context);
    std::vector<std::string> args(arguments.size());
    token_visitor visitor(&positional, &named);
    prepare(args, visitor);
    bunsan::process::context ctx;
    ctx.argv(args);
    auto iter = named.find(executable_key);
    if (iter != named.end())
        ctx.executable(iter->second);
    iter = named.find(current_path_key);
    if (iter != named.end())
        ctx.current_path(iter->second);
    iter = named.find(use_path_key);
    if (iter != named.end())
        ctx.use_path(iter->second == "true" || iter->second == "yes" || iter->second == "1"); // TODO bad code
    return ctx;
}

int bunsan::utility::executor::sync() const
{
    DLOG(trying to execute);
    return bunsan::process::sync_execute(context());
}
