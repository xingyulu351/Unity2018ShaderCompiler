using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class MemberTests : ClassBasedTests
    {
        [TestCase("SimpleFieldDeclaration",
@"class C {
    int k;
};",
@"class C {
    var k;
}", false)]
        [TestCase("AssignmentWithType",
@"class C {
    Vector3 closestPoint = coll.ClosestPointOnBounds(explosionPos);
};",
@"class C {
    var closestPoint: Vector3 = coll.ClosestPointOnBounds(explosionPos);
}", true)]
        [TestCase("LocalVariableIndentation",
@"class C {
    void Foo() {
        int k;
    }
};",
@"class C {
    function Foo() {
        var k;
    }
}", false)]
        [TestCase("ExplicitFunctionType",
@"class C {
    void Foo() {
    }
};",
@"class C {
    function Foo() {
    }
}", true)]
        [TestCase("FunctionWithParameter",
@"class C
{
    void Foo(int i)
    {
    }
};",
@"class C {
    function Foo(i) {
    }
}", false)]
        [TestCase("FunctionWithParameterExplicit",
@"class C
{
    void Foo(int i)
    {
    }
};",
@"class C {
    function Foo(i: int) {
    }
}", true)]

        [TestCase("FunctionWithMultipleParameters",
@"class C
{
    void Foo(int i, IEnumerable e)
    {
    }
};",
@"class C {
    function Foo(i, e) {
    }
}", false)]
        [TestCase("FunctionWithMultipleParametersExplicit",
@"class C
{
    void Foo(int i, IEnumerable e)
    {
    }
};",
@"class C {
    function Foo(i: int, e: IEnumerable) {
    }
}", true)]
        public void MemberTest(string name, string input, string expected, bool explicitlyTyped)
        {
            AssertConversion(input, expected, explicitlyTyped, true);
        }
    }
}
