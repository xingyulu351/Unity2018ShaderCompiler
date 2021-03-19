using System.Linq;
using System.Linq.Expressions;
using Bee.Core;
using Bee.Core.Tests;
using Bee.CSharpSupport.IntegrationTests;
using Bee.DotNet;
using Mono.Cecil;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

class BackendTestsRequiringCSharp : CSharpIntegrationTestBase
{
    [TestCaseSource(nameof(FastestCompiler_AllBackends))]
    public void ResponseFilesWork(CSharpCompiler testableCSharpCompiler, BeeAPITestBackend<CSharpCompiler> testBackend)
    {
        ManualFixtureSetup(testableCSharpCompiler, testBackend);
        MainCsFile.WriteAllText(@"
class A
{
    static void Main(string[] args)
    {
        if (args[0][0] != '@')
            throw new System.ArgumentException(""First argument does not start with a @, looks like we're not being invoked with response file syntax"");
        var rspContent = System.IO.File.ReadAllText(args[0].Substring(1));
        System.IO.File.WriteAllText(""outputfile"",rspContent);
    }
}");
        Build(compiler =>
        {
            SetupCSharpProgram(compiler, cp => {});

            var dotNetAssembly = new DotNetAssembly(MyProgramExe, Framework.Framework46);
            var program = new DotNetRunnableProgram(dotNetAssembly);

            var manyStrings = MakeManyStrings();

            Backend.Current.AddAction(
                "InvokeProgram",
                inputs: new[] {dotNetAssembly.Path },
                targetFiles: new NPath[] {"outputfile"},
                executableStringFor: program.InvocationString,
                commandLineArguments: manyStrings,
                supportResponseFile: true
            );
        });
        var outputFileContents = TestRoot.Combine("outputfile").ReadAllText();
        Assert.AreEqual(MakeManyStrings().SeparateWithSpace(), outputFileContents);
    }

    private static string[] MakeManyStrings()
    {
        return Enumerable.Range(1, 100).Select(i => $"Hallo{i}").ToArray();
    }
}
