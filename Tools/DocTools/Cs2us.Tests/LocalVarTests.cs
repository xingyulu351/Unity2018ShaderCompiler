using System.Collections.Generic;
using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class LocalVarTests : BaseConversionTest
    {
        [TestCase("ArrayDeclarationWithSize", @"Color[] color = new Color[5];", @"var color: Color[] = new Color[5];", true)]
        [TestCase("ArrayDeclaration", @"Color[] color;", @"var color: Color[];", true)]
        [TestCase("ArrayWithImplicitCreation", @"int[] values = {1, 2};", @"var values = [1, 2];", false)]
        [TestCase("ArrayWithInitializer", @"int[] values = new int[] {1, 2};", @"var values = [1, 2];", false)]
        [TestCase("LocalVarDeclarationWithInit", @"Color color = Color.red;", @"var color: Color = Color.red;", true)]
        [TestCase("SimpleLocalVar", @"Color color", @"var color: Color;", true)]
        [TestCase("SimpleLocalVarImplicit", @"Color color", @"var color;", false)]
        [TestCase("objectInitUpper", "object o = new Object();", "var o: Object = new Object();", true)]
        [TestCase("objectInitLower", "object o = new object();", "var o: Object = new Object();", true)]
        [TestCase("objectInitUpperImplicit", "object o = new Object();", "var o = new Object();", false)]
        [TestCase("objectInitLowerImplicit", "object o = new object();", "var o = new Object();", false)]
        [TestCase("stringInit", "string s = \"Hello\"", "var s: String = \"Hello\";", true)]
        [TestCase("nullObjectAssignment", "object o = null", "var o: Object = null;", true)]
        [TestCase("ctorWithSingleArgument", "DateTime f = new DateTime(10);", "var f: DateTime = new DateTime(10);", true)]
        [TestCase("ctorWithMultipleArguments", "DateTime f = new DateTime(1, 5, 1971);", "var f: DateTime = new DateTime(1, 5, 1971);", true)]
        [TestCase("ctorWithMultipleArgumentsImplicit", "DateTime f = new DateTime(1, 5, 1971);", "var f = new DateTime(1, 5, 1971);", false)]
        [TestCase("ctorWithMultipleArgumentsString", "Exception f = new Exception(\"hello\", null);", "var f = new Exception(\"hello\", null);", false)]
        [TestCase("intInit", "int life = 42;", "var life = 42;", false)]
        [TestCase("simpleIntArray", "int []a;", "var a: int[];", true)]
        [TestCase("simpleFloatArray", "float []a;", "var a: float[];", true)]
        [TestCase("simpleDoubleArray", "double []a;", "var a: double[];", true)]
        [TestCase("simpleCharArray", "char []a;", "var a: char[];", true)]
        [TestCase("simpleBool", "bool b;", "var b: boolean;", true)]
        // [TestCase("MultiDiminsionalArrayNoInitializer", @"int[,] values;", @"var values: int[,];", true)] // BEHAVIOR ??
        // [TestCase("MultiDiminsionalArrayNoInitializer", @"int[,] values = { {1, 2} {3, 4} };", @"var values: int[,];", true)]
        public void LocalVarTest(string name, string input, string expected, bool explicitlyTyped)
        {
            AssertConversion(input, expected, explicitlyTyped, false);
        }
    }
}
