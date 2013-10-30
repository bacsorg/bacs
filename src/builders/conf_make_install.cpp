#include "conf_make_install.hpp"

void bunsan::utility::builders::conf_make_install::install(
    const boost::filesystem::path &src,
    const boost::filesystem::path &bin,
    const boost::filesystem::path &root)
{
    try
    {
        configure_(src, bin);
        make_(src, bin);
        install_(src, bin, root);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(builder_install_error() <<
                              builder_install_error::src(src) <<
                              builder_install_error::bin(bin) <<
                              builder_install_error::root(root) <<
                              enable_nested_current());
    }
}
