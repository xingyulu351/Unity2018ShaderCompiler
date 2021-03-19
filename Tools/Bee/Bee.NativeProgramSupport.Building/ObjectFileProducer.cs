using System;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class ObjectFileProducer
    {
        public abstract string[] SupportedExtensions { get; }
        public abstract string ObjectExtension { get; }

        public abstract ObjectFileProductionInstructions ObjectProduceInstructionsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            NPath inputFile,
            NPath[] allIncludes,
            string[] allDefines,
            NPath artifactPath);

        public virtual ObjectFileProductionInstructions[] PostProcessObjectProductionInstructions(
            ObjectFileProductionInstructions[] inputInstructions,
            NPath artifactsPath,
            Action<NPath> reportAdditionalObjectFile)
            => inputInstructions;
    }
}
