using NUnit.Framework;

namespace Cs2us.Tests
{
    class SwitchCaseTests : ClassBasedTests
    {
        [TestCase("simpleSwitchDefaultOnly",
@"class C {
    void Foo() {
        int i = 0;
        switch(i)
        {
        default:
            break;
        }
    }
}",
@"class C {
    function Foo() {
        var i = 0;
        switch (i) {
            default:
                break;
        }
    }
}")]
        [TestCase("simpleSwitchDefaultOnlyWithStatement",
@"class C {
    void Foo() {
        int i = 0;
        switch(i) {
            default:
                int k = 0;
                break;
        }
    }
}",
@"class C {
    function Foo() {
        var i = 0;
        switch (i) {
            default:var k = 0;

                break;
        }
    }
}")]

        [TestCase("simpleSwitch1Case",
@"class C {
    void Foo() {
        int i = 0;
        switch(i)
        {
        case 0:
            break;
        }
    }
}",
@"class C {
    function Foo() {
        var i = 0;
        switch (i) {
            case 0:
                break;
        }
    }
}")]

        [TestCase("simpleSwitch2Cases",
@"class C {
    void Foo() {
        int i = 0;
        switch(i)
        {
        case 0:
            break;
        case 1:
            break;
        }
    }
}",
@"class C {
    function Foo() {
        var i = 0;
        switch (i) {
            case 0:
                break;
            case 1:
                break;
        }
    }
}")]

        [TestCase("DeeperSwitchCase2",
@"class C {
    void Foo() {
        int i = 0;
        switch(i)
        {
        case 0:
            Console.WriteLine(""hello"");
            break;
        case 1:
            break;
        }
    }
}",
@"class C {
    function Foo() {
        var i = 0;
        switch (i) {
            case 0:
                Console.WriteLine(""hello"");

                break;
            case 1:
                break;
        }
    }
}")]


        [TestCase("DeeperSwitchCase1",
@"class C {
    void Foo() {
        int i = 0;
        switch(i)
        {
        case 0:
            Console.WriteLine(""hello"");
            break;
        }
    }
}",
@"class C {
    function Foo() {
        var i = 0;
        switch (i) {
            case 0:
                Console.WriteLine(""hello"");

                break;
        }
    }
}")]
        //switch cases in UnityScript are pretty much exact copies in syntax of their C# counterparts
        public void SwitchTests(string title, string input, string expected)
        {
            AssertConversion(input, expected, false);
        }
    }
}
