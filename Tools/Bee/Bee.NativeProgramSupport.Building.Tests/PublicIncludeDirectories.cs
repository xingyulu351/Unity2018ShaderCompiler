using System.Linq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class PublicIncludeDirectories : NativeProgramTestsBase
    {
        private static readonly NPath PublicIncludeDir = "mylibdir";
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
            Lib.PublicIncludeDirectories.Add(PublicIncludeDir);
            NativeProgram.StaticLibraries.Add(Lib);
            CollectionAssert.Contains(ResultingIncludesFor("myfile.cpp"), PublicIncludeDir);
        }

        [Test]
        public void OnStaticLibraryWithCustomizationCallback()
        {
            NativeProgram.StaticLibraries.Add(new FromSourceLibraryDependency { Library = Lib, ConfigurationCustomizationCallback = c => c.WithCodeGen(CodeGen.Release)});
            Lib.PublicIncludeDirectories.Add(c => c.CodeGen == CodeGen.Release, PublicIncludeDir);
            CollectionAssert.Contains(ResultingIncludesFor("myfile.cpp"), PublicIncludeDir);
        }

        [Test]
        public void PropagatesMultipleDependencies()
        {
            NativeProgram.StaticLibraries.Add(Lib);

            var lib2 = new NativeProgram("lib2");
            lib2.PublicIncludeDirectories.Add(PublicIncludeDir);
            Lib.StaticLibraries.Add(lib2);
            CollectionAssert.Contains(ResultingIncludesFor("myfile.cpp"), PublicIncludeDir);
        }

        [Test]
        public void AlsoPropagatesToItself()
        {
            NativeProgram.PublicIncludeDirectories.Add(PublicIncludeDir);
            CollectionAssert.Contains(ResultingIncludesFor("myfile.cpp"), PublicIncludeDir);
        }

        private NPath[] ResultingIncludesFor(string file)
        {
            var units = ObjectFileProductionInstructionses(NativeProgram);
            var includes = units.OfType<CLikeCompilationUnitInstruction>().Single(x => x.InputFile == file).CompilerSettings.IncludeDirectories;
            return includes;
        }
    }
}
