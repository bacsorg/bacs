#include "bacs/archive/web/content/form/widgets/problem/optional_id_set.hpp"

#include <algorithm>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    optional_id_set::optional_id_set(const std::string &flag_,
                                     const flag_type_enum flag_type_):
        flag_type(flag_type_)
    {
        std::string ids_id(16, '\0');
        {
            std::mt19937 rng(std::time(nullptr));
            std::uniform_int_distribution<char> rnd('a', 'z');
            std::generate(ids_id.begin(), ids_id.end(), [&rng, &rnd](){return rnd(rng);});
        }
        flag.name("ids_flag");
        flag.message(flag_);
        static const std::string jhead = R"EOF("document.getElementById(')EOF";
        static const std::string jmid = R"EOF(').disabled = )EOF";
        static const std::string jtail = R"EOF(this.checked;")EOF";
        std::string script;
        switch (flag_type)
        {
        case flag_enables:
            script = jhead + ids_id + jmid + "!" + jtail;
            break;
        case flag_disables:
            script = jhead + ids_id + jmid + jtail;
            break;
        }
        flag.attributes_string("onload=" + script + " onchange=" + script);
        ids.id(ids_id);
        ids.name("ids");
        ids.message(cppcms::locale::translate("Problem ids"));
        add(flag);
        add(ids);
    }

    bool optional_id_set::validate()
    {
        bool valid = flag.validate();
        switch (flag_type)
        {
        case flag_enables:
            if (flag.value())
                valid = valid && ids.validate();
            break;
        case flag_disables:
            if (!flag.value())
                valid = valid && ids.validate();
            break;
        }
        return valid;
    }

    boost::optional<archive::problem::id_set> optional_id_set::value()
    {
        switch (flag_type)
        {
        case flag_enables:
            if (flag.value())
                return ids.value();
            break;
        case flag_disables:
            if (!flag.value())
                return ids.value();
            break;
        }
        return boost::optional<archive::problem::id_set>();
    }

    void optional_id_set::value(const boost::optional<archive::problem::id_set> &id_set_)
    {
        ids.disabled(!id_set_);
        if (id_set_)
            ids.value(id_set_.get());
        switch (flag_type)
        {
        case flag_enables:
            flag.value(static_cast<bool>(id_set_));
            break;
        case flag_disables:
            flag.value(!static_cast<bool>(id_set_));
            break;
        }
    }
}}}}}}}
