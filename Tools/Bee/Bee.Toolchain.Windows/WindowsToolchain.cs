using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.VisualStudio;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;
using Unity.BuildTools;

namespace Bee.Toolchain.Windows
{
    public class WindowsToolChainForHostProvider : ToolChainForHostProvider
    {
        public override ToolChain Provide() => HostPlatform.IsWindows
        ? new WindowsToolchain(WindowsSdk.Locatorx64.UserDefaultOrDummy)
        : null;
    }

    public sealed class WindowsCompiler : MsvcCompiler
    {
        public WindowsCompiler(WindowsToolchain toolchain) : base(toolchain)
        {
            DefaultSettings = new MsvcCompilerSettings(this)
                .WithStaticStandardLibrary(true)
                .WithDefines(new[]
                {
                    "WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP",
                });
        }
    }

    public sealed class WindowsToolchain : VisualStudioToolchain
    {
        public override string ActionName => $"{Platform.DisplayName}{Architecture.Bits}";
        public override string LegacyPlatformIdentifier => $"win{Architecture.Bits}";
        public override NativeProgramFormat ExecutableFormat { get; }
        public override NativeProgramFormat DynamicLibraryFormat { get; }
        public override CLikeCompiler CppCompiler => new WindowsCompiler(this);

        public WindowsToolchain(VisualStudioSdk sdk) : base(sdk)
        {
            ExecutableFormat = new WindowsExecutableFormat(this);
            DynamicLibraryFormat = new WindowsDynamicLibraryFormat(this);
        }
    }

    public sealed class WindowsExecutableFormat : NativeProgramFormat<MsvcDynamicLinker>
    {
        public override string Extension { get; } = "exe";

        public WindowsExecutableFormat(WindowsToolchain toolchain) : base(
            new MsvcDynamicLinker(toolchain))
        {
        }
    }

    public sealed class WindowsDynamicLibraryFormat : NativeProgramFormat<MsvcDynamicLinker>
    {
        public override string Extension { get; } = "dll";

        public WindowsDynamicLibraryFormat(VisualStudioToolchain toolchain) : base(
            new MsvcDynamicLinker(toolchain).AsDynamicLibrary())
        {
        }
    }
}
