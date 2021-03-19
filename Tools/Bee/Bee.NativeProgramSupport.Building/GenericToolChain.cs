using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class GenericSdk : Sdk
    {
        public override Platform Platform { get; }
        public override NPath Path { get; }
        public override Version Version { get; }
        public override NPath CppCompilerPath { get; }
        public override NPath StaticLinkerPath { get; }
        public override NPath DynamicLinkerPath { get; }
        public override bool SupportedOnHostPlatform => false;

        public override IEnumerable<NPath> IncludePaths { get; }

        public GenericSdk(Platform platform, NPath path = null, IEnumerable<NPath> includePaths = null)
        {
            Platform = platform;
            Path = path;
            IncludePaths = includePaths ?? Array.Empty<NPath>();
        }
    }

    public class GenericToolChain : ToolChain
    {
        public override bool CanBuild => false;

        public override NativeProgramFormat ExecutableFormat { get; }
        public override NativeProgramFormat StaticLibraryFormat { get; }
        public override NativeProgramFormat DynamicLibraryFormat { get; }
        public override CLikeCompiler CppCompiler { get; }
        public override string LegacyPlatformIdentifier { get; }

        protected override IEnumerable<ObjectFileProducer> ProvideObjectFileProducers() => Array.Empty<ObjectFileProducer>();

        public GenericToolChain(Platform platform, Architecture arch, string legacyPlatformIdentifier, NPath path = null, IEnumerable<NPath> includePaths = null)
            : base(new GenericSdk(platform, path, includePaths), arch)
        {
            LegacyPlatformIdentifier = legacyPlatformIdentifier;
        }
    }
}
