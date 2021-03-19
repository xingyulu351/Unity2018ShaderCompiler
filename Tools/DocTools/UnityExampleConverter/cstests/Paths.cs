using System;
using System.IO;
using NUnit.Framework;

namespace UnityExampleConverter.Tests
{
    public static class Paths
    {
        public static string TestCases
        {
            get { return FindTestsPath(); }
        }

        public static string Lib
        {
            get { return Path.Combine(TestCases, "../Lib"); }
        }

        private static string FindTestsPath()
        {
            string path = Path.GetFullPath(TestContext.CurrentContext.TestDirectory);
            while (!Directory.Exists(Path.Combine(path, "cstests")))
            {
                string oldPath = path;
                path = Path.GetDirectoryName(path);
                if (oldPath == path)
                {
                    throw new Exception("'tests' directory not found");
                }
            }
            return path;
        }
    }
}
