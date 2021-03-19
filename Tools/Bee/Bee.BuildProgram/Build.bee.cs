using System;
using System.Linq;
using System.Text;
using Bee.BuildProgram;
using Bee.Core;
using Bee.DotNet;
using Bee.Stevedore;
using Bee.Stevedore.Program;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

class BeeBootstrapProgram : BuildProgramForBee
{
    protected override string NewtonSoftJsonDll { get; } = "External/Newtonsoft/Newtonsoft.Json.dll";
    protected override NPath Moq { get; } = "External/Moq/Moq.dll";
    protected override string NUnitFramework { get; } = "External/NUnit/nunit.framework.dll";
    protected override NPath BeeRoot { get; } = new NPath(".");

    protected override NPath CecilPath { get; } = "External/Mono.Cecil/Unity.Cecil.dll";
    protected override NPath NiceIOPath { get; } = "External/NiceIO";

    StevedoreArtifact ILRepackArtifact { get; }
    StevedoreArtifact NUnitRunnerArtifact { get; }
    StevedoreArtifact ReportUnitArtifact { get; }

    protected override NPath ILRepackExePath => ILRepackArtifact.Path.Combine("tools/ILRepack.exe");


    public BeeBootstrapProgram()
    {
        ILRepackArtifact =
            StevedoreArtifact.Public(
                "ILRepack/2.0.15_c935abf346acc153f26f0cde7f581aab3135992ef9cb2cdd9096b063107e1bb9.zip");
        Backend.Current.Register(ILRepackArtifact);

        NUnitRunnerArtifact = StevedoreArtifact.Public("nunit-consolerunner/3.8.0_f3dfe17c1d1411b00fe00c9d0d23d745002f60c9ec9f68aff288dd0dddc757bf.zip");
        Backend.Current.Register(NUnitRunnerArtifact);

        ReportUnitArtifact = StevedoreArtifact.Public("reportunit/1.2.1_0feed9be804ba85fc4962c86b80fefc8827d0149cb6858046c2960e9abbcb79b.zip");
        Backend.Current.Register(ReportUnitArtifact);
    }

    private DotNetRunnableProgram NUnitRunnerProgram => new DotNetRunnableProgram(new DotNetAssembly(NUnitRunnerArtifact.Path.Combine("tools/nunit3-console.exe"), Framework.Framework46));
    private DotNetRunnableProgram ReportUnitProgram => new DotNetRunnableProgram(new DotNetAssembly(ReportUnitArtifact.Path.Combine("tools/reportunit.exe"), Framework.Framework46));

    protected override string TundraPathFor(string platform)
    {
        switch (platform)
        {
            case "MacOS":
                return "External/tundra/darwin/tundra2";
            case "Linux":
                return "External/tundra/linux/tundra2";
            case "Windows":
                return "External/tundra/MSWin32/tundra2.exe";
            default:
                throw new ArgumentException();
        }
    }

    public override void SetupBuildProgramBuildProgram()
    {
        base.SetupBuildProgramBuildProgram();

        var testAssemblies = CSharpPrograms.Where(p => p.Path.FileName.Contains("Tests")).Select(c => c.Path.InQuotes(SlashMode.Native));

        var runTestScript = new StringBuilder();
        NPath testresultxml = "artifacts/testresults.xml";
        runTestScript.AppendLine($"{NUnitRunnerProgram.InvocationString} {testAssemblies.SeparateWithSpace()} --result={testresultxml}");
        runTestScript.AppendLine($"{ReportUnitProgram.InvocationString} {testresultxml} {testresultxml.ChangeExtension("html")}");

        Backend.Current.AddWriteTextAction("artifacts/runtests", runTestScript.ToString());

        var solutionFileBuilder = new SolutionFileBuilder().WithPath("Bee.gen.sln");
        foreach (var csharpProgram in CSharpPrograms)
            solutionFileBuilder.WithProgram(SolutionFolderFor(csharpProgram), csharpProgram);
        solutionFileBuilder.Complete();
    }

    static void Main()
    {
        new BeeBootstrapProgram().SetupBuildProgramBuildProgram();
    }

    private static string SolutionFolderFor(CSharpProgram csharpProgram)
    {
        return IsBeeToolchain(csharpProgram) ? "Toolchains" : null;
    }
}
