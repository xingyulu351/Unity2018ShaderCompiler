using System;
using System.Collections.Generic;
using System.Linq;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem
{
    public interface IToolChainEmulatingV1BuildCodeProvider
    {
        ToolChain Provide(string v1PlatformName);
    }

    public static class V1BuildCodeEmulation
    {
        private static readonly Type[] ProviderTypes;

        static Dictionary<string, ToolChain> _toolchains = new Dictionary<string, ToolChain>();

        static V1BuildCodeEmulation()
        {
            // Build code providers live in main assembly
            var asm = AppDomain.CurrentDomain.GetAssemblies().Where(a => a.FullName.StartsWith("Unity.BuildSystem,"));
            var types = asm.SelectMany(a => a.GetTypes());
            ProviderTypes = types.Where(
                t => typeof(IToolChainEmulatingV1BuildCodeProvider).IsAssignableFrom(
                    t) && !t.IsInterface).ToArray();
        }

        public static ToolChain CreateToolChainEmulatingV1BuildCode(string v1PlatformName)
        {
            if (_toolchains.ContainsKey(v1PlatformName))
                return _toolchains[v1PlatformName];

            // New style build code wants ability to base certain decisions on the platform they're being built for.
            // We pretend that the V2 buildcode is already working, and we provide it with a V2-style ToolChain
            // and UnityPlayerConfiguration object. These aren't actually used to build something for real,
            // they are used for new style modules to do things like "if (config.ToolChain.Platform is MacOSXPlatform)".
            // Therefore the toolchains we return here are not necesseraly functioning and perfect, but will return the
            // right values for architecture and platform.

            var results = ProviderTypes.Select(
                t => ((IToolChainEmulatingV1BuildCodeProvider)Activator.CreateInstance(t)).Provide(v1PlatformName));

            var toolChain = results.SingleOrDefault(result => result != null);

            if (toolChain == null)
                Errors.Exit($"No IToolChainEmulatingV1BuildCodeProvider found for {v1PlatformName}");

            _toolchains.Add(v1PlatformName, toolChain);
            return toolChain;
        }
    }
}
