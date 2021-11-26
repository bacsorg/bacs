using System;

using Bunsan.PM.Detail;

namespace Bunsan.PM
{
	public class Repository
	{
		private String ConfigPath;

		public Repository (String configPath)
		{
			ConfigPath = configPath;
		}

		public void Create (String path, bool strip)
		{
			RepositoryImplementation.Create (ConfigPath, path, strip);
		}

		public void Create (String path)
		{
			RepositoryImplementation.Create (ConfigPath, path);
		}

		public void Clean ()
		{
			RepositoryImplementation.Clean (ConfigPath);
		}

		public void Extract (String package, String path)
		{
			RepositoryImplementation.Extract (ConfigPath, package, path);
		}
	}
}
