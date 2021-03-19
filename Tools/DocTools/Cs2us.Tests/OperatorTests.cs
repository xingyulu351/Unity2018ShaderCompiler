using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class OperatorTests : ClassBasedTests
    {
        [TestCase("LessThan",
@"class ExampleClass {
    bool Example() {
        var x = 2 < 3;
    }
}",
            @"var x = 2 < 3;")]

        [TestCase("GreaterThan",
@"class ExampleClass {
    void Example() {
        var x = 2 > 3;
    }
}",
            @"var x = 2 > 3;")]

        [TestCase("Equals",
@"class ExampleClass {
    void Example() {
        var x = 2 == 3;
    }
}",
            @"var x = 2 == 3;")]

        [TestCase("NotEquals",
@"class ExampleClass {
    void Example() {
        var x = 2 != 3;
    }
}",
            @"var x = 2 != 3;")]

        [TestCase("Plus",
@"class ExampleClass {
    void Example() {
        var x = 2 + 3;
    }
}",
            @"var x = 2 + 3;")]
        [TestCase("PlusAndMultiply",
@"class ExampleClass {
    void Example() {
        var x = 2 + 3 * 5;
    }
}",
            @"var x = 2 + 3 * 5;")]

        [TestCase("PlusAndMultiplyParens",
@"class ExampleClass {
    void Example() {
        var x = (2 + 3) * 5;
    }
}",
            @"var x = (2 + 3) * 5;")]

        [TestCase("BinaryAnd",
@"class ExampleClass {
    void Example() {
        var x = 2 && 3;
    }
}",
            @"var x = 2 && 3;")]
        [TestCase("BinaryOr",
@"class ExampleClass {
    void Example() {
        var x = true || false;
    }
}",
            @"var x = true || false;")]

        [TestCase("LogicalNot",
@"class ExampleClass {
    void Example() {
        var x = !true;
    }
}",
            @"var x = !true;")]

        [TestCase("PostIncrement",
@"class ExampleClass {
    void Example() {
        int i=0;
        i++;
    }
}",
@"var i = 0;
i++;")]
        [TestCase("PostDecrement",
@"class ExampleClass {
    void Example() {
        int i=0;
        i--;
    }
}",
@"var i = 0;
i--;")]

        [TestCase("PreIncrement",
@"class ExampleClass {
    void Example() {
        int i=0;
        ++i;
    }
}",
@"var i = 0;
++i;")]
        [TestCase("PreIncrementPlus",
@"class ExampleClass {
    void Example() {
        int i=0;
        int k= ++i + 5;
    }
}",
@"var i = 0;
var k = ++i + 5;")]

        [TestCase("PreDecrement",
@"class ExampleClass {
    void Example() {
        int i=0;
        --i;
    }
}",
@"var i = 0;
--i;")]
        [TestCase("UnaryMinus",
@"class ExampleClass {
    void Example() {
        int i=0;
        int k = -i;
    }
}",
@"var i = 0;
var k = -i;")]
        [TestCase("UnaryMinusPlus",
@"class ExampleClass {
    void Example() {
        int i=0;
        int k = -i + 5;
    }
}",
@"var i = 0;
var k = -i + 5;")]

        [TestCase("TertiaryOperator",
@"class ExampleClass {
    void Example() {
        int i= 2 ? 1 : 0;
    }
}",
            @"var i = 2 ? 1 : 0;")]

        [TestCase("AdditionAssignment",
@"class ExampleClass {
    void Example() {
        var x = 0;
        x += 2;
    }
}",
@"var x = 0;
x += 2;")]
        public void Tests(string title, string input, string expected)
        {
            AssertConversion(input, expected, false);
        }
    }
}
