using System;
using System.Collections.Generic;
using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.UWP
{
    public class UWPDynamicLinker : MsvcDynamicLinker
    {
        public UWPDynamicLinker(UWPToolchain toolchain) : base(toolchain)
        {
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var flag in base.CommandLineFlagsFor(destination, codegen, objectFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return flag;

            yield return "/APPCONTAINER";
            yield return "/NODEFAULTLIB:ole32.lib";
            yield return "/NODEFAULTLIB:kernel32.lib";
            yield return "WindowsApp.lib";
        }
    }
}
