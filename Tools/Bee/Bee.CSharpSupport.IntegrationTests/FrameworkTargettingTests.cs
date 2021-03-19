using System;
using Bee.Core.Tests;
using Bee.DotNet;
using JamSharp.Runtime;
using NUnit.Framework;
using Unity.BuildSystem.CSharpSupport;

namespace Bee.CSharpSupport.IntegrationTests
{
    [TestFixture]
    public class FrameworkTargettingTests : CSharpIntegrationTestBase
    {
        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        [Platform(Exclude = "Win")]
        public void UsingSomethingFromNonDefaultReferencedAssemblyFailsWhenTargetting46(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);
            if (testableCSharpCompiler is Csc && testBackend.Name == "Jam")
                Assert.Ignore("We have a bug where if csc is invoked with /noconfig and /nostdlib in the responsfile the flags dont work. jam backend is the only backend right now that uses responsefiles");

            WriteMainCsFileUsingSystemXmlLinq();
            var result = Build(
                crp => { SetupCSharpProgram(crp, cp => cp.Framework = Framework.Framework46); }, throwOnFailure: false);
            Assert.NotZero(result.ExitCode);
            AssertOutputContainsTypeDoesntExistError(result, "Linq");
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        [Platform(Exclude = "Win")] //on windows on many systems we do not have .net46 framework, only 4.0
        public void UsingSomethingFromNonDefaultReferencedAssemblWorksWhenExplicitelyReferencedWhenTargetting46(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            WriteMainCsFileUsingSystemXmlLinq();
            Build(compiler => { SetupCSharpProgram(compiler, cp =>
            {
                cp.Framework = Framework.Framework46;
                cp.SystemReferences.Add("System.Xml.Linq", "System.Xml");
            }); });
            RunAndExpect("XDocument");
        }

        private void AssertOutputContainsTypeDoesntExistError(Shell.ExecuteResult result, string missingType)
        {
            if (TestableCSharpCompiler is Csc)
                StringAssert.Contains("The type or namespace name '" + missingType + "' does not exist", result.StdOut);
            else if (TestableCSharpCompiler is Mcs)
            {
                //the mcs error message mistakenly has a backtick instead of a single quote
                StringAssert.Contains("The type or namespace name `" + missingType + "' does not exist", result.StdOut);
            }
            else
                throw new NotSupportedException($"Unknown compiler {this.TestableCSharpCompiler}");
        }

        private void WriteMainCsFileUsingSystemXmlLinq()
        {
            MainCsFile.WriteAllText(@"
class A
{
 static void Main()
 {
    System.Console.WriteLine(new System.Xml.Linq.XDocument().GetType().Name);
 }
}
");
        }

        [Platform(Include = "MacOsX", Reason = "Only on mac do we have reference assemblies for non46 frameworks")]
        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void TargettingFramework20AndUsingSomethingFrom46FailsToBuild(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            WriteMainCsFileWithField("public System.Threading.Tasks.Task t = null;");
            var result = Build(compiler => { SetupCSharpProgram(compiler, cp => cp.Framework = Framework.Framework20); }, throwOnFailure: false);
            Assert.NotZero(result.ExitCode);
            AssertOutputContainsTypeDoesntExistError(result, "Tasks");
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        [Platform(Include = "MacOsX", Reason = "Only on mac do we have reference assemblies for non46 frameworks")]
        public void TargettingFramework20ResultsInAssemblyReferencesToCorlibVersion20(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);
            if (testableCSharpCompiler is Csc && testBackend.Name == "Jam")
                Assert.Ignore("We have a bug where if csc is invoked with /noconfig and /nostdlib in the responsfile the flags dont work. jam backend is the only backend right now that uses responsefiles");

            WriteMainCsFileWithField("public object o = null;");
            Build(compiler => { SetupCSharpProgram(compiler, cp => cp.Framework = Framework.Framework20); });
            ValidateResultingAssembly(TestRoot.Combine(MyProgramExe), assemblyNameReference => Assert.AreEqual(new Version(2, 0, 0, 0), assemblyNameReference.Version));
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void TargettingFramework46ResultsInAssemblyReferencesToCorlibVersion46(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            WriteMainCsFileWithField("public object o = null;");
            Build(compiler => { SetupCSharpProgram(compiler, cp => cp.Framework = Framework.Framework46); });
            ValidateResultingAssembly(TestRoot.Combine(MyProgramExe), assemblyNameReference =>
            {
                //the assemblyversion of .net framework46 is 4.0.
                var expected = new Version(4, 0, 0, 0);

                Assert.AreEqual(expected, assemblyNameReference.Version);
            });
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void TargettingDotNetStandardResultsInOnlyReferencesToNetStandardDll(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            AssertIgnoreIfNoDotNetStandardReferenceAssembliesArePresent();
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            WriteMainCsFileWithField("public object o = null;");
            Build(compiler => { SetupCSharpProgram(compiler, cp => cp.Framework = Framework.NetStandard20); });
            ValidateResultingAssembly(TestRoot.Combine(MyProgramExe), assemblyNameReference => Assert.AreEqual("netstandard", assemblyNameReference.Name));
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void CanBuildAndRunNetStandard2(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            AssertIgnoreIfNoDotNetStandardReferenceAssembliesArePresent();
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            MainCsFile.WriteAllText("class A { static void Main() { System.Console.WriteLine(\"Hello\"); } }");
            Build(compiler => { SetupCSharpProgram(compiler, cp => { cp.Framework = Framework.NetStandard20; }); });
            RunAndExpect("Hello");
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void ConsumingNetStandard2LibraryIn46TargettingProgram(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            AssertIgnoreIfNoDotNetStandardReferenceAssembliesArePresent();
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            WriteProgramAndLibrary();
            Build(compiler =>
            {
                SetupCSharpProgram(compiler, cp => { SetupWithLibrary(cp, libraryFramework: Framework.NetStandard20, programFramework: Framework.Framework46); });
            });
            RunAndExpect("there");
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void ConsumingNetStandard13LibraryIn46TargettingProgram(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            AssertIgnoreIfNoDotNetStandardReferenceAssembliesArePresent();
            ManualFixtureSetup(testableCSharpCompiler, testBackend);

            WriteProgramAndLibrary();
            Build(compiler =>
            {
                SetupCSharpProgram(compiler, cp => { SetupWithLibrary(cp, libraryFramework: Framework.NetStandard13, programFramework: Framework.Framework46); });
            });
            RunAndExpect("there");
        }

        [TestCaseSource(nameof(AllCompilers_AllBackends))]
        public void Consuming46LibraryInNetStandard2TargettingProgram(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            AssertIgnoreIfNoDotNetStandardReferenceAssembliesArePresent();
            ManualFixtureSetup(testableCSharpCompiler, testBackend);
            WriteProgramAndLibrary();
            Build(compiler =>
            {
                SetupCSharpProgram(compiler, cp => { SetupWithLibrary(cp, libraryFramework: Framework.Framework46, programFramework: Framework.NetStandard20); });
            });
            RunAndExpect("there");
            ValidateResultingAssembly(TestRoot.Combine(MyProgramExe), assemblyNameReference => CollectionAssert.Contains(new[] { "lib", "netstandard"}, assemblyNameReference.Name));
            ValidateResultingAssembly(TestRoot.Combine(MyLibDll), assemblyNameReference => Assert.AreEqual("mscorlib", assemblyNameReference.Name));
        }

        private void WriteProgramAndLibrary()
        {
            TestRoot.Combine("lib.cs").WriteAllText("public class lib { public static string hello => \"there\"; }");
            MainCsFile.WriteAllText("class A { static void Main() { System.Console.WriteLine(lib.hello); } }");
        }

        private static void SetupWithLibrary(CSharpProgram cp, Framework libraryFramework, Framework programFramework)
        {
            cp.Framework = programFramework;
            cp.References.Add(new CSharpProgram()
            {
                Path = MyLibDll,
                Sources = {"lib.cs"},
                Framework = libraryFramework,
            });
        }

        private void WriteMainCsFileWithField(string fieldDefinition)
        {
            WriteMainCsFile(@"
public class A
{
    " + fieldDefinition + @"
    static void Main(){}
}
");
        }
    }
}
