using NUnit.Framework;
using System;
using System.Collections.Generic;

namespace Unity.BindingsGenerator.TestFramework
{
    [TestFixture]
    internal class AssertEqualTests : BindingsGeneratorTestsBase
    {
        #pragma warning disable 649

        [Test]
        public void CatchingWorks()
        {
            Assert.Catch(delegate
            {
                Assert.True(false);
            });
        }

        [Test]
        public void UnsupportedPrimitiveTypeComparisonThrows()
        {
            Assert.Throws<NotImplementedException>(delegate { AssertEqual(90.0, 90.0); });
        }

        private class ClassWithField
        {
            public string Hello;
        }

        [Test]
        public void FieldComparisonThrows()
        {
            Assert.Throws<NotImplementedException>(delegate { AssertEqual(new ClassWithField(), new ClassWithField()); });
        }

        [Test]
        public void EqualStringsPass()
        {
            AssertEqual("a", "a");
        }

        [Test]
        public void StringWithNullExpectedPasses()
        {
            AssertEqual(null, "a");
        }

        [Test]
        public void NonEqualStringsAsserts()
        {
            Assert.Catch(delegate { AssertEqual("a", "b"); });
        }

        private class SP
        {
            public string S { get; set; }
        }

        [Test]
        public void StringPropertyComparisonPasses()
        {
            AssertEqual(new SP {S = "a"}, new SP {S = "a"});
        }

        [Test]
        public void StringPropertyWithNullExpectedComparisonPasses()
        {
            AssertEqual(new SP {}, new SP {S = "a"});
        }

        [Test]
        public void StringPropertyComparisonAsserts()
        {
            Assert.Catch(delegate
            {
                AssertEqual(new SP {S = "a"},
                    new SP {S = "b"});
            });
        }

        [Test]
        public void EmptyListWithExpectedNullPasses()
        {
            AssertEqual(null, new List<string> {});
        }

        [Test]
        public void ListWithExpectedNullPasses()
        {
            AssertEqual(new List<string> {"a", null}, new List<string> {"a", "b"});
        }

        [Test]
        public void StringListPasses()
        {
            AssertEqual(new List<string> {"a", "b"}, new List<string> {"a", "b"});
        }

        [Test]
        public void StringListAsserts()
        {
            Assert.Catch(delegate
            {
                AssertEqual(new List<string> {"a", "b"}, new List<string> {"a", "c"});
            });
        }

        [Test]
        public void ListClassStringPropPasses()
        {
            AssertEqual(new List<SP> { new SP { S = "a"}, new SP {S = "b"}  }, new List<SP> { new SP { S = "a"}, new SP {S = "b"}  });
        }

        [Test]
        public void ListClassStringPropWithExpectedNullPasses1()
        {
            AssertEqual(new List<SP> { new SP { S = "a"}, new SP {S = null}  }, new List<SP> { new SP { S = "a"}, new SP {S = "b"}  });
        }

        [Test]
        public void ListClassStringPropWithExpectedNullPasses2()
        {
            AssertEqual(new List<SP> { new SP { S = "a"}, null  }, new List<SP> { new SP { S = "a"}, new SP {S = "b"}  });
        }

        [Test]
        public void ListClassStringPropWithExpectedNullPasses3()
        {
            AssertEqual(null, new List<SP> { new SP { S = "a"}, new SP {S = "b"}  });
        }

        [Test]
        public void ListClassStringPropWithActualNullAsserts1()
        {
            Assert.Catch(delegate
            {
                AssertEqual(new List<SP> {new SP {S = "a"}, new SP {S = "b"}}, new List<SP> {new SP {S = "a"}, new SP {S = null}});
            });
        }

        [Test]
        public void ListClassStringPropWithActualNullAsserts2()
        {
            Assert.Catch(delegate
            {
                AssertEqual(new List<SP> {new SP {S = "a"}, new SP {S = "b"}}, new List<SP> {new SP {S = "a"}, null});
            });
        }

        [Test]
        public void ListClassStringPropWithActualNullAsserts3()
        {
            Assert.Catch(delegate
            {
                AssertEqual(new List<SP> {new SP {S = "a"}, new SP {S = "b"}}, null);
            });
        }

        [Test]
        public void ListClassStringPropAsserts()
        {
            Assert.Catch(delegate
            {
                AssertEqual(new List<SP> { new SP { S = "a"}, new SP {S = "b"}  }, new List<SP> { new SP { S = "a"}, new SP {S = "c"}  });
            });
        }

        [Test]
        public void ListOfListClassStringPropPasses()
        {
            var expected = new List<List<SP>> { new List<SP> { new SP { S = "a" } } };
            var actual = new List<List<SP>> { new List<SP> { new SP { S = "a" } } };
            AssertEqual(expected, actual);
        }

        [Test]
        public void ListOfListClassStringPropAsserts()
        {
            var expected = new List<List<SP>> { new List<SP> { new SP { S = "a" } } };
            var actual = new List<List<SP>> { new List<SP> { new SP { S = "b" } } };
            Assert.Catch(delegate
            {
                AssertEqual(expected, actual);
            });
        }

        [Test]
        public void ListOfListClassStringPropExpectationNullPasses1()
        {
            var actual = new List<List<SP>> { new List<SP> { new SP { S = "a" } } };
            AssertEqual(null, actual);
        }

        [Test]
        public void ListOfListClassStringPropExpectationNullPasses2()
        {
            var expected = new List<List<SP>> {null};
            var actual = new List<List<SP>> { new List<SP> { new SP { S = "a" } } };
            AssertEqual(expected, actual);
        }

        private class T1
        {
        }

        private class T2
        {
        }

        [Test]
        public void DifferentTypesAssert()
        {
            var expected = new T1();
            var actual = new T2();

            Assert.Catch(delegate
            {
                AssertEqual(expected, actual);
            });
        }
    }
}
