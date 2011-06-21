#include "bunsan/pm/bunsan_pm_c.hpp"

#include <cstring>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "bunsan/util.hpp"

#include "bunsan/pm/repository.hpp"

void *bunsan_pm_create(char *error, size_t ebufsize, const char *config)
{
	try
	{
		boost::property_tree::ptree cfg;
		bunsan::read_info(config, cfg);
		bunsan::pm::repository *repo = new bunsan::pm::repository(cfg);
		return repo;
	}
	catch (std::exception &e)
	{
		strncpy(error, e.what(), ebufsize);
		return 0;
	}
	catch (...)
	{
		strncpy(error, "Unknown error", ebufsize);
		return 0;
	}
}

int bunsan_pm_clean(char *error, size_t ebufsize, void *repository)
{
	try
	{
		static_cast<bunsan::pm::repository *>(repository)->clean();
		return 0;
	}
	catch (std::exception &e)
	{
		strncpy(error, e.what(), ebufsize);
		return -2;
	}
	catch (...)
	{
		strncpy(error, "Unknown error", ebufsize);
		return -1;
	}
}

int bunsan_pm_extract(char *error, size_t ebufsize, void *repository, const char *package, const char *dir)
{
	try
	{
		static_cast<bunsan::pm::repository *>(repository)->extract(package, dir);
		return 0;
	}
	catch (std::exception &e)
	{
		strncpy(error, e.what(), ebufsize);
		return -2;
	}
	catch (...)
	{
		strncpy(error, "Unknown error", ebufsize);
		return -1;
	}
}

int bunsan_pm_remove(char *error, size_t ebufsize, void *repository)
{
	try
	{
		delete static_cast<bunsan::pm::repository *>(repository);
		return 0;
	}
	catch (std::exception &e)
	{
		strncpy(error, e.what(), ebufsize);
		return -2;
	}
	catch (...)
	{
		strncpy(error, "Unknown error", ebufsize);
		return -1;
	}
}

