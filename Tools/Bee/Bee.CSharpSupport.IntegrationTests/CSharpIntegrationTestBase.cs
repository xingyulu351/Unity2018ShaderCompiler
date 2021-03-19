using System;
using System.Collections;
using System.Linq;
using Bee.Core;
using Bee.Core.Tests;
using Bee.DotNet;
using Bee.TundraBackend.Tests;
using JamSharp.Runtime;
using Mono.Cecil;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.CSharpSupport;

namespace Bee.CSharpSupport.IntegrationTests
{
    public class CSharpIntegrationTestBase
    {
        protected BeeAPITestBackend<CSharpCompiler> TestBackend { get; private set; }
        protected CSharpCompiler TestableCSharpCompiler { get; private set; }
        protected NPath TestRoot => TestBackend.TestRoot;
        protected NPath MainCsFile => TestRoot.Combine("main.cs");

        protected void ManualFixtureSetup(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
        {
            if (!testableCSharpCompiler.SupportedOnHostPlatform())
                Assert.Ignore("This compiler cannot run on this operating system");
            if (!testableCSharpCompiler.CanBuild())
                Assert.Ignore("This compiler is not present on this machine");

            TestBackend = testBackend;
            TestableCSharpCompiler = testableCSharpCompiler;
            testBackend.Start();
        }

        protected static IEnumerable AllCompilers_AllBackends()
        {
            var backends = BeeTestApiBackendsLocator.AllBeeApiTestBackends<CSharpCompiler>();
            foreach (var csharpCompiler in AllTestableCSharpCompilers.Value)
                foreach (var backend in backends.Where(b => b.Name != "StandaloneBee"))
                    yield return new TestCaseData(csharpCompiler, backend).SetName(
                        $"{{m}}_{csharpCompiler.ActionName}_{backend.Name}");
        }

        protected static IEnumerable FastestCompiler_AllBackends()
        {
            var backends = BeeTestApiBackendsLocator.AllBeeApiTestBackends<CSharpCompiler>();
            foreach (var backend in backends.Where(b => b.Name != "StandaloneBee"))
                yield return new TestCaseData(AllTestableCSharpCompilers.Value.First(), backend).SetName(
                    $"{{m}}_{backend.Name}");
        }

        protected static IEnumerable AllCompilers_FastestBackend()
        {
            foreach (var csharpCompiler in AllTestableCSharpCompilers.Value)
            {
                var beeApiTestBackendTundra = new BeeAPITestBackend_Tundra<CSharpCompiler>();
                yield return new TestCaseData(csharpCompiler, beeApiTestBackendTundra).SetName(
                    $"{{m}}_{csharpCompiler.ActionName}_{beeApiTestBackendTundra.Name}");
            }
        }

        protected static readonly Lazy<CSharpCompiler[]> AllTestableCSharpCompilers =
            new Lazy<CSharpCompiler[]>(
                () => DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<CSharpCompiler>("*.dll").ToArray()
            );

        protected static string MyProgramExe = "artifacts/myprogram.exe";
        protected static string MyLibDll = "artifacts/lib.dll";

        protected void AssertIgnoreIfNoDotNetStandardReferenceAssembliesArePresent()
        {
            //todo: actually try to find them
            Assert.Ignore("This test can only run if we have dotnet standard reference assemblies");
        }

        protected void WriteMainCsFile(string body) => MainCsFile.WriteAllText(body);

        protected Shell.ExecuteResult Build(Action<CSharpCompiler> buildCode, string[] requestedTargets = null, bool throwOnFailure = true)
        {
            return TestBackend.Build(TestableCSharpCompiler.StaticFunctionToCreateMe, buildCode, requestedTargets: requestedTargets, throwOnFailure: throwOnFailure);
        }

        protected void ValidateResultingAssembly(NPath assemblyToAnalyze,
            Action<AssemblyNameReference> nameReferenceAssertion)
        {
            var assembly = AssemblyDefinition.ReadAssembly(assemblyToAnalyze.ToString());
            var references = assembly.MainModule.AssemblyReferences;

            foreach (var reference in references)
                nameReferenceAssertion(reference);
        }

        protected void RunAndExpect(string expectedOutput, NPath managedProgram = null)
        {
            managedProgram = managedProgram ?? TestRoot.Combine(MyProgramExe);
            var dotNetAssembly = new DotNetAssembly(managedProgram, Framework.Framework46);
            var runtime = DotNetRuntime.FindFor(dotNetAssembly);

            Assert.AreEqual(expectedOutput, Shell.Execute(runtime.InvocationStringFor(dotNetAssembly)).StdOut);
        }

        protected static DotNetAssembly SetupCSharpProgram(CSharpCompiler csharpCompiler, Action<CSharpProgram> modify)
        {
            var cp = new CSharpProgram()
            {
                Path = MyProgramExe,
                Sources = {"main.cs"}
            };
            modify(cp);

            return cp.SetupSpecificConfiguration(new CSharpProgramConfiguration(CSharpCodeGen.Debug, csharpCompiler));
        }
    }
}
