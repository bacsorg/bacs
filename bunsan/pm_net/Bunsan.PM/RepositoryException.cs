using System;

namespace Bunsan.PM
{
	public class RepositoryException : System.Exception
	{
		public RepositoryException (int returnCode, String message):
			base(String.Format(
				"Function has not completed succesfully: "+
				"return code is {0} and error message is \"{1}\"",
				returnCode, message))
		{
		}
	}
}

