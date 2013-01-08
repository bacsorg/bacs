#pragma once

#include "cwd_split.hpp"

#include "bunsan/stream_enum.hpp"

#include <set>

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace utility{namespace archivers
{
    class tar: public cwd_split
    {
    public:
        struct config
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(format);
                ar & BOOST_SERIALIZATION_NVP(flags);
            }

            BUNSAN_INCLASS_STREAM_ENUM_CLASS(flag,
            (
                exclude_vcs
            ))

            boost::optional<std::string> format;
            std::set<flag> flags; ///< \note: std::hash is not specialized for this type
        };

    public:
        explicit tar(const boost::filesystem::path &exe);

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
        std::vector<std::string> flag_arguments() const;

    private:
        const boost::filesystem::path m_exe;
        config m_config;

    private:
        static const bool factory_reg_hook;
    };
}}}
