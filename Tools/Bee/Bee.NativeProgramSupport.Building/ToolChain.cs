using System;
using System.Collections.Generic;
using Bee.Core;
using System.Linq;
using Bee.NativeProgramSupport;
using Bee.NativeProgramSupport.Building;
using Bee.NativeProgramSupport.Building.FluentSyntaxHelpers;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class ToolChain : IBackendRegistrable
    {
        public static FluentPlatforms Store { get; } = new FluentPlatforms();
        public virtual string ActionName => Platform.DisplayName + "_" + Architecture.DisplayName;
        public Sdk Sdk { get; }
        public Architecture Architecture { get; }
        public abstract NativeProgramFormat ExecutableFormat { get; }
        public abstract NativeProgramFormat StaticLibraryFormat { get; }
        public abstract NativeProgramFormat DynamicLibraryFormat { get; }
        public abstract CLikeCompiler CppCompiler { get; }
        public Platform Platform => Sdk.Platform;

        protected virtual IEnumerable<ObjectFileProducer> ProvideObjectFileProducers() => new[] {CppCompiler};
        public virtual bool CanBuild => Sdk.SupportedOnHostPlatform && Sdk.Path != null && (Sdk.Path.Exists() || Sdk.IsDownloadable);
        public bool CanRun => ExecutableFormat.CanRun;
        public Func<ToolChain> CreatingFunc { get; set; }

        ObjectFileProducer[] _objectFileProducers;
        public ObjectFileProducer[] ObjectFileProducers => _objectFileProducers ?? (_objectFileProducers = ProvideObjectFileProducers().ToArray());

        public virtual bool EnablingExceptionsRequiresRTTI => false;

        public virtual string LegacyPlatformIdentifier => $"{Platform.Name.ToLower()}_{Architecture.Name}";

        protected ToolChain(Sdk sdk, Architecture architecture)
        {
            Sdk = sdk ?? throw new ArgumentNullException(nameof(sdk));
            Architecture = architecture ?? throw new ArgumentNullException(nameof(architecture));
        }

        [Obsolete("Deprecated, use ToolChain.Store.Host instead", true)]
        public static ToolChain BestForHostPlatform { get; }

        //a very rough guestimate on how fast this toolchain is, so that if test frameworks need to choose a toolchain for a test,
        //and they don't care which one, they can just take the fastest
        public virtual int SpeedScore => 0;

        public void Register(IBackend backend)
        {
            backend.Register(Sdk);
        }
    }
}
