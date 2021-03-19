using System;
using System.CodeDom.Compiler;
using System.IO;
using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class ResignAssemblyTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            //The test scenario is not important here. We just want to make sure that it
            //runs green and that after patching the target assembly still have a valid
            //signature.
            var testSpec = CompileTestAssemblyAndExtractConfigFor("TestScenarios/VoidMethodBodyRemoval.cs", TestBaseDir);

            Assert.IsTrue(HasValidSignature(testSpec.AssemblyPath));

            AssertUnpatchedAssembly(testSpec);
            var patchedAssemblyPath = PatchSignedAssembly(testSpec, TestAssemblyKeySnkPath);

            Assert.IsTrue(HasValidSignature(patchedAssemblyPath));

            PEVerify(patchedAssemblyPath);
            AssertPatchedAssembly(testSpec, patchedAssemblyPath);
        }

        protected override void ConfigureCSharpCompiler(CompilerParameters compilerParameters)
        {
            compilerParameters.CompilerOptions += " /keyfile:" + TestAssemblyKeySnkPath;
        }
    }
}
