using System;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    [TestFixture]
    public class ClassInsteadOfEnumPatternBaseTests
    {
        class MyType : ClassInsteadOfEnumPatternBase
        {
        }

        class MyOtherType : ClassInsteadOfEnumPatternBase
        {
        }

        [Test]
        public void TwoDifferentInstancesCompareAsEqual()
        {
            var one = new MyType();
            var two = new MyType();
            Assert.IsTrue(one == two);
        }

        [Test]
        public void TwoDifferentInstances_AsObjects_Equals()
        {
            object one = new MyType();
            object two = new MyType();
            Assert.IsTrue(Object.Equals(one, two));
        }

        [Test]
        public void TwoDifferentTypesCompareAsNotEqual()
        {
            var one = new MyType();
            var two = new MyOtherType();
            Assert.IsFalse(one == two);
        }

        [Test]
        public void OneInstanceOneNullCompareAsNotEqual()
        {
            var one = new MyType();
            MyType two = null;
            Assert.IsFalse(one == two);
        }

        [Test]
        public void TwoNullsCompareAsEqual()
        {
            MyType one = null;
            MyType two = null;
            Assert.IsTrue(one == two);
        }

        [Test]
        public void TwoNullsOfDifferntTypesCompareAsEqual()
        {
            MyType one = null;
            MyOtherType two = null;
            Assert.IsTrue(one == two);
        }

        [Test]
        public void AsKeyInDictionary()
        {
            var dictionary = new System.Collections.Generic.Dictionary<ClassInsteadOfEnumPatternBase, string>()
            {
                {new MyType(), "mytype"},
                {new MyOtherType(), "othertype"}
            };

            Assert.AreEqual("othertype", dictionary[new MyOtherType()]);
        }

        [Test]
        public void TwoDifferentInstancesHaveSameHashCode()
        {
            Assert.AreEqual(new MyType().GetHashCode(), new MyType().GetHashCode());
        }
    }
}
