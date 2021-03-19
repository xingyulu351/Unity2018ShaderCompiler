using NUnit.Framework;

namespace Cs2us.Tests
{
    public class BaseClassConversionTest
    {
        protected static void AssertClassConversion(string input, string expected, bool usePragmaStrict = true)
        {
            var actualOutput = Cs2UsUtils.ConvertClass(input, false, usePragmaStrict);
            if (usePragmaStrict)
                expected = "#pragma strict\n" + expected;
            Assert.AreEqual(expected.CollapseWhitespace(), actualOutput.CollapseWhitespace());
        }
    }
}
