#include <bunsan/utility/maker.hpp>

#include <boost/io/detail/quoted_manip.hpp>

#include <sstream>

BUNSAN_FACTORY_DEFINE(bunsan::utility::maker)

namespace boost
{
    std::string to_string(const bunsan::utility::maker_error::targets &targets)
    {
        std::ostringstream sout;
        sout << "[" << bunsan::error::info_name(targets) << "] = {";
        for (std::size_t i = 0; i < targets.value().size(); ++i)
        {
            if (i)
                sout << ' ';
            sout << boost::io::quoted(targets.value()[i]);
        }
        sout << "}\n";
        return sout.str();
    }
}
