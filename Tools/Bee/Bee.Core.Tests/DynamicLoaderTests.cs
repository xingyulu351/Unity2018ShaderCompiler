using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    [TestFixture]
    public class DynamicLoaderTests
    {
        abstract class TestBase
        {
        }

        class TestClass : TestBase
        {
            public TestClass()
            {
            }
        }

        [Test]
        public void FindsAndInstantiatesClassWithDefaultConstructor()
        {
            var items = DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<TestBase>("*.dll").ToArray();
            Assert.IsInstanceOf<TestClass>(items.Single());
        }

        abstract class TestBase2
        {
        }

        class TestClass2 : TestBase2
        {
            public int Argument { get; }

            public TestClass2(int argument)
            {
                Argument = argument;
            }
        }

        class TestClass2DynamicLoader : DynamicLoaderFor<TestBase2>
        {
            public override IEnumerable<TestBase2> Provide()
            {
                yield return new TestClass2(3);
                yield return new TestClass2(4);
            }
        }

        [Test]
        public void FindsCustomDynamicLoader()
        {
            var items = DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<TestBase2>("*.dll").ToArray();
            Assert.AreEqual(2, items.Length);

            Assert.AreEqual(3, ((TestClass2)items[0]).Argument);
            Assert.AreEqual(4, ((TestClass2)items[1]).Argument);
        }
    }
}
