using System;
using NUnit.Framework;

using Bunsan.PM.Detail;

namespace Bunsan.PM
{
	[TestFixture()]
	public class RepositoryTest
	{
		[Ignore()]
		public void TestCall ()
		{
			Repository repository = new Repository ("pm.rc");
			repository.Clean ();
			repository.Extract ("exp/cpu_runner", "/tmp/test");
		}

		[Test()]
		public void TestConvert ()
		{
			String test = "something short";
			byte[] b = RepositoryImplementation.GetChars (test);
			Assert.Greater (b.Length, 0);
			Assert.AreEqual (0, b [b.Length - 1]);
			String s1 = RepositoryImplementation.GetString (b);
			Assert.AreEqual (test, s1);
			byte[] r = new byte[b.Length - 1];
			Array.Copy (b, r, r.Length);
			String s2 = RepositoryImplementation.GetString (b);
			Assert.AreEqual (test, s2);
		}
	}
}
