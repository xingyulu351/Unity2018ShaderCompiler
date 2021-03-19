using System.IO;
using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class AttributeTests : ClassBasedTests
    {
        private string m_OldCurDir;

        [OneTimeSetUp]
        public void Init()
        {
            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory("../..");
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            //restore cur dir
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        [TestCase("AttributeWithValues",
@"[Obsolete(""Why are you using this?"")]
class AttributeWithValues
{
    [Obsolete(""Use something else"")]
    void Method()
    {
    }
}",
@"@Obsolete(""Why are you using this?"")
class AttributeWithValues {
    @Obsolete(""Use something else"")
    function Method() {
    }
}")]

        [TestCase("SimpleEveryWhere",
@"[Obsolete]
class SimpleEveryWhere
{
    [Obsolete]
    void Method([ParamArray] int p)
    {
    }
}",
@"@Obsolete
class SimpleEveryWhere {
    @Obsolete
    function Method(@ParamArray p: int) {
    }
}")]

        [TestCase("PropertyWithAttribute",
@"public class Ship
{
    [SyncVar]
    public int health = 100;

    [SyncVar]
    public float energy = 100;
};",
@"public class Ship {
    @SyncVar
    public var health: int = 100;
    @SyncVar
    public var energy: float = 100;
}")]
        public void Tests(string testName, string input, string expected)
        {
            AssertConversion(input, expected, true);
        }
    }
}
