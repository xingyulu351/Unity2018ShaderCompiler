using System;
using System.IO;
using System.Text.RegularExpressions;
using NiceIO;
using NUnit.Framework;
using NUnit.Framework.Internal;

namespace Bee.TestHelpers
{
    public class TempDirectory : NPath, IDisposable
    {
        /// <summary>
        /// Common base directory for all TempDirectory instances. Directories
        /// are created beneath this, rather than directly in NPath.SystemTemp,
        /// to make manual clean up easier.
        /// </summary>
        public static NPath BeeTestTempPath => NPath.SystemTemp.Combine("BeeTest");

        // By default, the TempDirectory path is length limited on Windows to
        // guarantee users another 160 characters before they hit Windows path
        // length limitations. Some tests need much more than this, though, and
        // they will have to request a higher "path budget" in the constructor.
        // (Windows MAX_PATH: This is why we can't have nice things.)
        public const int DefaultPathBudget = 160;

        static string GetCurrentTestName()
        {
            // (TestContext.CurrentContext?.Test is more straightforward, but may throw if there's no current context.)
            var test = (TestContext.CurrentTestExecutionContext ?? TestExecutionContext.CurrentContext)?.CurrentTest;
            if (test == null) return "unknown";
            if (test.TypeInfo == null) return test.Name;

            // Use unqualified type name (unlike FullName, which uses fully qualified class name).
            var className = test.TypeInfo.Name;
            // Remove the filler word "Tests" to maximize utility under Windows path length limtis.
            if (className.EndsWith("Tests")) className = className.Substring(0, className.Length - 5);
            return $"{className}.{test.Name}";
        }

        static readonly Regex UnsafePathChars = new Regex(@"[^_.,=@a-zA-Z0-9]+", RegexOptions.CultureInvariant);
        static string GetTempPath(string testName, int pathBudget)
        {
            var suffixLength = 10; // '-' + 8 random characters + '/'
            var tempPrefix = BeeTestTempPath.ToString() + '/';
            testName = UnsafePathChars.Replace(testName, "-");

            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                var pathBudgetAvail = 259 - tempPrefix.Length - suffixLength;
                var maxTestNameLength = pathBudgetAvail - pathBudget;
                if (maxTestNameLength < 0)
                    throw new AssertionException($"TempDirectory cannot accomodate path budget of {pathBudget} characters; only {pathBudgetAvail} available");
                if (testName.Length > maxTestNameLength)
                    testName = testName.Substring(0, maxTestNameLength);
            }

            while (true)
            {
                var randomSuffix = '-' + Path.GetRandomFileName().Substring(0, suffixLength - 2);
                var result = tempPrefix + testName + randomSuffix;
                if (!File.Exists(result) && !Directory.Exists(result))
                    return result;
            }
        }

        /// <summary>
        /// Creates a temporary directory for Bee testing purposes. The resulting
        /// directory is created beneath a single common "BeeTest" directory, and
        /// named after the currently executing NUnit test (if any).
        ///
        /// Disposing removes the directory and its contents again; if that's not
        /// desired, simply do not call Dispose. (Finalizer does not dispose.)
        /// </summary>
        public TempDirectory(string testName = null, int pathBudget = DefaultPathBudget)
            : base(GetTempPath(testName ?? GetCurrentTestName(), pathBudget))
        {
            EnsureDirectoryExists();
        }

        public void Dispose()
        {
            DeleteIfExists();
        }
    }
}
