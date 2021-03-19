using System;
using System.Linq;
using Bee.ToolchainTestInfrastructure;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;
using Unity.BuildTools;

namespace Bee.Toolchain.Windows.Tests
{
    [TestFixture]
    class WindowsToolChainTests : SingleToolchainTests
    {
        protected override ToolChain ProduceToolchainToTest() => new WindowsToolchain(WindowsSdk.Locatorx64.UserDefaultOrDummy);

        [Test]
        [Platform(Include = "Win")]
        public void MultiPCHCreatesMultiPDB()
        {
            TestRoot.Combine("MyPrefix.h")
                .WriteAllText(
@"
#if DO_ERROR
#error
#endif");
            TestRoot.Combine("file1.cpp")
                .WriteAllText(
@"
int howdy() {return 5;}");
            TestRoot.Combine("file2.cpp")
                .WriteAllText(
@"
#include <stdio.h>
void nothappening() {printf(""this could never happen\n"");}");
            TestRoot.Combine("main.cpp")
                .WriteAllText(
@"
int main() {
return 0;
}");
            Build(
                toolChain => SetupNativeProgram(
                    toolChain,
                    np =>
                    {
                        np.DefaultPCH = "MyPrefix.h";
                        np.PerFileDefines.Add(new[] {"DO_ERROR=0"}, "file1.cpp", "main.cpp");
                        np.PerFileDefines.Add(new[] {"DO_ERROR=1"}, "file2.cpp");
                    }), false);
            Assert.That(TestRoot.Files(recurse: true).Count(f => f.HasExtension("pdb")) == 2);
        }
    }
}
