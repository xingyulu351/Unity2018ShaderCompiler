using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class MethodTests : ClassBasedTests
    {
        [TestCase(true)]
        [TestCase(false)]
        [Test]
        public void SimpleAction(bool explictTypedVars)
        {
            AssertConversion(
@"class C
              {
                Action a = delegate() { };
              }",

@"class C {
    var a = function() {
    };
}", explictTypedVars);
        }

        [Test]
        public void SimpleMethodCall()
        {
            AssertConversion(
@"class C
              {
                void M(C other)
                {
                    other.M(this);
                }
              }",

@"class C {
    function M(other: C) {
        other.M(this);
    }
}", true);
        }

        [Test]
        public void SimpleStaticMethodDeclaration()
        {
            AssertConversion(
@"class C
              {
                static void M(int number)
                {
                    C.M(number - 1);
                }
              }",

@"class C {
    static function M(number: int) {
        C.M(number - 1);
    }
}", true);
        }

        [Test]
        public void StaticMethodCall()
        {
            AssertConversion(
@"class C
              {
                string M()
                {
                    return string.Format(""{0}"", 42);
                }
              }",

@"class C {
    function M() {
        return String.Format(""{0}"", 42);
    }
}", false);
        }

        [Test]
        public void LambdaExpressionsAsParameters()
        {
            AssertConversion(
@"using System;
              class C
              {
                String M(Func<int, string> f)
                {
                    return f(42);
                }

                void DoIt()
                {
                    M( i => i.ToString() );
                }
              }",

@"class C {
    function M(f: function(int): String): String {
        return f(42);
    }
    function DoIt(): void {
        M(function(i) {
            return i.ToString();
        });
    }
}", true);
        }

        [Test]
        public void ActionExpressionsAsParameters()
        {
            AssertConversion(
@"using System;
              class C
              {
                void M(Action<int> f)
                {
                    f(42);
                }

                void DoIt()
                {
                    M( i => Console.Write(i) );
                }
              }",

@"class C {
    function M(f: function(int): void): void {
        f(42);
    }
    function DoIt(): void {
        M(function(i) {
            Console.Write(i);
        });
    }
}", true);
        }

        [Test]
        public void EmptyTest()
        {
            AssertConversion(
@"using UnityEngine;
using System.Collections;
public class Example : MonoBehaviour {
    void Update () {}
}",
@"public class Example {
    function Update(): void {
    }
}"
                , true);
        }
    }
}
