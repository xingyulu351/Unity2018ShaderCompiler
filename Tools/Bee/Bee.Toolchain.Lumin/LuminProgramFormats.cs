using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Bee.Toolchain.GNU;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Lumin
{
    public class LuminExecutableFormat : NativeProgramFormat<LuminDynamicLinker>
    {
        public override string Extension { get; } = ".elf";

        internal LuminExecutableFormat(LuminToolchain toolchain)
            : base(new LuminDynamicLinker(toolchain).AsPositionIndependentExecutable(true))
        {
        }
    }

    public class LuminDynamicLibraryFormat : NativeProgramFormat<LuminDynamicLinker>
    {
        public override string Extension { get; } = ".so";

        internal LuminDynamicLibraryFormat(LuminToolchain toolchain)
            : base(new LuminDynamicLinker(toolchain).AsDynamicLibrary())
        {
        }
    }
}
