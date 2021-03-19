using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Bee.Core;
using Bee.NativeProgramSupport;
using Bee.Toolchain.GNU;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using static Bee.NativeProgramSupport.NativeProgramFormatUtils;
using System.Linq;

namespace Bee.Toolchain.Linux
{
    public abstract class LinuxToolchain : ToolChain
    {
        public override string LegacyPlatformIdentifier => $"linux{Architecture.Bits}";

        public abstract LdDynamicLinker Linker { get; }

        public override NativeProgramFormat DynamicLibraryFormat => new LinuxDynamicLibraryFormat(Linker);
        public override NativeProgramFormat ExecutableFormat => new LinuxExecutableFormat(Linker, Linker.Toolchain.Architecture);

        // WSL needs to be explicitly enable to not cause issues for windows users by default :)
        public LinuxToolchain(Sdk sdk, IntelArchitecture architecture, bool enableWSLSupport = false)
            : base(WrapIfNeeded(sdk, enableWSLSupport), architecture)
        {
        }

        protected override IEnumerable<ObjectFileProducer> ProvideObjectFileProducers()
        {
            foreach (var s in base.ProvideObjectFileProducers())
                yield return s;
            // Don't run NASM on WSL since in most cases the linux binary is not available
            if (Sdk is WSLSdk)
                yield break;
            yield return new NasmAssembler(this);
            yield return new YasmAssembler(this);
        }

        private static Sdk WrapIfNeeded(Sdk sdk, bool enableWSLSupport) =>
            HostPlatform.IsWindows && enableWSLSupport && sdk != null &&
            !BuildEnvironment.IsRunningOnBuildMachine() // disable WSL when running headless
            ? new WSLSdk(sdk) : sdk;
    }

    public class LinuxExecutableFormat : NativeProgramFormat<LdDynamicLinker>
    {
        public override string Extension { get; } = string.Empty;

        public LinuxExecutableFormat(LdDynamicLinker linker, Architecture arch) : base(
            linker
                .AsPositionIndependentExecutable(arch is x64Architecture)
                .WithStaticCppRuntime(false)
                .WithDynamicLibraryLinkPath(LinuxSysrootHelper.LibrarySearchPathsFor(linker.Toolchain.Sdk).ToArray()))
        {
        }
    }

    public class LinuxDynamicLibraryFormat : NativeProgramFormat<LdDynamicLinker>
    {
        public override string Extension { get; } = "so";

        public LinuxDynamicLibraryFormat(LdDynamicLinker linker) : base(
            linker
                .AsDynamicLibrary()
                .WithDynamicLibraryLinkPath(LinuxSysrootHelper.LibrarySearchPathsFor(linker.Toolchain.Sdk).ToArray()))
        {
        }
    }
}
