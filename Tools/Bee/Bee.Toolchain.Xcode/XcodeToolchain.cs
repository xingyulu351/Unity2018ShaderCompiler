using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Xcode
{
    public enum StdLib
    {
        libcpp,
        libstdcpp
    }

    public sealed class SystemFramework : PrecompiledLibrary
    {
        public override bool System => true;
        public SystemFramework(string name) => _Name = name;
        public static implicit operator SystemFramework(string name) => new SystemFramework(name);
    }

    public sealed class NonSystemFramework : PrecompiledLibrary
    {
        public NonSystemFramework(NPath path) => _Path = path;
    }

    public abstract class XcodeSdk : Sdk
    {
        protected NPath XcodePath { get; }
        public override NPath Path { get; }
        public override Version Version { get; }
        public virtual NPath BinPath => $"{XcodePath}/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin";

        public override bool SupportedOnHostPlatform => HostPlatform.IsOSX;

        protected XcodeSdk(NPath sdkPath, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(backendRegistrables)
        {
            Path = sdkPath;
            Version = version;
            XcodePath = xcodePath ?? "/Applications/Xcode.app";
            IsDownloadable = isDownloadable;
        }

        public override NPath SysRoot => Path;

        public override NPath CppCompilerPath => BinPath.Combine("clang++");
        public override NPath StaticLinkerPath => BinPath.Combine("libtool");
        public override NPath DynamicLinkerPath => BinPath.Combine("clang++");
    }

    public abstract class XcodeToolchain : ToolChain
    {
        public string MinOSVersion { get; }

        internal StdLib StandardLibrary { get; }

        public override CLikeCompiler CppCompiler => new XcodeClangCompiler(this);
        public override NativeProgramFormat DynamicLibraryFormat => new XcodeDynamicLibraryFormat(this);
        public override NativeProgramFormat ExecutableFormat => new XcodeExecutableFormat(this);
        public override NativeProgramFormat StaticLibraryFormat => new XcodeStaticLibraryFormat(this);

        public override bool EnablingExceptionsRequiresRTTI => true;

        // todo move minSOVersion and stdlib to be settings
        protected XcodeToolchain(XcodeSdk sdk, Architecture architecture, string minOSVersion, StdLib stdlib = StdLib.libcpp)
            : base(sdk, architecture)
        {
            MinOSVersion = minOSVersion;
            StandardLibrary = stdlib;
        }
    }

    public class XcodeExecutableFormat : NativeProgramFormat<XcodeDynamicLinker>
    {
        public override string Extension { get; } = string.Empty;

        protected internal XcodeExecutableFormat(XcodeToolchain toolchain) : base(
            new XcodeDynamicLinker(toolchain))
        {
        }
    }

    public class XcodeDynamicLibraryFormat : NativeProgramFormat<XcodeDynamicLinker>
    {
        public override string Extension { get; } = "dylib";

        protected internal XcodeDynamicLibraryFormat(XcodeToolchain toolchain) : base(
            new XcodeDynamicLinker(toolchain).AsDynamicLibrary())
        {
        }
    }
}
