using System;
using Bee.Core;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class NativeProgramTests : NativeProgramTestsBase
    {
        [Test]
        public void ToolchainInNativeConfigurationDoesNotMatch()
        {
            var toolchain1 = new MockToolchain();
            var toolchain2 = new MockToolchain();
            var config = new NativeProgramConfiguration(CodeGen.Debug, toolchain1, true);

            Assert.Throws<ArgumentException>(() => NativeProgram.SetupSpecificConfiguration(config, toolchain2.ExecutableFormat));
        }

        [Test]
        public void AllIncludeDirectoriesFor()
        {
            var mylib2 = new NativeProgram("mylib2") {PublicIncludeDirectories = {"third"}, IncludeDirectories = { "dontincludeme"}};
            var mylib = new NativeProgram("mylib") {PublicIncludeDirectories = {"mydir", "myotherdir"}, StaticLibraries = { mylib2}};
            var np = new NativeProgram("hallo") {IncludeDirectories = {"mydir", "folder"}, PublicIncludeDirectories = { "includeme"}, StaticLibraries = {mylib}};

            CollectionAssert.AreEquivalent(new NPath[] {".", "mydir", "folder", "includeme", "myotherdir", "third"}, np.AllIncludeDirectoriesFor(new NativeProgramConfiguration(CodeGen.Debug, new MockToolchain(), true)));
        }

        [Test]
        public void AllDefinesFor()
        {
            var mylib2 = new NativeProgram("mylib2") {PublicDefines = {"third"}, Defines = { "dontincludeme"}};
            var mylib = new NativeProgram("mylib") {PublicDefines = { "mydir", "myotherdir"}, StaticLibraries = { mylib2}};
            var np = new NativeProgram("hallo") {Defines = {"mydir", "folder"}, PublicDefines = { "includeme"}, StaticLibraries = {mylib}};

            CollectionAssert.AreEquivalent(new[] {"mydir", "folder", "includeme", "myotherdir", "third"}, np.AllDefinesFor(new NativeProgramConfiguration(CodeGen.Debug, new MockToolchain(), true)));
        }

        [Test]
        public void SourceFilesAlreadyUnderArtifactsDontGetObjectFilePathsDuplicated()
        {
            NativeProgram.Sources.Add($"artifacts/test.cpp");
            var instr = ObjectFileProductionInstructionses(NativeProgram);
            Assert.That(instr.Length, Is.EqualTo(1));
            var path = NativeProgram.ObjectPathFor("artifacts", instr[0]);
            Assert.That(path, Is.EqualTo(new NPath("artifacts/test.o")));
        }
    }
}
