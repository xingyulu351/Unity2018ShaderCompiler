using System;
using System.CodeDom.Compiler;
using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class TypeMemberAccessToThrowingMethod : AssemblyPatcherTestBase
    {
        [Test]
        public void TestSpecificMethod()
        {
            try
            {
                Test(@"TestScenarios\MapTypeMemberAccessToThrowingMethod.cs");
                Assert.Fail();
            }
            catch (MissingMemberException mme)
            {
                Assert.That(mme.Message, Contains.Substring("B::CallMethodOnA(A)"));
            }
        }

        [Test]
        public void TestMapAllTypeMethodsToThrowingMethod()
        {
            try
            {
                Test(@"TestScenarios\MapAllTypeMemberAccessToThrowingMethod.cs");
                Assert.Fail();
            }
            catch (MissingMemberException mme)
            {
                Assert.That(mme.Message, Contains.Substring("MethodToBeRemoved"));
            }
        }

        [Test]
        public void TestCallerDetails()
        {
            try
            {
                Test(@"TestScenarios\MapAllTypeMemberAccessToThrowingMethod.cs");
                Assert.Fail();
            }
            catch (MissingMemberException mme)
            {
                Assert.That(mme.Message, Contains.Substring("MapAllTypeMemberAccessToThrowingMethod.cs (9, 9)"));
                Assert.That(mme.Message, Contains.Substring("A::MethodToBeRemoved()"));
            }
        }

        protected override void ConfigureCSharpCompiler(CompilerParameters compilerParameters)
        {
            compilerParameters.CompilerOptions += "/debug:pdbonly";
        }
    }
}
