using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class CommentTests : BaseClassConversionTest
    {
        [TestCase("EmptyEnumWithComment",
@"//hello
enum EmptyEnum {
}",
@"//hello
enum EmptyEnum {
}")]

        [TestCase("EmptyEnumWithMultilineComment", //passes, the output is syntactically correct, but weird
@"/*hello*/
enum EmptyEnum {
}",
@"/*hello*/enum EmptyEnum {
}")]

        [TestCase("EmptyEnumWithMidComment",
@"enum EmptyEnum {
//hello
}",
@"enum EmptyEnum {
    //hello
}")]

        [TestCase("EmptyEnumWithEndComment",
@"enum EmptyEnum {
}
//hello",
@"enum EmptyEnum {
}
//hello")]

        [TestCase("EmptyEnumWithStartMidComment",
@"//hello
enum EmptyEnum {
//hello2
}",
@"//hello
enum EmptyEnum {
    //hello2
}")]

        [TestCase("EnumWithInitializerComment",
@"enum EnumWithInitializer {
    //first
    First,
    Second = 2,
    Last,
}",
@"enum EnumWithInitializer {
    //first
    First,
    Second = 2,
    Last,
}"
         )]

        [TestCase("EnumWithPostComment",
@"enum EnumWithPostComment {
    First, //first
    Last,
}",
@"enum EnumWithPostComment {
    First,
    //first
    Last,
}"
         )]

        [TestCase("foreachWithAnStatementWithComment",
@"class C {
    void Foo(IEnumerable coll)
    {
        foreach (var item in coll) {
            //test comment
            Console.Write(item);
        }
    }
}",
@"class C {
    function Foo(coll) {
        for (var item in coll) {
            //test comment
            Console.Write(item);
        }
    }
}")]
        [TestCase("foreachWithAnStatementWithCommentBeforeFor",
@"class C {
    void Foo(IEnumerable coll)
    {
        //test comment
        foreach (var item in coll)
            Console.Write(item);
    }
}",
@"class C {
    function Foo(coll) {
        //test comment
        for (var item in coll)
            Console.Write(item);
    }
}")]

        [TestCase("forWithComment",
@"class C {
    void Foo()
    {
        //testcomment
        for (int i = 0; i < 10; i++) {
        }
    }
}",
@"class C {
    function Foo() {
        //testcomment
        for (var i = 0; i < 10; i++) {
        }
    }
}")]
        [TestCase("simpleIfWithComment",
@"class C {
    void Foo() {
        //test comment
        if (true)
        {
        }
    }
}",
@"class C {
    function Foo() {
        //test comment
        if (true) {
        }
    }
}")]
        [TestCase("simpleIfWithInternalComment",
@"class C {
    void Foo() {
        if (true) {
            //test comment
        }
    }
}",
@"class C {
    function Foo() {
        if (true) {
            //test comment
        }
    }
}")]
        [TestCase("simpleMethodCallWithComment",
@"class C {
    void M(C other)
    {
        //this is a method call
        other.M(this);
    }
}",
@"class C {
    function M(other) {
        //this is a method call
        other.M(this);
    }
}")]

        [TestCase("funcDeclWithComment",
@"class C {
    //this is a function
    void M(C other)
    {
    }
}",
@"class C {
    //this is a function
    function M(other) {
    }
}")]
        [TestCase("varDeclWithComment",
@"public class C {
    public void foo() {
        // get the current keywords from the material
        var targetMat = materialEditor.targetMaterial;
    }
}",
@"public class C {
    public function foo() {
        // get the current keywords from the material
        var targetMat = materialEditor.targetMaterial;
    }
}")]


        [TestCase("simpleSwitchDefaultOnlyWithComment",
@"class C {
    void Foo() {
        int i = 0;
        //test comment
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
        //test comment
        switch (i) {
            default:
                break;
        }
    }
}")]
        [TestCase("simpleSwitchCommentBeforeBreak",
@"class C {
    void Foo() {
        int i = 0;
        switch(i)
        {
            default:
                //before break
                break;
        }
    }
}",
@"class C {
    function Foo() {
        var i = 0;
        switch (i) {
            default://before break

                break;
        }
    }
}")]
        [TestCase("simpleSwitchCommentBeforeCase",
@"class C {
    void Foo() {
        switch (touch.phase) {
            // Record initial touch position.
            case TouchPhase.Began:
                directionChosen = false;
                break;
            default:

                directionChosen = true;
                break;
        }
    }
}",
@"class C {
    function Foo() {
        switch (touch.phase) {
            // Record initial touch position.
            case TouchPhase.Began:
                directionChosen = false;

                break;
            default:
                directionChosen = true;

                break;
        }
    }
}")]
        [TestCase("simpleSwitchDoubleCommentBeforeCase", //fails because it only picks up the last comment line
@"class C {
    void Foo() {
        switch (touch.phase) {
            // Record initial touch position.
            // and do other stuff
            case TouchPhase.Began:
                directionChosen = false;
                break;
            default:

                directionChosen = true;
                break;
        }
    }
}",
@"class C {
    function Foo() {
        switch (touch.phase) {
            // Record initial touch position.
            // and do other stuff
            case TouchPhase.Began:
                directionChosen = false;

                break;
            default:
                directionChosen = true;

                break;
        }
    }
}")]

        public void CommentTest(string title, string input, string expected)
        {
            AssertClassConversion(input, expected);
        }
    }
}
