using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class IfElseStatementTests : BaseClassConversionTest
    {
        [TestCase("simpleIf",
@"class C {
    void Foo() {
        if (true)
        {
        }
    }
}",
@"class C {
    function Foo() {
        if (true) {
        }
    }
}")]
        [TestCase("simpleIfElse",
@"class C {
    void Foo() {
        if (true)
        {
        }
        else
        {
        }
    }
}",
@"class C {
    function Foo() {
        if (true) {
        }
        else {
        }
    }
}")]
        [TestCase("IfWithStatement",
@"class C {
    void Foo() {
        int x;
        if (true)
        {
            x = 1;
        }
    }
}",
@"class C {
    function Foo() {
        var x;
        if (true) {
            x = 1;
        }
    }
}")]
        [TestCase("IfElseWithStatement",
@"class C {
    void Foo() {
        int x;
        if (true)
        {
            x = 1;
        }
        else
        {
            x = 2;
        }
    }
}",
@"class C {
    function Foo() {
        var x;
        if (true) {
            x = 1;
        }
        else {
            x = 2;
        }
    }
}")]
        [TestCase("IfWithCondition",
@"class C {
    void Foo() {
        int x;
        if (2 < 3)
        {
            x = 1;
        }
    }
}",
@"class C {
    function Foo() {
        var x;
        if (2 < 3) {
            x = 1;
        }
    }
}")]
        [TestCase("IfStatementWithNoBlocks",
@"class C {
    void Foo() {
        int x;
        if (true)
            x = 1;
    }
}",
@"class C {
    function Foo() {
        var x;
        if (true)
            x = 1;
    }
}")]

        [TestCase("IfElseStatementWithNoBlocks",
@"class C {
    void Foo() {
        int x;
        if (true)
            x = 1;
        else
            y = 3;
    }
}",
@"class C {
    function Foo() {
        var x;
        if (true)
            x = 1;
        else
            y = 3;
    }
}")]

        [TestCase("IfWithNoBlocksElseWithBlockStatement",
@"class C {
    void Foo() {
        int x;
        if (true)
            x = 1;
        else
        {
            y = 3;
        }
    }
}",
@"class C {
    function Foo() {
        var x;
        if (true)
            x = 1;
        else {
            y = 3;
        }
    }
}")]

        [TestCase("IfWithBlocksElseWithNoBlockStatement",
@"class C {
    void Foo() {
        int x;
        if (true)
        {
            x = 1;
        }
        else
            y = 3;
    }
}",
@"class C {
    function Foo() {
        var x;
        if (true) {
            x = 1;
        }
        else
            y = 3;
    }
}")]
        public void IfElseTest(string title, string input, string expected)
        {
            AssertClassConversion(input, expected);
        }
    }
}
