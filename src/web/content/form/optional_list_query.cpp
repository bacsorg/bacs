#include "bacs/archive/web/content/form/optional_list_query.hpp"

#include <random>
#include <ctime>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    optional_list_query::optional_list_query(const std::string &flag_,
                                             const std::string &query,
                                             const flag_type_enum flag_type_):
        flag_type(flag_type_)
    {
        std::string ids_id(16, '\0');
        {
            std::mt19937 rng(std::time(nullptr));
            std::uniform_int_distribution<char> rnd('a', 'z');
            std::generate(ids_id.begin(), ids_id.end(), [&rng, &rnd](){return rnd(rng);});
        }
        flag.name("flag");
        flag.message(flag_);
        static const std::string jhead = R"EOF(onchange="document.getElementById(')EOF";
        static const std::string jmid = R"EOF(').disabled = )EOF";
        static const std::string jtail = R"EOF(this.checked;")EOF";
        switch (flag_type)
        {
        case flag_enables:
            flag.attributes_string(jhead + ids_id + jmid + "!" + jtail);
            break;
        case flag_disables:
            flag.attributes_string(jhead + ids_id + jmid + jtail);
            break;
        }
        ids.id(ids_id);
        ids.name("ids");
        ids.message(cppcms::locale::translate("Problem ids"));
        submit.value(query);
        add(flag);
        add(ids);
        add(submit);
    }

    bool optional_list_query::validate()
    {
        bool valid = flag.validate() && submit.validate();
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

    boost::optional<problem::id_set> optional_list_query::value()
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
        return boost::optional<problem::id_set>();
    }
}}}}}
