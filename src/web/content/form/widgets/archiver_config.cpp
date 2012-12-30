#include "bacs/archive/web/content/form/widgets/archiver_config.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets
{
    archiver_config::archiver_config()
    {
        type.name("archiver_type");
        format.name("archiver_format");
        type.message(cppcms::locale::translate("Archiver type"));
        format.message(cppcms::locale::translate("Archiver format"));
        add(type);
        add(format);
    }

    archiver_options archiver_config::value()
    {
        archiver_options config;
        config.type = type.value();
        config.config.put("format", format.value());
        return config;
    }
}}}}}}
