using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.VisualStudio;
using Bee.Toolchain.Xcode;
using NUnit.Framework;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.IntegrationTests
{
    [TestFixture]
    [Platform(Include = "MacOsX")]
    public class CompilerCustomizationTests
    {
        private XcodeToolchain ToolChain;
        private XcodeClangCompiler Compiler;
        private XcodeClangCompilerSettings CompilerSettings1;
        private XcodeClangCompilerSettings CompilerSettings2;
        private NativeProgramConfiguration Config;

        private CompilerCustomizations CompilerCustomizations;

        [SetUp]
        public void SetUp()
        {
            ToolChain = new MacToolchain(MacSdk.Locator.UserDefaultOrLatest, new x64Architecture());
            Compiler = new XcodeClangCompiler(ToolChain);

            CompilerSettings1 = new XcodeClangCompilerSettings(Compiler);
            CompilerSettings2 = new XcodeClangCompilerSettings(Compiler);

            CompilerCustomizations = new CompilerCustomizations();
            Config = new NativeProgramConfiguration(
                CodeGen.Debug,
                new MacToolchain(MacSdk.Locator.UserDefaultOrLatest, new x86Architecture(), "10.12"),
                true);
        }

        [Test]
        public void WithoutCustomization()
        {
            Assert.AreEqual(CompilerSettings1, CompilerCustomizations.For(Config, CompilerSettings1, "file1.cpp"));
        }

        [Test]
        public void WithCustomization()
        {
            CompilerCustomizations.Add<XcodeClangCompilerSettings>(c => {
                Assert.AreEqual(CompilerSettings1, c);
                return CompilerSettings2;
            });
            Assert.AreEqual(CompilerSettings2, CompilerCustomizations.For(Config, CompilerSettings1, "file1.cpp"));
        }

        [Test]
        public void WithPerFileCustomization()
        {
            CompilerCustomizations.Add<XcodeClangCompilerSettings>(c => {
                Assert.AreEqual(CompilerSettings1, c);
                return CompilerSettings2;
            }, "file1.cpp");
            Assert.AreEqual(CompilerSettings2, CompilerCustomizations.For(Config, CompilerSettings1, "file1.cpp"));
        }

        [Test]
        public void WithNonMatchingPerFileCustomization()
        {
            CompilerCustomizations.Add<XcodeClangCompilerSettings>(c => {
                throw new AssertionException("shouldnt be called");
            }, "file1.cpp");
            Assert.AreEqual(CompilerSettings1, CompilerCustomizations.For(Config, CompilerSettings1, "file2.cpp"));
        }

        [Test]
        public void WithNonMatchingCustomization()
        {
            CompilerCustomizations.Add<MsvcCompilerSettings>(c => {
                throw new AssertionException("shouldnt be called");
            });
            Assert.AreEqual(CompilerSettings1, CompilerCustomizations.For(Config, CompilerSettings1, "file1.cpp"));
        }
    }
}
