#include "bacs/problem/statement.hpp"
#include "bacs/problem/error.hpp"

#include <boost/filesystem/path.hpp>

#include <string>

namespace bacs{namespace problem{namespace statement_versions
{
    class pdflatex: public statement::version
    {
    public:
        pdflatex(const boost::filesystem::path &location,
                 const boost::property_tree::ptree &config);

        void make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package,
                          const bunsan::pm::entry &resources_package) const override;

        pb::Statement::Version info() const override;

    private:
        const boost::filesystem::path m_root;
        const boost::filesystem::path m_source;
        const boost::filesystem::path m_target;

    private:
        static const bool factory_reg_hook;
    };
}}}
