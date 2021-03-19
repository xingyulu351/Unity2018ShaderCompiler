using System;
using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport
{
    public enum ObjectFormatType
    {
        Mach,
        Elf,
        Win,
    }

    public abstract class AssemblyCompilerBase : AssemblerBase
    {
        protected AssemblyCompilerBase(ToolChain toolchain)
            : base(toolchain) {}

        public abstract ObjectFormatType ObjectFormat { get; }

        protected override IEnumerable<string> CommandLineArgumentsFor(NPath objectFile, AssemblerObjectFileProductionInstructions instructions)
        {
            if (Toolchain.Architecture is x64Architecture)
                yield return "-DBUILD_X64";

            if (ObjectFormat == ObjectFormatType.Elf)
            {
                if (Toolchain.Architecture is x64Architecture)
                    yield return "-f elf64";
                else
                    yield return "-f elf32";
            }
            else if (ObjectFormat == ObjectFormatType.Mach)
            {
                if (Toolchain.Architecture is x64Architecture)
                    yield return "-f macho64";
                else
                    yield return "-f macho32";
            }
            else if (ObjectFormat == ObjectFormatType.Win)
            {
                if (Toolchain.Architecture is x64Architecture)
                    yield return "-f win64";
                else
                    yield return "-f win32";
            }
            else
            {
                throw new NotImplementedException();
            }
        }
    }
}
