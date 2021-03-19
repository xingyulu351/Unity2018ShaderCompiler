using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class GenericsTests : ClassBasedTests
    {
        [TestCase("SimpleListInstantiation",
@"class ExampleClass {
    void Example() {
        var v = new System.Collections.Generic.List<int>();
    }
}",
            @"var v = new System.Collections.Generic.List.<int>();")]

        [TestCase("SimpleGetComponent",
@"class ExampleClass {
    void Example() {
        var rend = GetComponent<Renderer>();
    }
}",
            @"var rend = GetComponent.<Renderer>();")]

        [TestCase("SimpleDictionatyInstantiation",
@"class ExampleClass {
    void Example() {
        var v = new System.Collections.Generic.Dictionary<int, string>();
    }
}",
            @"var v = new System.Collections.Generic.Dictionary.<int, String>();")]

        [TestCase("GenericMethodExplicitTypeArgument",
@"class ExampleClass {
    void Example() {
        var il = new System.Collections.Generic.List<int>();
        var sl = il.ConvertAll<string>(i => i.ToString());
    }
}",
@"var il = new System.Collections.Generic.List.<int>();
var sl = il.ConvertAll.<String>(function(i) {
    return i.ToString();
});")]

        [TestCase("GenericMethodInferredTypeArgument",
@"class ExampleClass {
    void Example() {
        var il = new System.Collections.Generic.List<int>();
        var sl = il.ConvertAll(i => i.ToString());
    }
}",
@"var il = new System.Collections.Generic.List.<int>();
var sl = il.ConvertAll.<String>(function(i) {
    return i.ToString();
});")]
        [TestCase("GenericClass",
@"public class MyIntEvent : UnityEvent<int, int, int>
{
}",
@"public class MyIntEvent extends UnityEvent.<int, int, int> {
}")]
        [TestCase("GenericDictionary",
@"class ExampleClass {
    void Example() {
        var dict = new Dictionary<string, object> ();
    }
}",
            @"var dict = new Dictionary.<String,Object>();")]
        public void GenericTests(string title, string input, string expected)
        {
            AssertConversion(input, expected, false);
        }
    }
}
