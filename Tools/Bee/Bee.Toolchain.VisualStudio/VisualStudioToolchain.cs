using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.NativeProgramSupport;
using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;
using Unity.BuildTools;

namespace Unity.BuildSystem.VisualStudio
{
    public abstract class VisualStudioToolchain : ToolChain
    {
        protected VisualStudioToolchain(VisualStudioSdk sdk) : base(sdk, sdk.Architecture)
        {
            StaticLibraryFormat = new MsvcStaticLibraryFormat(this);
        }

        protected override IEnumerable<ObjectFileProducer> ProvideObjectFileProducers()
        {
            foreach (var p in base.ProvideObjectFileProducers())
                yield return p;
            if (Sdk is VisualStudioSdk sdk)
                yield return new ResourceCompiler(sdk);
            yield return new YasmAssembler(this);
            yield return new NasmAssembler(this);
        }

        public override NativeProgramFormat StaticLibraryFormat { get; }
    }
}
