using System;
using NUnit.Framework;

namespace Cs2us.Tests
{
    class ForStatementTests : ClassBasedTests
    {
        [TestCase("simpleFor",
@"class C {
    void Foo() {
        for (;;)
        {
        }
    }
}",
@"class C {
    function Foo() {
        for (; ; ) {
        }
    }
}")]
        [TestCase("forWithSingleInitializer",
@"class C {
    void Foo() {
        for (int i = 0; i < 10; i++)
        {
        }
    }
}",
@"class C {
    function Foo() {
        for (var i = 0; i < 10; i++) {
        }
    }
}")]

        [TestCase("forWithAnStatement",
@"class C {
    void Foo()
    {
        for (int i = 0; i < 10; i++)
            Console.Write(i);
    }
}",
@"class C {
    function Foo() {
        for (var i = 0; i < 10; i++)
            Console.Write(i);
    }
}")]
        public void ForTests(string title, string input, string expected)
        {
            AssertConversion(input, expected, false);
        }

        [TestCase("simpleForeach",
@"class C {
    void Foo()
    {
        IEnumerable collection = null;
        foreach (Object v in collection)
        {
        }
    }
}",
@"class C {
    function Foo() {
        var collection = null;
        for (var v in collection) {
        }
    }
}", false)]

        [TestCase("typedVariables",
@"class C {
    void Foo(IEnumerable collection)
    {
        foreach (Object v in collection)
        {
        }
    }
}",
@"class C {
    function Foo(collection: IEnumerable) {
        for (var v: Object in collection) {
        }
    }
}", true)]
        [TestCase("foreachWithAnStatement",
@"class C {
    void Foo(IEnumerable coll)
    {
        foreach (var item in coll)
            Console.Write(item);
    }
}",
@"class C {
    function Foo(coll) {
        for (var item in coll)
            Console.Write(item);
    }
}", false)]
        public void ForEachTests(string title, string input, string expected, bool explicitTypedVar)
        {
            AssertConversion(input, expected, explicitTypedVar);
        }

        [Test]
        public void MultipleInitializersAreNotSupported()
        {
            var e = Assert.Throws(Is.InstanceOf<Exception>(),
                () => Convert(
@"class C {
                    void Foo() {
                        for (int i = 0, j = 42; i < j; i++, j--)
                        {
                        }
                    }
                }"));
            StringAssert.Contains("Multiple initializers not supported", e.Message);
        }
    }
}
