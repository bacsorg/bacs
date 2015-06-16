#include <bacs/archive/web/content/form/widgets/problem/id_set.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <vector>
#include <unordered_set>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    archive::problem::IdSet id_set::value()
    {
        archive::problem::IdSet set;
        archive::problem::id value_(text::value());
        boost::algorithm::trim(value_);
        std::unordered_set<std::string> uset;
        if (!value_.empty())
            boost::algorithm::split(uset, value_,
                                    boost::algorithm::is_space(),
                                    boost::algorithm::token_compress_on);
        *set.mutable_id() = {uset.begin(), uset.end()};
        return set;
    }

    void id_set::value(const archive::problem::IdSet &id_set_)
    {
        std::vector<archive::problem::id> ids(id_set_.id().begin(), id_set_.id().end());
        std::sort(ids.begin(), ids.end());
        text::value(boost::algorithm::join(ids, " "));
    }

    bool id_set::validate()
    {
        if (!text::validate())
            return false;
        const archive::problem::IdSet set = value();
        valid(!set.id().empty() && std::all_of(set.id().begin(),
                                               set.id().end(),
                                               archive::problem::is_allowed_id));
        return valid();
    }
}}}}}}}
