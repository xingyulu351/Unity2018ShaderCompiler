using System;
using System.Collections.Generic;
using Moq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    class MockToolchain : ToolChain
    {
        private readonly MockFormat _mockFormat;

        public MockToolchain() : base(new MockSdk(), new Arm64Architecture())
        {
            _mockFormat = new MockFormat(this);
        }

        public MockToolchain(Platform platform, Architecture architecture) : base(new MockSdk(platform: platform), architecture)
        {
            _mockFormat = new MockFormat(this);
        }

        public override NativeProgramFormat ExecutableFormat => _mockFormat;
        public override NativeProgramFormat StaticLibraryFormat => _mockFormat;
        public override NativeProgramFormat DynamicLibraryFormat => _mockFormat;
        public override CLikeCompiler CppCompiler => new MockCompiler(this);
    }

    class MockPlatform : Platform
    {
        public override bool HasPosix => false;
    }

    class MockSdk : Sdk
    {
        public override Platform Platform { get; }
        public override NPath Path { get; }
        public override Version Version { get; }
        public override NPath CppCompilerPath { get; }
        public override NPath StaticLinkerPath { get; }
        public override NPath DynamicLinkerPath { get; }
        public override bool SupportedOnHostPlatform { get; }

        public MockSdk(NPath path = null, Version version = null, bool isDownloadable = false, Platform platform = null)
        {
            Path = path ?? "/__path__";
            Version = version ?? new Version();
            IsDownloadable = isDownloadable;
            Platform = platform ?? new MockPlatform();
        }
    }

    abstract class MockSdkLocator<T> : SdkLocator<T> where T : Sdk
    {
        protected override Platform Platform { get; } = new MockPlatform();
        protected override IEnumerable<T> ScanForSystemInstalledSdks() => Array.Empty<T>();

        public override T UserDefault { get; } = null;
        public override T FromPath(NPath path) => NewSdkInstance(path, new Version(), false);
    }

    class MockSdkLocator : MockSdkLocator<MockSdk>
    {
        protected override MockSdk NewSdkInstance(NPath path, Version version, bool isDownloadable) => new MockSdk(path, version, isDownloadable);
    }

    class MockFormat : NativeProgramFormat
    {
        public MockFormat(ObjectFileLinker linker) : base(linker)
        {
        }

        public MockFormat(ToolChain toolChain) : base(new MockObjectLinker(toolChain))
        {
        }

        public override string Extension { get; } = "mock";
    }

    class MockObjectLinker : ObjectFileLinker
    {
        public MockObjectLinker(ToolChain toolchain) : base(toolchain)
        {
        }

        protected override NPath LinkerPath { get; }
        protected override string ActionName { get; }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            return Array.Empty<string>();
        }
    }

    class MockCompiler : CLikeCompiler
    {
        public MockCompiler(ToolChain toolchain) : base(toolchain)
        {
            DefaultSettings = new MockCompilerSettings(this);
        }

        public override string[] SupportedExtensions { get; } = {"c", "cpp", "m", "mm"};
        public override string ObjectExtension { get; } = "o";
        public override string WarningStringForUnusedVariable { get; }
    }

    class MockCompilerSettings : CLikeCompilerSettings
    {
        public MockCompilerSettings(CLikeCompiler compiler) : base(compiler)
        {
        }

        public override IEnumerable<string> CommandLineInfluencingCompilationFor(NPath outputObjectFile, NPath inputCppFile,
            NPath createOutputPchAt = null, NPath headerToCreateOutputPchFrom = null)
        {
            return Array.Empty<string>();
        }
    }
}
