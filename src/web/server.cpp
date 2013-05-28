#include "bacs/statement_provider/web/server.hpp"
#include "bacs/statement_provider/pb/request.pb.h"

#include "bacs/single/problem/statement.hpp"

#include "bunsan/config/cast.hpp"
#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"
#include "bunsan/filesystem/operations.hpp"

#include <cppcms/base64.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_response.h>
#include <cppcms/service.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace statement_provider{namespace web
{
    server::server(cppcms::service &srv,
                   const std::shared_ptr<bunsan::pm::cache> &cache):
        cppcms::application(srv),
        m_cache(cache),
        m_mime_file(bunsan::web::load_mime_file(srv.settings().get<std::string>("statement_provider.mime_types")))
    {
        dispatcher().assign("/([^/]+)/get/([^/]+)", &server::get_index, this, 1, 2);
        mapper().assign("get_index", "/{1}/get/{2}");

        dispatcher().assign("/([^/]+)/get/([^/]+)/(.*)", &server::get, this, 1, 2, 3);
        mapper().assign("get", "/{1}/get/{2}/{3}");

        mapper().root("/statement");
    }

    void server::get_index(std::string referrer, std::string request)
    {
        boost::filesystem::path index;
        if (get_(referrer, request, nullptr, &index))
        {
            response().status(cppcms::http::response::temporary_redirect);
            response().set_redirect_header(url("get", referrer, request, index.generic_string()));
        }
    }

    void server::get(std::string referrer, std::string request, std::string path)
    {
        bunsan::pm::cache::entry cache_entry;
        if (get_(referrer, request, &cache_entry))
        {
            const boost::filesystem::path filepath = cache_entry.root() / "data" / path; //FIXME "data"
            const std::string filename = filepath.filename().string();
            if (boost::filesystem::is_regular_file(filepath))
            {
                response().content_type(m_mime_file.mime_type_by_name(filepath));
                BOOST_ASSERT(filename.find('"') == std::string::npos);
                response().set_header("Content-Disposition", "filename=\"" + filename + "\"");
                response().content_length(boost::filesystem::file_size(filepath));
                BUNSAN_EXCEPTIONS_WRAP_BEGIN()
                {
                    bunsan::filesystem::ifstream fin(filepath);
                    response().out() << fin.rdbuf();
                    fin.close();
                }
                BUNSAN_EXCEPTIONS_WRAP_END()
            }
            else
            {
                response().status(cppcms::http::response::not_found);
            }
        }
    }

    bool server::get_(const std::string &referrer,
                      const std::string &request,
                      bunsan::pm::cache::entry *cache_entry,
                      boost::filesystem::path *const index)
    {
        std::string cipherdata;
        if (!cppcms::b64url::decode(request, cipherdata))
        {
            response().status(cppcms::http::response::bad_request, "Invalid base64url encoded message");
            return false;
        }
        std::string data = cipherdata; // TODO decrypt, this step is skipped for now
        bacs::statement_provider::pb::Request statement_request;
        if (!statement_request.ParseFromString(data))
        {
            response().status(cppcms::http::response::bad_request, "Invalid protobuf message");
            return false;
        }
        bunsan::pm::entry package;
        try
        {
            package = statement_request.package();
        }
        catch (bunsan::pm::invalid_entry_name_error &)
        {
            response().status(cppcms::http::response::bad_request, "Invalid package name");
            return false;
        }
        bunsan::pm::cache::entry entry;
        if (!cache_entry)
            cache_entry = &entry;
        *cache_entry = m_cache->get(package);
        const boost::filesystem::path package_dir = cache_entry->root();
        // FIXME refactoring is needed
        const boost::filesystem::path manifest_file = package_dir / "manifest.ini";
        const boost::filesystem::path data_dir = package_dir / "data";
        if (false) // TODO check hash
        {
            response().status(cppcms::http::response::gone, "Problem's hash is not equal to statement version's hash");
            return false;
        }
        boost::property_tree::ptree manifest_ptree;
        boost::property_tree::read_ini(manifest_file.string(), manifest_ptree);
        single::problem::statement::version::manifest manifest =
            bunsan::config::load<single::problem::statement::version::manifest>(manifest_ptree);
        if (index)
            *index = manifest.data.index;
        return true;
    }
}}}
