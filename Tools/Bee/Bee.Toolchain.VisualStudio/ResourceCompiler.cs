using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;

namespace Bee.Toolchain.VisualStudio
{
    class ResourceCompiler : ObjectFileProducer
    {
        public override string[] SupportedExtensions { get; } = {"rc"};
        public override string ObjectExtension { get; } = "res";

        private VisualStudioSdk Sdk { get; }

        public ResourceCompiler(VisualStudioSdk sdk) => Sdk = sdk;

        public override ObjectFileProductionInstructions ObjectProduceInstructionsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config, NPath inputFile, NPath[] allIncludes, string[] allDefines,
            NPath artifactPath)
        {
            var includeDirs = allIncludes
                .Concat(nativeProgram.PerFileIncludeDirectories.For(inputFile, config)).Distinct().ToArray();
            return new ResourceCompileInstructions(inputFile, this, includeDirs);
        }

        public void SetupInvocation(NPath objectFile, ResourceCompileInstructions resourceCompileInstructions)
        {
            Backend.Current.AddAction(
                "VisualStudio_ResourceCompiler",
                new[] {objectFile},
                new[] {resourceCompileInstructions.InputFile},
                Sdk.ToolPath("rc.exe").InQuotes(SlashMode.Native),
                CommandLineInfluencing(objectFile, resourceCompileInstructions).ToArray()
            );
        }

        IEnumerable<string> CommandLineInfluencing(NPath objectFile, ResourceCompileInstructions resourceCompileInstructions)
        {
            yield return "/nologo";
            yield return "/FO";
            yield return objectFile.InQuotes();
            foreach (var includeDir in Sdk.IncludePaths.Concat(resourceCompileInstructions.IncludeDirs))
                yield return $"/I{includeDir.InQuotes()}";
            yield return resourceCompileInstructions.InputFile.InQuotes();
        }
    }
}
