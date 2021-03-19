using System;
using System.IO;
using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class EnumTests : ClassBasedTests
    {
        [SetUp]
        public void SetUp()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
        }

        [TestCase("EmptyEnum",
@"enum EmptyEnum {
}",
@"enum EmptyEnum {
}")]
        [TestCase("SimpleEnum",
@"enum SimpleEnum {
    First,
    Second,
}",
@"enum SimpleEnum {
    First,
    Second,
}")]
        [TestCase("EnumWithInitializer",
@"enum EnumWithInitializer {
    First,
    Second = 2,
    Last
}",
@"enum EnumWithInitializer {
    First,
    Second = 2,
    Last,
}")]

        [TestCase("Flags",
@"[Flags]
enum FlagsEnum {
    First,
    Last,
}",
@"@Flags
enum FlagsEnum {
    First,
    Last,
}")]
        public void Tests(string testName, string input, string expected)
        {
            AssertConversion(input, expected, true);
        }

        [Test]
        public void EnumMemberReferencingSiblingMembersIsInvalid()
        {
            const string input = @"enum InvalidEnumMemberReference
{
    First,
    Second = First + 42,
}";
            const string expected = @"enum InvalidEnumMemberReference {
    First,
    Second = First + 42,
}";

            var e = Assert.Throws(Is.InstanceOf<Exception>(), () => AssertConversion(input, expected, true, false, compileOutput: true));
            StringAssert.Contains("BCE0043: Unexpected token: First.", e.Message);
        }
    }
}
