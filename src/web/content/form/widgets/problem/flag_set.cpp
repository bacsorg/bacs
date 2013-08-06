#include <bacs/archive/web/content/form/widgets/problem/flag_set.hpp>

#include <algorithm>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
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
        valid(std::all_of(set.begin(), set.end(), archive::problem::is_allowed_flag));
        return valid();
    }
}}}}}}}
