using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.VisualStudio
{
    internal class ResourceCompileInstructions : ObjectFileProductionInstructions
    {
        ResourceCompiler ResourceCompiler { get; }
        public NPath[] IncludeDirs { get; }

        public ResourceCompileInstructions(NPath inputFile, ResourceCompiler resourceCompiler, NPath[] includeDirs) : base(inputFile, resourceCompiler)
        {
            ResourceCompiler = resourceCompiler;
            IncludeDirs = includeDirs;
        }

        public override void TellJamHowToProduce(NPath objectFile)
        {
            ResourceCompiler.SetupInvocation(objectFile, this);
        }
    }
}
