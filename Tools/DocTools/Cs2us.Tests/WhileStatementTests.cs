using NUnit.Framework;

namespace Cs2us.Tests
{
    class WhileStatementTests : ClassBasedTests
    {
        [TestCase("simpleWhile",
@"class C {
    void Foo() {
        while(true)
        {
        }
    }
}",
@"class C {
    function Foo() {
        while ( true ) {
        }
    }
}")]
        [TestCase("simpleWhileNoBlock",
@"class C {
    void Foo() {
        while(true)
            Console.Write(""hello"");
    }
}",
@"class C {
    function Foo() {
        while ( true )
            Console.Write(""hello"");
    }
}")]
        [TestCase("normalWhile",
@"class C {
    void Foo() {
        while(true)
        {
            Console.Write(""hello"");
        }
    }
}",
@"class C {
    function Foo() {
        while ( true ) {
            Console.Write(""hello"");
        }
    }
}")]
        public void WhileTests(string title, string input, string expected)
        {
            AssertConversion(input, expected, false);
        }
    }
}
