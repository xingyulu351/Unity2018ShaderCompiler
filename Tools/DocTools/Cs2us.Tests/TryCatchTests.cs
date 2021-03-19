using NUnit.Framework;

namespace Cs2us.Tests
{
    class TryCatchTests : ClassBasedTests
    {
        [TestCase("TryCatch",
@"class C {
    void Foo() {
        try {
            DoStuff();
        }
        catch(System.Exception err) {
            Debug.Log(""blah"");
        }
    }
}",
@"class C {
    function Foo() {
        try {
            DoStuff();
        }
        catch(err: System.Exception) {
            Debug.Log(""blah"");
        }
    }
}")]
        [TestCase("TryCatchOnlyType",
@"class C {
    void Foo() {
        try {
            DoStuff();
        }
        catch(System.Exception) {
            Debug.Log(""blah"");
        }
    }
}",
@"class C {
    function Foo() {
        try {
            DoStuff();
        }
        catch(System.Exception) {
            Debug.Log(""blah"");
        }
    }
}")]

        [TestCase("TryTwoCatches",
@"class C {
    void Foo() {
        try {
            DoStuff();
        }
        catch(System.Whatever err) {
            Debug.Log(""foo"");
        }
        catch(System.Exception err) {
            Debug.Log(""bar"");
        }
    }
}",
@"class C {
    function Foo() {
        try {
            DoStuff();
        }
        catch(err: System.Whatever) {
            Debug.Log(""foo"");
        }
        catch(err: System.Exception) {
            Debug.Log(""bar"");
        }
    }
}")]
        [TestCase("TryCatchWithTryComment",
@"class C {
    void Foo() {
        //try comment
        try {
            DoStuff();
        }
        catch(System.Exception err) {
            Debug.Log(""blah"");
        }
    }
}",
@"class C {
    function Foo() {
        //try comment
        try {
            DoStuff();
        }
        catch(err: System.Exception) {
            Debug.Log(""blah"");
        }
    }
}")]
        [TestCase("TryCatchWithCatchComment",
@"class C {
    void Foo() {
        try {
            DoStuff();
        }
        //catch comment
        catch(System.Exception err) {
            Debug.Log(""blah"");
        }
    }
}",
@"class C {
    function Foo() {
        try {
            DoStuff();
        }
        //catch comment
        catch(err: System.Exception) {
            Debug.Log(""blah"");
        }
    }
}")]
        public void TryCatch(string title, string input, string expected)
        {
            AssertConversion(input, expected, false);
        }
    }
}
