#pragma once

#include "cwd_split.hpp"

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace utility{namespace archivers
{
    class _7z: public cwd_split
    {
    public:
        struct config
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(format);
            }

            boost::optional<std::string> format;
        };

    public:
        explicit _7z(const boost::filesystem::path &exe);

        void unpack(
            const boost::filesystem::path &archive,
            const boost::filesystem::path &dir) override;

        void setup(const boost::property_tree::ptree &ptree) override;

    protected:
        void pack_from(
            const boost::filesystem::path &cwd,
            const boost::filesystem::path &archive,
            const boost::filesystem::path &file) override;

    private:
        boost::optional<std::string> format_argument() const;

    private:
        static const bool factory_reg_hook_7z;
        static const bool factory_reg_hook_7za;
        static const bool factory_reg_hook_7zr;

    private:
        const boost::filesystem::path m_exe;
        config m_config;
    };
}}}
