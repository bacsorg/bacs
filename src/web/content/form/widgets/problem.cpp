#include "bacs/archive/web/content/form/widgets/problem.hpp"

#include <algorithm>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    archive::problem::id id::value()
    {
        return text::value();
    }

    void id::value(const archive::problem::id &id_)
    {
        text::value(id_);
    }

    bool id::validate()
    {
        if (!text::validate())
            return false;
        valid(archive::problem::is_allowed_id(value()));
        return text::validate();
    }

    archive::problem::id_set id_set::value()
    {
        archive::problem::id_set set;
        archive::problem::id value_(text::value());
        boost::algorithm::trim(value_);
        if (!value_.empty())
            boost::algorithm::split(set, value_,
                                    boost::algorithm::is_space(),
                                    boost::algorithm::token_compress_on);
        return set;
    }

    void id_set::value(const archive::problem::id_set &id_set_)
    {
        std::vector<archive::problem::id> ids(id_set_.begin(), id_set_.end());
        std::sort(ids.begin(), ids.end());
        text::value(boost::algorithm::join(ids, " "));
    }

    bool id_set::validate()
    {
        if (!text::validate())
            return false;
        const archive::problem::id_set set = value();
        valid(!set.empty() && std::all_of(set.begin(), set.end(), archive::problem::is_allowed_id));
        return text::validate();
    }

    archive::problem::flag_set flag_set::value()
    {
        archive::problem::flag_set set;
        archive::problem::flag value_(text::value());
        boost::algorithm::trim(value_);
        if (!value_.empty())
            boost::algorithm::split(set, value_,
                                    boost::algorithm::is_space(),
                                    boost::algorithm::token_compress_on);
        return set;
    }

    void flag_set::value(const archive::problem::flag_set &flag_set_)
    {
        std::vector<archive::problem::flag> flags(flag_set_.begin(), flag_set_.end());
        std::sort(flags.begin(), flags.end());
        text::value(boost::algorithm::join(flags, " "));
    }

    bool flag_set::validate()
    {
        if (!text::validate())
            return false;
        const archive::problem::flag_set set = value();
        valid(std::all_of(set.begin(), set.end(), archive::problem::is_allowed_id));
        return text::validate();
    }
}}}}}}}
