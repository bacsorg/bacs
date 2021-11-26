using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Bunsan.PM.Detail
{
	using size_type = UInt32;
	using char_ = System.Byte;
	using cchar_ = System.Byte;
	using bool_ = System.Boolean;
	using int_ = Int32;

	public class RepositoryImplementation
	{
		private const String Library = "bunsan_pm_compatibility";
		private const CallingConvention Convention = CallingConvention.Cdecl;
		private const size_type error_size = 1000;

		[DllImport(Library, CallingConvention=Convention)]
		private extern static int_ bunsan_pm_repository_create (
			cchar_[] config, cchar_[] path, bool_ strip,
			char_[] error_msg, size_type error_size);

		[DllImport(Library, CallingConvention=Convention)]
		private extern static int_ bunsan_pm_repository_clean (
			cchar_[] config, char_[] error_msg, size_type error_size);

		[DllImport(Library, CallingConvention=Convention)]
		private extern static int_ bunsan_pm_repository_extract (
			cchar_[] config, cchar_[] package,
			cchar_[] path, char_[] error_msg, size_type error_size);

		public static void ThrowException (int_ ret, char_[] error_msg)
		{
			if (ret != 0) {
				String msg = GetString (error_msg);
				throw new RepositoryException (ret, msg);
			}
		}

		public static char_[] GetChars (String s)
		{
			char_[] b = Encoding.UTF8.GetBytes (s);
			char_[] r = new char_[b.Length + 1];
			Array.Copy (b, r, b.Length);
			r [b.Length] = 0;
			return r;
		}

		public static String GetString (char_[] b)
		{
			int size = Array.IndexOf (b, (byte)0);
			if (size == -1)
				size = b.Length;
			char_[] r = new char_[size];
			Array.Copy (b, r, size);
			String s = Encoding.UTF8.GetString (r);
			return s;
		}

		public static void Create (String configPath, String path, bool strip=false)
		{
			char_[] error_msg = new char_[error_size];
			int_ ret = bunsan_pm_repository_create (
				GetChars (configPath), GetChars (path), strip, error_msg, error_size);
			ThrowException (ret, error_msg);
		}

		public static void Clean (String configPath)
		{
			char_[] error_msg = new char_[error_size];
			int_ ret = bunsan_pm_repository_clean (
				GetChars (configPath), error_msg, error_size);
			ThrowException (ret, error_msg);
		}

		public static void Extract (String configPath, String package, String path)
		{
			char_[] error_msg = new char_[error_size];
			int_ ret = bunsan_pm_repository_extract (
				GetChars (configPath), GetChars (package),
				GetChars (path), error_msg, error_size);
			ThrowException (ret, error_msg);
		}
	}
}

