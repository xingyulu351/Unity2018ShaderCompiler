using System.Collections.Generic;
using Bee.Toolchain.GNU;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Android
{
    public class AndroidNdkToolchain : ToolChain
    {
        public override CLikeCompiler CppCompiler { get; }
        public override NativeProgramFormat StaticLibraryFormat => new GNUStaticLibraryFormat(this);
        public override NativeProgramFormat DynamicLibraryFormat => new AndroidNdkDynamicLibraryFormat(this);
        public override NativeProgramFormat ExecutableFormat => new AndroidNdkExecutableFormat(this);

        public AndroidNdkToolchain(AndroidNdk ndk)
            : base(ndk, ndk.Architecture)
        {
            CppCompiler = new AndroidNdkCompiler(this, ndk.ApiLevel);
        }
    }

    public sealed class AndroidDynamicLinker : LdDynamicLinker
    {
        // workaround arm64 issue (https://issuetracker.google.com/issues/70838247)
        protected override string LdLinkerName => Toolchain.Architecture is Arm64Architecture ? "bfd" : "gold";

        public AndroidDynamicLinker(AndroidNdkToolchain toolchain) : base(toolchain, true) {}

        protected override IEnumerable<string> CommandLineFlagsFor(NPath target, CodeGen codegen, IEnumerable<NPath> inputFiles, IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var flag in base.CommandLineFlagsFor(target, codegen, inputFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return flag;

            yield return "-lgnustl_static"; // turn this into a stdlib setting

            if (LdLinkerName == "gold")
            {
                // enable identical code folding (saves ~500k (3%) of Android mono release library as of May 2018)
                yield return "-Wl,--icf=safe";

                // redo folding multiple times (default is 2, saves 13k of Android mono release library as of May 2018)
                yield return "-Wl,--icf-iterations=5";
            }

            // overwrite compiler's sysroot
            var ndk = (AndroidNdk)Toolchain.Sdk;
            yield return $"--sysroot={ndk.LinkerSysRoot.InQuotes()}";
        }
    }

    public sealed class AndroidNdkExecutableFormat : NativeProgramFormat<AndroidDynamicLinker>
    {
        public override string Extension { get; } = "";

        internal AndroidNdkExecutableFormat(AndroidNdkToolchain toolchain) : base(
            new AndroidDynamicLinker(toolchain).AsPositionIndependentExecutable(true))
        {
        }
    }

    public sealed class AndroidNdkDynamicLibraryFormat : NativeProgramFormat<AndroidDynamicLinker>
    {
        public override string Extension { get; } = "so";

        internal AndroidNdkDynamicLibraryFormat(AndroidNdkToolchain toolchain) : base(
            new AndroidDynamicLinker(toolchain).AsDynamicLibrary())
        {
        }
    }
}
