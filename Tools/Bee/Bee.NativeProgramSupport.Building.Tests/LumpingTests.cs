using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class LumpingTests : NativeProgramTestsBase
    {
        [Test]
        public void TwoFilesGetLumpedTogether()
        {
            NativeProgram.Sources.Add("file1.cpp", "file2.cpp");
            Assert.AreEqual(1, ObjectFileProductionInstructionses(NativeProgram).Length);
        }

        [Test]
        public void DifferentDefinesCausesFilesToNotLumpTogether()
        {
            NativeProgram.Sources.Add("file1.cpp", "file2.cpp");
            NativeProgram.PerFileDefines.Add(new[] {"HARRY=1"}, "file2.cpp");
            Assert.AreEqual(2, ObjectFileProductionInstructionses(NativeProgram).Length);
        }

        [Test]
        public void DifferentIncludesCausesFilesToNotLumpTogether()
        {
            NativeProgram.Sources.Add("file1.cpp", "file2.cpp");
            NativeProgram.PerFileIncludeDirectories.Add(new NPath[] {"mydir"}, "file2.cpp");
            Assert.AreEqual(2, ObjectFileProductionInstructionses(NativeProgram).Length);
        }

        [Test]
        public void DifferentCompilerSettingsCausesFilesToNotLumpTogether()
        {
            NativeProgram.Sources.Add("file1.cpp", "file2.cpp");
            NativeProgram.CompilerSettings().Add(c => c.WithSSE(SIMD.sse), "file1.cpp");
            NativeProgram.CompilerSettings().Add(c => c.WithSSE(SIMD.sse2), "file2.cpp");

            Assert.AreEqual(2, ObjectFileProductionInstructionses(NativeProgram).Length);
        }

        [Test]
        public void LumpedFileNames_AreBasedOn_ParentFolderPlusCounter()
        {
            NativeProgram.Sources.Add("Foo/Bar/file1.cpp", "Foo/Bar/file2.cpp");
            var instr = ObjectFileProductionInstructionses(NativeProgram);
            Assert.AreEqual(1, instr.Length);
            Assert.AreEqual("artifacts/Foo_Bar_0.cpp", instr[0].InputFile.ToString());
        }

        [Test]
        public void LumpedFileNames_AreBasedOn_ParentFolderPlusCounter_WhenInputHasJustOneParent()
        {
            NativeProgram.Sources.Add("Foo/file1.cpp", "Foo/file2.cpp");
            var instr = ObjectFileProductionInstructionses(NativeProgram);
            Assert.AreEqual(1, instr.Length);
            Assert.AreEqual("artifacts/Foo_0.cpp", instr[0].InputFile.ToString());
        }

        [Test]
        public void LumpedFileNames_AreBasedOn_Counter_WhenInputIsAtRoot()
        {
            NativeProgram.Sources.Add("file1.cpp", "file2.cpp");
            var instr = ObjectFileProductionInstructionses(NativeProgram);
            Assert.AreEqual(1, instr.Length);
            Assert.AreEqual("artifacts/0.cpp", instr[0].InputFile.ToString());
        }

        [Test]
        public void LumpedFileNames_AreBasedOn_ParentFolder_WorksWhenFolderHasDots()
        {
            NativeProgram.Sources.Add("Foo.Bar/file1.cpp", "Foo.Bar/file2.cpp", "Foo.Baz/file3.cpp", "Foo.Baz/file4.cpp");
            var instr = ObjectFileProductionInstructionses(NativeProgram);
            Assert.AreEqual(2, instr.Length);
            Assert.AreEqual("artifacts/Foo.Bar_0.cpp", instr[0].InputFile.ToString());
            Assert.AreEqual("artifacts/Foo.Baz_0.cpp", instr[1].InputFile.ToString());
        }
    }
}
