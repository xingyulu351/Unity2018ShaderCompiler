using System.Linq;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class PublicDefines : NativeProgramTestsBase
    {
        private static readonly string ExtraDefine = "mydefine";
        private NativeProgram Lib;

        [SetUp]
        public override void SetUp()
        {
            base.SetUp();
            Lib = new NativeProgram("mylib");
            NativeProgram.Sources.Add("myfile.cpp");
        }

        [Test]
        public void OnStaticLibrary()
        {
            Lib.PublicDefines.Add(ExtraDefine);
            NativeProgram.StaticLibraries.Add(Lib);
            CollectionAssert.Contains(ResultingDefinesFor("myfile.cpp"), ExtraDefine);
        }

        [Test]
        public void OnStaticLibraryWithCustomizationCallback()
        {
            NativeProgram.StaticLibraries.Add(new FromSourceLibraryDependency { Library = Lib, ConfigurationCustomizationCallback = c => c.WithCodeGen(CodeGen.Release)});
            Lib.PublicDefines.Add(c => c.CodeGen == CodeGen.Release, ExtraDefine);
            CollectionAssert.Contains(ResultingDefinesFor("myfile.cpp"), ExtraDefine);
        }

        [Test]
        public void PropagatesMultipleDependencies()
        {
            NativeProgram.StaticLibraries.Add(Lib);

            var lib2 = new NativeProgram("lib2");
            lib2.PublicDefines.Add(ExtraDefine);
            Lib.StaticLibraries.Add(lib2);
            CollectionAssert.Contains(ResultingDefinesFor("myfile.cpp"), ExtraDefine);
        }

        [Test]
        public void AlsoPropagatesToItself()
        {
            NativeProgram.PublicDefines.Add(ExtraDefine);
            CollectionAssert.Contains(ResultingDefinesFor("myfile.cpp"), ExtraDefine);
        }

        private string[] ResultingDefinesFor(string file)
        {
            var units = ObjectFileProductionInstructionses(NativeProgram);
            var defines = units.OfType<CLikeCompilationUnitInstruction>().Single(x => x.InputFile == file).CompilerSettings.Defines;
            return defines;
        }
    }
}
