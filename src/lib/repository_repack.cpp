#include <bacs/archive/repository.hpp>
#include <bacs/archive/error.hpp>
#include <bacs/archive/problem/flags.hpp>

namespace bacs{namespace archive
{
    problem::import_info repository::repack_(const problem::id &id,
                                             const problem::hash &hash,
                                             const boost::filesystem::path &problem_location)
    {
        problem::import_info import_info;
        try
        {
            bacs::problem::importer::options options;
            options.problem_dir = problem_location;
            options.destination = m_location.pm_repository_root / m_problem.root_package.location() / id;
            options.root_package = m_problem.root_package / id;
            options.id = id;
            options.hash = hash;
            write_info_(id, m_importer.convert(options));
            m_repository.create_recursively(options.destination, m_problem.strip);
            unset_flag_(id, problem::flags::ignore);
            problem::status status;
            status.hash = hash;
            status.flags = flags_(id);
            import_info.status = status;
        }
        catch (std::exception &e)
        {
            set_flag_(id, problem::flags::ignore);
            import_info.error = e.what();
        }
        return import_info;
    }
}}
