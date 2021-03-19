using System.CodeDom.Compiler;
using System.IO;
using System.Linq;
using Mono.Cecil;
using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class SignedAssembliesTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void TestReferencesToSignedAssembliesUsesPublicKeyToken()
        {
            //We want to make sure that the test runs green and that after patching the
            //reference to the signed replacement assembly is done through the public
            //key token instead of the full public key.

            Test(
                "TestScenarios/Fields.cs",
                patchedAssembly =>
                {
                    var externalAssemblyRef = patchedAssembly.MainModule.AssemblyReferences.Single(ar => ar.Name.Contains("ExternalAssembly"));
                    if (externalAssemblyRef.HasPublicKey)
                    {
                        Assert.That(externalAssemblyRef.HasPublicKey, Is.False, "Reference to " + externalAssemblyRef.Name + " should use public key token instead of full public key");
                    }

                    Assert.That(externalAssemblyRef.PublicKeyToken, Is.Not.Null);
                },
                "AssemblyPatcher.Tests.ExternalAssembly.dll");
        }

        [Test]
        public void TestStrongNameIsRemovedIfKeyNotPassed()
        {
            //The test scenario is not important here. We just want to make sure that it
            //runs green and that after patching the target assembly still have a valid
            //signature.
            var testSpec = CompileTestAssemblyAndExtractConfigFor("TestScenarios/VoidMethodBodyRemoval.cs", TestBaseDir);

            Assert.IsTrue(HasValidSignature(testSpec.AssemblyPath));

            AssertUnpatchedAssembly(testSpec);
            var patchedAssemblyPath = PatchAssembly(testSpec);

            Assert.IsFalse(HasStrongName(patchedAssemblyPath));

            PEVerify(patchedAssemblyPath);
            AssertPatchedAssembly(testSpec, patchedAssemblyPath);
        }

        private bool HasStrongName(string patchedAssemblyPath)
        {
            using (var assemblyStream = File.Open(patchedAssemblyPath, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                return AssemblyDefinition.ReadAssembly(assemblyStream).Name.HasPublicKey;
            }
        }

        protected override void ConfigureCSharpCompiler(CompilerParameters compilerParameters)
        {
            compilerParameters.CompilerOptions += " /keyfile:" + TestAssemblyKeySnkPath;
        }
    }
}
