using System;
using System.Linq;
using Bee.Core;
using Bee.Core.Tests;
using Bee.DotNet;
using NiceIO;
using NUnit.Framework;
using NUnit.Framework.Internal;
using Unity.BuildSystem.CSharpSupport;

namespace Bee.CSharpSupport.IntegrationTests
{
    [TestFixture]
    public class SimpleFunctionalityTests : CSharpIntegrationTestBase
    {
        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void CanBuildAndRun(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);
            MainCsFile.WriteAllText("class A { static void Main() { System.Console.WriteLine(\"Hello\"); } }");
            Build(compiler => { SetupCSharpProgram(compiler, cp => {}); });
            RunAndExpect("Hello");
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void Defines(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            MainCsFile.WriteAllText(@"
class A
{
 static void Main()
 {
#if THIS_WILL_BE_ON
    System.Console.WriteLine(""On"");
#endif
#if THIS_WILL_BE_OFF
    System.Console.WriteLine(""Off"");
#endif
 }
}
");
            Build(compiler => { SetupCSharpProgram(compiler, cp => { cp.Defines.Add("THIS_WILL_BE_ON"); }); });
            RunAndExpect("On");
        }

        [TestCaseSource(nameof(AllCompilers_FastestBackend))]
        public void CopyReferencesNextToTargetsCopiesTransitiveDependenciesToo(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);
            SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.PrepareTest(TestRoot);

            Build(compiler =>
            {
                SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.SetupBuildCode(compiler);
            });
            var managedProgram = TestRoot.Combine("artifacts/faraway/program.exe");
            RunAndExpect("1", managedProgram);

            var ext = testableCSharpCompiler is Csc ? "pdb" : "dll.mdb";
            var debugFileOfDependency = managedProgram.Parent.Combine($"dependency.{ext}");
            Assert.IsTrue(debugFileOfDependency.FileExists());
        }

        private static class SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2
        {
            public static readonly string DependencyCsContents = "public class Dependency { public static int InvokePrebuiltLib() { return PrebuiltLib.One(); } }";

            public static void PrepareTest(NPath testRoot)
            {
                testRoot.Combine("prebuilt_lib.cs").WriteAllText("public class PrebuiltLib { public static int One() => 1; }");
                testRoot.Combine("dependency.cs").WriteAllText(DependencyCsContents);
                testRoot.Combine("main.cs").WriteAllText("class A { static void Main() { System.Console.WriteLine(Dependency.InvokePrebuiltLib()); } }");
            }

            public static DotNetAssembly SetupBuildCode(
                CSharpCompiler compiler)
            {
                var prebuiltLibraryPath = "artifacts/prebuiltlibrary/prebuiltlibrary.dll";
                var prebuiltLibrary = new CSharpProgram()
                {
                    Path = prebuiltLibraryPath,
                    Sources = {"prebuilt_lib.cs"}
                };
                prebuiltLibrary.SetupSpecificConfiguration(new CSharpProgramConfiguration(CSharpCodeGen.Debug, compiler));

                var dependency = new CSharpProgram()
                {
                    Path = "artifacts/dependency/dependency.dll",
                    Sources = {"dependency.cs"},
                    PrebuiltReferences = {prebuiltLibraryPath},
                    CopyReferencesNextToTarget = false
                };
                return SetupCSharpProgram(compiler, cp =>
                {
                    cp.Path = "artifacts/faraway/program.exe";
                    cp.References.Add(dependency);
                    cp.CopyReferencesNextToTarget = true;
                });
            }
        }

        [Test, TestCaseSource(nameof(AllCompilers_FastestBackend))]
        public void CopyDotNetAssemblyCopiesReferencesToo(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);
            SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.PrepareTest(TestRoot);

            Build(compiler =>
            {
                var program = SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.SetupBuildCode(compiler);
                program.DeployTo("other_target_dir");
                //CopyTool.Instance().Setup("other_target_dir", program);
            }, new[] {"other_target_dir/program.exe"});
            RunAndExpect("1", TestRoot.Combine("other_target_dir/program.exe"));
        }

        [Test, TestCaseSource(nameof(AllCompilers_FastestBackend))]
        public void CopyTwoProgramsThatShareDependenciesIntoSameFolder(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);
            SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.PrepareTest(TestRoot);

            Build(compiler =>
            {
                var program = SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.SetupBuildCode(compiler);
                program.DeployTo("other_target_dir");

                var program2 = new CSharpProgram()
                {
                    Path = "artifacts/myotherprogram.exe",
                    Sources = {"main.cs"},
                    PrebuiltReferences = {program.RuntimeDependencies.Select(d => d.Path)},
                }.SetupSpecificConfiguration(new CSharpProgramConfiguration(CSharpCodeGen.Debug, compiler));

                program2.DeployTo("other_target_dir");
            });
        }

        [Test, TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void UsingReferenceAssembliesCausesNonAPIChangeToNotRebuildConsumers(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            if (testableCSharpCompiler is Mcs)
                Assert.Ignore("Mcs does not support reference assemblies");
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.PrepareTest(TestRoot);

            var dependencyCs = TestRoot.Combine("dependency.cs");

            //backdate so we don't have to sleep in the middle of this test.
            dependencyCs.SetLastWriteTimeUtc(dependencyCs.GetLastWriteTimeUtc() - TimeSpan.FromDays(1));

            Build(compiler =>
            {
                var program = SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.SetupBuildCode(compiler);
                program.DeployTo("other_target_dir");
            });

            dependencyCs.WriteAllText(SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.DependencyCsContents.Replace(".One();", ".One()+1;"));

            var buildOutput = Build(compiler =>
            {
                var program = SetupProgramThatReferencesSourceLibraryAndPrebuiltLibrary2.SetupBuildCode(compiler);
                program.DeployTo("other_target_dir");
            });
            var programExe = "artifacts/faraway/program.exe";

            //since we only changed the body of dependency.dll, dependency.dll itself should have been rebuilt,
            //but program.exe that references dependency.dll should _not_ have been rebuilt.
            StringAssert.DoesNotContain($"{testableCSharpCompiler.ActionName} {programExe}", buildOutput.StdOut);

            //but the final app should run just fine, using the "old" program.exe, the "new" dependency.dll, and should return 2,
            //the value that the updated dependency.dll should return.
            RunAndExpect("2", TestRoot.Combine(programExe));
        }
    }
}
