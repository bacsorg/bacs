#include "conf_make_install.hpp"

void bunsan::utility::builders::conf_make_install::install(
    const boost::filesystem::path &src,
    const boost::filesystem::path &bin,
    const boost::filesystem::path &root)
{
    configure_(src, bin);
    make_(src, bin);
    install_(src, bin, root);
}
