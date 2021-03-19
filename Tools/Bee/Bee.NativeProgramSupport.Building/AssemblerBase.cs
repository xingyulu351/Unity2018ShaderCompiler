using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class AssemblerBase : ObjectFileProducer
    {
        protected virtual string ActionName => $"Asm_{Toolchain.ActionName}";

        protected AssemblerBase(ToolChain toolchain)
        {
            Toolchain = toolchain;
        }

        protected ToolChain Toolchain { get; }

        public override ObjectFileProductionInstructions ObjectProduceInstructionsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config, NPath inputFile, NPath[] allIncludes, string[] allDefines,
            NPath artifactPath)
        {
            return new AssemblerObjectFileProductionInstructions(inputFile, this);
        }

        public void SetupInvocation(NPath objectFile, AssemblerObjectFileProductionInstructions instructions)
        {
            Backend.Current.AddAction(
                ActionName,
                new[] {objectFile},
                new[] {instructions.InputFile},
                AssemblerExecutable,
                CommandLineArgumentsFor(objectFile, instructions).ToArray());
        }

        protected abstract string AssemblerExecutable { get; }

        protected abstract IEnumerable<string> CommandLineArgumentsFor(NPath objectFile, AssemblerObjectFileProductionInstructions instructions);
    }
}
