using System;
using System.Linq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class PerFileIncludeDirectory : NativeProgramTestsBase
    {
        [Test]
        public void PerFileIncludeDirectoryOnCppFile()
        {
            NativeProgram.Sources.Add("reference.cpp", "test.cpp");
            NativeProgram.PerFileIncludeDirectories.Add(new NPath[] {"mydir"}, "test.cpp");
            var units = ObjectFileProductionInstructionses(NativeProgram);
            var defaultInclludes = units.OfType<CLikeCompilationUnitInstruction>().Single(x => x.InputFile == "reference.cpp").CompilerSettings.IncludeDirectories;
            var testIncludes = units.OfType<CLikeCompilationUnitInstruction>().Single(x => x.InputFile == "test.cpp").CompilerSettings.IncludeDirectories;
            CollectionAssert.AreEquivalent(new NPath[] {"mydir"}, testIncludes.Where(x => !defaultInclludes.Contains(x)));
        }

        [Test]
        [Platform(Include = "MacOsX")]
        public void PerFileIncludeDirectoryOnMMFile()
        {
            NativeProgram.Sources.Add("reference.mm", "test.mm");
            NativeProgram.PerFileIncludeDirectories.Add(new NPath[] {"mydir"}, "test.mm");
            var units = ObjectFileProductionInstructionses(NativeProgram);
            var defaultInclludes = units.OfType<CLikeCompilationUnitInstruction>().Single(x => x.InputFile == "reference.mm").CompilerSettings.IncludeDirectories;
            var testIncludes = units.OfType<CLikeCompilationUnitInstruction>().Single(x => x.InputFile == "test.mm").CompilerSettings.IncludeDirectories;
            CollectionAssert.AreEquivalent(new NPath[] {"mydir"}, testIncludes.Where(x => !defaultInclludes.Contains(x)));
        }

        [Test]
        public void SourceFilesWithAbsolutePathsWork()
        {
            NativeProgram.Sources.Add("test.cpp", NPath.SystemTemp.Combine("global.cpp"));

            var toolchain = new MockToolchain();
            var config = new NativeProgramConfiguration(CodeGen.Debug, toolchain, true);

            // should throw no exceptions due to absolute path
            try
            {
                NativeProgram.SetupSpecificConfiguration(config, toolchain.ExecutableFormat, artifactsPath: "artifacts",
                    targetDirectory: "");
            }
            catch (Exception e)
            {
                TestContext.Out.WriteLine(e);
                Assert.Fail(e.ToString());
            }
        }
    }
}
