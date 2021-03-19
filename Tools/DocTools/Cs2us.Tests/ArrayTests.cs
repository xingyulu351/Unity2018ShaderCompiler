using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class ArrayTests : ClassBasedTests
    {
        [Test]
        public void SingleDimensionCreation()
        {
            AssertConversion(
@"class C
              {
                int[] arr = new int[42];
              }",

@"class C {
    var arr: int[] = new int[42];
}",

                true);
        }

        [Test]
        public void SingleDimensionAccess()
        {
            AssertConversion(
@"class ExampleClass
              {
                void Example()
                {
                    int[] arr = new int[42];
                    arr[0] = 10;
                }
              }",

@"var arr: int[] = new int[42];
arr[0] = 10;",

                true);
        }

        [Test]
        public void TypedMultiDimensionAccess()
        {
            AssertConversion(
@"class ExampleClass
              {
                void Example()
                {
                    int[,] arr = new int[2,2];
                    arr[0,1] = 10;
                }
              }",

@"var arr: int[,] = new int[2, 2];
arr[0, 1] = 10;",

                true);
        }

        [Test]
        public void InferedMultiDimensionAccess()
        {
            AssertConversion(
@"class ExampleClass
              {
                void Example()
                {
                    int[,] arr = new int[2,2];
                    arr[0,1] = 10;
                }
              }",

@"var arr = new int[2, 2];
arr[0, 1] = 10;",

                false);
        }
    }
}
