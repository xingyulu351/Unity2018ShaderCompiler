using System;
using System.Linq;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.Tools.Tests
{
    [TestFixture]
    public class EnumerableExtensionsTests
    {
        private readonly int[] testArray = new[] {0, 1, 2, 3};

        private int SelectOddAndThrowExceptionForEven(int i)
        {
            if (i % 2 == 1)
                return i;
            throw new Exception(i.ToString());
        }

        private void HandleExceptionAndAssertForEvenNumber(Exception e)
        {
            Assert.That(int.Parse(e.Message) % 2 == 0);
        }

        [Test]
        public void CatchExceptions_WithoutAction_IgnoresExceptions()
        {
            Assert.DoesNotThrow(() =>
            {
                testArray
                    .Select(SelectOddAndThrowExceptionForEven)
                    .CatchExceptions()
                    .ToArray();
            });
        }

        [Test]
        public void CatchExceptions_WithAction_IgnoresExceptionsAndCallsActionWithCorrectException()
        {
            int numThrows = 0;
            Assert.DoesNotThrow(() =>
            {
                testArray
                    .Select(SelectOddAndThrowExceptionForEven)
                    .CatchExceptions(e =>
                    {
                        Assert.That(int.Parse(e.Message) % 2 == 0);
                        ++numThrows;
                    })
                    .ToArray();
            });
            Assert.AreEqual(testArray.Count(i => i % 2 == 0), numThrows);
        }

        [Test]
        public void Append()
        {
            var a = new[] {1, 2, 3};
            CollectionAssert.AreEquivalent(new[] { 1, 2, 3, 4}, a.Append(4));
            Assert.AreEqual(3, a.Length);
        }

        [Test]
        public void FilterByBool()
        {
            var a = new[] {1, 2, 3, 4, 5, 6};

            a.FilterByBool(i => i < 3, out var match, out var notmatch);

            CollectionAssert.AreEquivalent(new[] {1, 2}, match);
            CollectionAssert.AreEquivalent(new[] {3, 4, 5, 6}, notmatch);
        }
    }
}
