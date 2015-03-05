#include <bacs/problem/utility.hpp>

#include <boost/optional.hpp>

#include <string>
#include <vector>

namespace bacs{namespace problem{namespace utilities
{
    class single: public utility
    {
    public:
        single(const boost::filesystem::path &location,
               const boost::property_tree::ptree &config);

        bool make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) const override;

    private:
        const boost::filesystem::path m_source;
        const boost::optional<std::string> m_std;
        const std::vector<std::string> m_libs;
    };
}}}
