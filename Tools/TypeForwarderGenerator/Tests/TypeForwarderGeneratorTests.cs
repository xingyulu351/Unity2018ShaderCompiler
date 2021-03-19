using System.Text;
using Mono.Cecil;
using NUnit.Framework;

[TestFixture]
public class Tests
{
    AssemblyDefinition TestAssemblyDefinition = AssemblyDefinition.ReadAssembly(typeof(TestDLL.TestType).Assembly.Location);

    public void TestForwarderCodeContains(string key, bool shouldContain = true)
    {
        StringBuilder sb = new StringBuilder();
        Program.GenerateForwarderCode(new[] { TestAssemblyDefinition }, sb);
        var forwarderCode = sb.ToString();
        if (shouldContain)
            Assert.IsTrue(forwarderCode.Contains(key), "Expected to find " + key + ", but got " + forwarderCode);
        else
            Assert.IsFalse(forwarderCode.Contains(key), "Did not expect to find " + key + ", but got " + forwarderCode);
    }

    public void TestObsoleteCodeContains(string key, bool shouldContain = true)
    {
        StringBuilder sb = new StringBuilder();
        Program.GenerateObsoleteCode(new[] { TestAssemblyDefinition }, sb);
        var forwarderCode = sb.ToString();
        if (shouldContain)
            Assert.IsTrue(forwarderCode.Contains(key), "Expected to find " + key + ", but got " + forwarderCode);
        else
            Assert.IsFalse(forwarderCode.Contains(key), "Did not expect to find " + key + ", but got " + forwarderCode);
    }

    [Test]
    public void TestAssemblyComment()
    {
        TestForwarderCodeContains("// TestDLL\n");
    }

    [Test]
    public void TestSimpleTypeForwarder()
    {
        TestForwarderCodeContains("[assembly:TypeForwardedToAttribute(typeof(TestDLL.TestType))]\n");
    }

    [Test]
    public void TestGenericTypeForwarder()
    {
        TestForwarderCodeContains("[assembly:TypeForwardedToAttribute(typeof(TestDLL.GenericType1<>))]\n");
    }

    [Test]
    public void TestGenericTypeForwarderWithMultipleArguments()
    {
        TestForwarderCodeContains("[assembly:TypeForwardedToAttribute(typeof(TestDLL.GenericType3<,,>))]\n");
    }

    [Test]
    public void TestPrivateType()
    {
        TestForwarderCodeContains("[assembly:TypeForwardedToAttribute(typeof(TestDLL.PrivateType))]\n", true);
    }

    [Test]
    public void WillMakeAStubInsteadOfAForwarderForObsoleteErrorTypes()
    {
        TestForwarderCodeContains("[assembly:TypeForwardedToAttribute(typeof(TestDLL.ObsoleteErrorType))]\n", false);
        TestObsoleteCodeContains("namespace TestDLL {\n");
        TestObsoleteCodeContains("internal class ObsoleteErrorType {}\n");
    }

    [Test]
    public void WillNotIgnoreObsoleteWarningTypes()
    {
        TestForwarderCodeContains("[assembly:TypeForwardedToAttribute(typeof(TestDLL.ObsoleteWarningType))]\n");
    }
}
