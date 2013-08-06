#include <bacs/archive/web/content/form/widgets/problem/id_set.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <vector>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
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
        return valid();
    }
}}}}}}}
