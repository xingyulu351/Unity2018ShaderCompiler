using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using NiceIO;
using Bee.Toolchain.GNU;
using Bee.Toolchain.LLVM;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Lumin
{
    // TODO :: Migrate to using the 'gold' linker ASAP.
    public class LuminDynamicLinker : LdDynamicLinker
    {
        public LuminDynamicLinker(LuminToolchain toolchain) : base(toolchain, true)
        {
        }

        protected LuminSdk PlatformSdk => (LuminSdk)Toolchain.Sdk;

        protected override IEnumerable<string> CommandLineFlagsFor(NPath target, CodeGen codegen, IEnumerable<NPath> inputFiles, IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var flag in LuminLinkerFlags()) yield return flag;

            nonToolchainSpecificLibraries = nonToolchainSpecificLibraries
                .Concat(SystemLibraries()).Concat(StaticSystemLibraries());

            foreach (var flag in base.CommandLineFlagsFor(target, codegen, inputFiles, nonToolchainSpecificLibraries,
                allLibraries))
                yield return flag;
        }

        protected IEnumerable<string> LuminLinkerFlags(bool addPrefix = true)
        {
            yield return AdjustLinkerFlagToCompilerFrontend("-z nocopyreloc");
            yield return AdjustLinkerFlagToCompilerFrontend("-z relro");
            yield return AdjustLinkerFlagToCompilerFrontend("-z now");
            yield return AdjustLinkerFlagToCompilerFrontend("-z origin");
            yield return AdjustLinkerFlagToCompilerFrontend("--warn-shared-textrel");
            yield return AdjustLinkerFlagToCompilerFrontend("--enable-new-dtags");
            yield return AdjustLinkerFlagToCompilerFrontend("--no-as-needed");
            yield return AdjustLinkerFlagToCompilerFrontend("-rpath=$ORIGIN");
            yield return "-nodefaultlibs";
        }

        private IEnumerable<PrecompiledLibrary> StaticSystemLibraries()
        {
            yield return PlatformSdk.GccSupportLibrary;
        }

        private static IEnumerable<SystemLibrary> SystemLibraries()
        {
            yield return "c++";
            yield return "c";
            yield return "m";
            yield return "dl";
        }
    }
}
