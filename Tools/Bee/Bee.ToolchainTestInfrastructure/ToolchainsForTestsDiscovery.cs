using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public static class ToolchainsForTestsDiscovery
    {
        private static readonly Lazy<ToolChain[]> _allToolchains = new Lazy<ToolChain[]>(() =>
        {
            var providers =  DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<TestableToolchainProvider>("Bee.Toolchain.*.dll").ToArray();
            return providers.SelectMany(p => p.Provide()).ToArray();
        });

        public static IEnumerable<ToolChain> All => _allToolchains.Value;
    }
}
