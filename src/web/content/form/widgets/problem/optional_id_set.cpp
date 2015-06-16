#include <bacs/archive/web/content/form/widgets/problem/optional_id_set.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <vector>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    optional_id_set::optional_id_set(const std::string &flag_,
                                     const flag_type_enum flag_type_):
        flag_type(flag_type_)
    {
        std::string ids_id(16, '\0');
        //std::string flag_id(16, '\0');
        {
            std::mt19937 rng(std::time(nullptr));
            std::uniform_int_distribution<char> rnd('a', 'z');
            std::generate(ids_id.begin(), ids_id.end(), [&rng, &rnd](){return rnd(rng);});
            //std::generate(flag_id.begin(), flag_id.end(), [&rng, &rnd](){return rnd(rng);});
        }
        flag.name("ids_flag");
        //flag.id(flag_id);
        flag.message(flag_);
        const std::string flag_head = " onclick=\"document.getElementById('" + ids_id + "').disabled = ";
        const std::string flag_tail = "this.checked;\"";
        std::string flag_attr;
        std::string ids_attr;
        switch (flag_type)
        {
        case flag_enables:
            flag_attr = flag_head + "!" + flag_tail;
            break;
        case flag_disables:
            flag_attr = flag_head + flag_tail;
            break;
        }
        flag.attributes_string(flag_attr);
        ids.attributes_string(ids_attr);
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

    boost::optional<archive::problem::IdSet> optional_id_set::value()
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
        return boost::optional<archive::problem::IdSet>();
    }

    void optional_id_set::value(const boost::optional<archive::problem::IdSet> &id_set_)
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

    void optional_id_set::render(cppcms::form_context &context)
    {
        switch (flag_type)
        {
        case flag_enables:
            ids.disabled(!flag.value());
            break;
        case flag_disables:
            ids.disabled(flag.value());
            break;
        }
        form::render(context);
    }
}}}}}}}
