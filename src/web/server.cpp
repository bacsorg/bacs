#include <bacs/statement_provider/web/server.hpp>

#include <bacs/statement_provider/request.pb.h>

#include <bacs/problem/statement.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>

#include <cppcms/base64.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_response.h>
#include <cppcms/service.h>

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
        boost::filesystem::path data_root;
        if (get_(referrer, request, &cache_entry, nullptr, &data_root))
        {
            const boost::filesystem::path filepath = bunsan::filesystem::keep_in_root(path, data_root);
            const std::string filename = filepath.filename().string();
            if (boost::filesystem::is_regular_file(filepath))
            {
                response().content_type(m_mime_file.mime_type_by_name(filepath));
                BOOST_ASSERT(filename.find('"') == std::string::npos);
                response().set_header("Content-Disposition", "filename=\"" + filename + "\"");
                response().content_length(boost::filesystem::file_size(filepath));
                bunsan::filesystem::ifstream fin(filepath);
                BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
                {
                    response().out() << fin.rdbuf();
                }
                BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
                fin.close();
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
                      boost::filesystem::path *const index,
                      boost::filesystem::path *const data_root)
    {
        std::string cipherdata;
        if (!cppcms::b64url::decode(request, cipherdata))
        {
            response().status(cppcms::http::response::bad_request, "Invalid base64url encoded message");
            return false;
        }
        std::string data = cipherdata; // TODO decrypt, this step is skipped for now
        bacs::statement_provider::Request statement_request;
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
        const problem::statement::version::built statement_version(package_dir);
        if (false) // TODO check hash
        {
            response().status(cppcms::http::response::gone, "Problem's hash is not equal to statement version's hash");
            return false;
        }
        if (index)
            *index = statement_version.manifest().data.index;
        if (data_root)
            *data_root = statement_version.data_root();
        return true;
    }
}}}
