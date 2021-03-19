using Bee.Core;
using Bee.CSharpSupport;
using Bee.DotNet;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Tools.TypeForwarderGenerator
{
    class TypeForwarderGeneratorTool : CSharpTool
    {
        protected override DotNetAssembly DotNetAssembly()
        {
            var program = new CSharpProgram
            {
                Path = "artifacts/BuildTools/TypeForwarderGenerator/TypeForwarderGenerator.exe",
                PrebuiltReferences = { Cecil.Current35WithSymbolRefs },
                Sources =
                {
                    "Tools/TypeForwarderGenerator/Program.cs",
                    "External/Mono.Options/Options.cs"
                }
            };

            var tests = new CSharpProgram
            {
                Path = "artifacts/BuildTools/TypeForwarderGenerator/TypeForwarderGenerator.Tests.dll",
                References = { program },
                PrebuiltReferences =
                {
                    "External/NUnit/framework/nunit.framework.dll",
                    Cecil.Current35WithSymbolRefs
                },
                Sources =
                {
                    "Tools/TypeForwarderGenerator/Tests/TypeForwarderGeneratorTests.cs",
                },
                CsprojReferences =
                {
                    new CSharpProjectFileReference("Tools/TypeForwarderGenerator/Tests/TestDLL/TestDLL.csproj"),
                }
            };

            ProjectFilesTarget.DependOn(
                new SolutionFileBuilder()
                    .WithProgram(program, tests)
                    .WithProjectFile("Tests", new CSharpProjectFileReference(
                        $"Tools/TypeForwarderGenerator/Tests/TestDLL/TestDLL.csproj"))
                    .Complete().Path);

            return program.SetupDefault();
        }

        public JamTarget SetupInvocation(JamTarget outputFile, NPath[] inputAssemblies)
        {
            return SetupInvocation(outputFile, (JamList)inputAssemblies);
        }

        public JamTarget SetupInvocation(JamTarget outputFile, JamList inputAssemblies)
        {
            base.SetupInvocation(
                outputFile,
                inputAssemblies,
                new[] {$"{inputAssemblies.GetBoundPath().InQuotes().SeparateWithSpace()}", $"--output={outputFile.GetBoundPath().InQuotes()}"});

            return outputFile;
        }

        static TypeForwarderGeneratorTool s_Instance;
        public static TypeForwarderGeneratorTool Instance() => s_Instance ?? (s_Instance = new TypeForwarderGeneratorTool());
        protected override bool SupportsResponseFile => true;
    }
}
