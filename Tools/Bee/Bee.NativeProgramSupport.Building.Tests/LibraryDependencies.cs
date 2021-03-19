using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class LibraryDependencies : NativeProgramTestsBase
    {
        private class TestLibrary : PrecompiledLibrary
        {
            public override bool System => true; // avoid path usage
            public TestLibrary(string name) => _Name = name;
        }

        private class TestLinker : ObjectFileLinker
        {
            protected override NPath LinkerPath { get; }
            protected override string ActionName { get; } = string.Empty;

            public TestLinker(ToolChain toolchain) : base(toolchain) {}

            protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
                IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
            {
                throw new NotImplementedException("Should never reach this point");
            }
        }

        private class ResultCollector : NativeProgramFormat
        {
            private List<PrecompiledLibrary> NonToolchainSepecificLibraries { get; } = new List<PrecompiledLibrary>();
            private List<PrecompiledLibrary> LibraryDependencyList { get; } = new List<PrecompiledLibrary>();
            public string[] NonToolchainSepecificLibraryNames => NonToolchainSepecificLibraries.Select(x => x.Name).ToArray();
            public string[] LibraryDependencyNames => LibraryDependencyList.Select(x => x.Name).ToArray();

            public override string Extension => "test";

            public ResultCollector(ToolChain toolchain) : base(new TestLinker(toolchain)) {}

            protected override IEnumerable<NPath> CombineObjectFiles(NativeProgram nativeProgram, NativeProgramConfiguration config,
                IEnumerable<NPath> objectFiles, IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
                IEnumerable<PrecompiledLibrary> allLibraries, NPath linkTarget, NPath artifactsPath)
            {
                NonToolchainSepecificLibraries.AddRange(nonToolchainSpecificLibraries);
                LibraryDependencyList.AddRange(allLibraries);
                yield return linkTarget;
            }
        }

        private ToolChain Toolchain { get; } = new MockToolchain();
        private NativeProgramConfiguration Config => new NativeProgramConfiguration(CodeGen.Debug, Toolchain, false);
        private ResultCollector Result { get; set; }

        public override void SetUp()
        {
            base.SetUp();
            Result = new ResultCollector(Toolchain);
        }

        [Test]
        public void EstablishBaseline_DefaultListIsEmpty()
        {
            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.IsEmpty(Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibraryIsPropagatedToNativeProgramFormat()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            NativeProgram.StaticLibraries.Add(staticLibrary1);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void PrebuiltLibraryIsPropagatedToNativeProgramFormat()
        {
            var prebuiltLibrary1 = new TestLibrary("prebuiltLibrary1");
            NativeProgram.PrebuiltLibraries.Add(prebuiltLibrary1);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                prebuiltLibrary1.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibraryOrderIsMaintained()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            var staticLibrary2 = new NativeProgram("staticLibrary2");
            NativeProgram.StaticLibraries.Add(staticLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary2);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
                staticLibrary2.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void PrebuiltLibraryOrderIsMaintained()
        {
            var prebuiltLibrary1 = new TestLibrary("prebuiltLibrary1");
            var prebuiltLibrary2 = new TestLibrary("prebuiltLibrary2");
            NativeProgram.PrebuiltLibraries.Add(prebuiltLibrary1);
            NativeProgram.PrebuiltLibraries.Add(prebuiltLibrary2);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                prebuiltLibrary1.Name,
                prebuiltLibrary2.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibraryWithStaticLibraryDependencyIsSortedTopFirst()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            var staticLibrary2 = new NativeProgram("staticLibrary2");
            staticLibrary1.StaticLibraries.Add(staticLibrary2);
            NativeProgram.StaticLibraries.Add(staticLibrary1);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
                staticLibrary2.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibraryWithPrebuiltLibraryDependencyIsSortedTopFirst()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            var prebuiltLibrary1 = new TestLibrary("prebuiltLibrary1");
            staticLibrary1.PrebuiltLibraries.Add(prebuiltLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary1);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
                prebuiltLibrary1.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibraryAddedMoreThanOnceKeepsLastOccurrence()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            var staticLibrary2 = new NativeProgram("staticLibrary2");
            NativeProgram.StaticLibraries.Add(staticLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary2);
            NativeProgram.StaticLibraries.Add(staticLibrary1);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary2.Name,
                staticLibrary1.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void PrebuiltLibraryAddedMoreThanOnceKeepsOnlyLastOccurrence()
        {
            var prebuiltLibrary1 = new TestLibrary("prebuiltLibrary1");
            var prebuiltLibrary2 = new TestLibrary("prebuiltLibrary2");
            NativeProgram.PrebuiltLibraries.Add(prebuiltLibrary1);
            NativeProgram.PrebuiltLibraries.Add(prebuiltLibrary2);
            NativeProgram.PrebuiltLibraries.Add(prebuiltLibrary1);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                prebuiltLibrary2.Name,
                prebuiltLibrary1.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibraryWithStaticLibraryNeededByAnotherStaticLibraryKeepsOnlyLastOccurrence()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            var staticLibrary2 = new NativeProgram("staticLibrary2");
            var staticLibrary3 = new NativeProgram("staticLibrary3");
            staticLibrary1.StaticLibraries.Add(staticLibrary3);
            staticLibrary2.StaticLibraries.Add(staticLibrary3);
            NativeProgram.StaticLibraries.Add(staticLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary2);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
                staticLibrary2.Name,
                staticLibrary3.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibraryWithStaticLibraryNeededByAnotherStaticLibraryKeepsOrderOfPreference()
        {
            var libraryThatWantsToKeepFirstOccurence = new LibraryThatWantsToKeepFirstOccurence("mylib");

            var staticLibrary1 = new NativeProgram("staticLibrary1") {PrebuiltLibraries = {libraryThatWantsToKeepFirstOccurence}};
            var staticLibrary2 = new NativeProgram("staticLibrary2") {PrebuiltLibraries = {libraryThatWantsToKeepFirstOccurence}};

            NativeProgram.StaticLibraries.Add(staticLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary2);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
                libraryThatWantsToKeepFirstOccurence.Name,
                staticLibrary2.Name,
            },
                Result.LibraryDependencyNames);
        }

        public class LibraryThatWantsToKeepFirstOccurence : PrecompiledLibrary
        {
            public LibraryThatWantsToKeepFirstOccurence(NPath lib)
            {
                _Path = lib;
            }

            public override WhenMultiplePolicy WhenMultiplePolicy => WhenMultiplePolicy.KeepFirst;
        }


        [Test]
        public void StaticLibraryWithPrebuiltLibraryNeededByAnotherStaticLibraryKeepsOnlyLastOccurrence()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            var staticLibrary2 = new NativeProgram("staticLibrary2");
            var prebuiltLibrary1 = new TestLibrary("prebuiltLibrary1");
            staticLibrary1.PrebuiltLibraries.Add(prebuiltLibrary1);
            staticLibrary2.PrebuiltLibraries.Add(prebuiltLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary2);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
                staticLibrary2.Name,
                prebuiltLibrary1.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void StaticLibrariesAreSortedBeforePrebuiltLibraries()
        {
            var staticLibrary1 = new NativeProgram("staticLibrary1");
            var prebuiltLibrary1 = new TestLibrary("prebuiltLibrary1");
            NativeProgram.PrebuiltLibraries.Add(prebuiltLibrary1);
            NativeProgram.StaticLibraries.Add(staticLibrary1);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                staticLibrary1.Name,
                prebuiltLibrary1.Name,
            },
                Result.LibraryDependencyNames);
        }

        private class ToolchainSpecificLibrary : PrecompiledLibrary
        {
            public override bool System => true;
            public override bool ToolchainSpecific => true;
            public ToolchainSpecificLibrary(string name) => _Name = name;
        }

        [Test]
        public void ToolchainSpecificLibrariesAreIncludedInAllLibrariesCollection()
        {
            var toolchainSpecificLibrary = new ToolchainSpecificLibrary("toolchainSpecificLibrary");
            NativeProgram.PrebuiltLibraries.Add(toolchainSpecificLibrary);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                toolchainSpecificLibrary.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void ToolchainSpecificLibrariesAreExcludedFromNonToolchainSpecificLibrariesCollection()
        {
            var toolchainSpecificLibrary = new ToolchainSpecificLibrary("toolchainSpecificLibrary");
            NativeProgram.PrebuiltLibraries.Add(toolchainSpecificLibrary);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.IsEmpty(Result.NonToolchainSepecificLibraryNames);
        }

        private class NonToolchainSpecificLibrary : PrecompiledLibrary
        {
            public override bool System => true;
            public override bool ToolchainSpecific => false;
            public NonToolchainSpecificLibrary(string name) => _Name = name;
        }

        [Test]
        public void NonToolchainSpecificLibrariesAreIncludedInAllLibrariesCollection()
        {
            var nonToolchainSpecificLibrary = new NonToolchainSpecificLibrary("nonToolchainSpecificLibrary");
            NativeProgram.PrebuiltLibraries.Add(nonToolchainSpecificLibrary);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                nonToolchainSpecificLibrary.Name,
            },
                Result.LibraryDependencyNames);
        }

        [Test]
        public void NonToolchainSpecificLibrariesAreIncludedInNonToolchainSpecificLibrariesCollection()
        {
            var nonToolchainSpecificLibrary = new NonToolchainSpecificLibrary("nonToolchainSpecificLibrary");
            NativeProgram.PrebuiltLibraries.Add(nonToolchainSpecificLibrary);

            NativeProgram.SetupSpecificConfiguration(Config, Result);
            Assert.AreEqual(new[]
            {
                nonToolchainSpecificLibrary.Name,
            },
                Result.NonToolchainSepecificLibraryNames);
        }

        private class NonSystemLibraryWithoutPath : PrecompiledLibrary
        {
            public NonSystemLibraryWithoutPath(string name) => _Name = name;
        }

        [Test]
        public void ExpectExceptionWhenANonSystemLibraryDoesNotHavePathSet()
        {
            var nonSystemLibraryWithoutPath = new NonSystemLibraryWithoutPath("nonSystemLibraryWithoutPath");
            NativeProgram.PrebuiltLibraries.Add(nonSystemLibraryWithoutPath);
            Assert.Throws<InvalidOperationException>(() => NativeProgram.SetupSpecificConfiguration(Config, Result));
        }

        class PrecompiledLibrarySubclass : PrecompiledLibrary
        {
        }

        [Test]
        public void PrecompiledLibrarySubclassesAreToolchainSpecificByDefault()
        {
            var precompiledLibrarySubclass = new PrecompiledLibrarySubclass();
            Assert.True(precompiledLibrarySubclass.ToolchainSpecific);
        }

        [Test]
        public void SystemLibraryThrowsInvalidOperationExceptionIfPathIsAccessed()
        {
            var systemLibrary = new SystemLibrary("systemLibrary");
            Assert.Throws<InvalidOperationException>(() => { Console.WriteLine(systemLibrary.Path); });
        }
    }
}
