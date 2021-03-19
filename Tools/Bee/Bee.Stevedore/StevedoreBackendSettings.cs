using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.Stevedore.Program;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Stevedore
{
    /// <summary>
    /// Per-backend Stevedore settings. Any changes to these must be made before
    /// any StevedoreArtifact is registered with the backend.
    /// </summary>
    public class StevedoreBackendSettings
    {
        public static StevedoreArtifact DefaultSevenZipForPlatform { get; } = new StevedoreArtifact("public", new ArtifactId(
            HostPlatform.IsWindows ? "7za-win-x64/16.04-1.1.4_0d1e6ad367de3bb0c1a396856903c63e080a1e8fee52897cb5f5dee063832831.zip" :
            HostPlatform.IsOSX     ? "7za-mac-x64/16.04-1.1.4_d7d625b52bf0c3e1b4948da5cd015e7ae2047e1bd18dc7a7ef367f950d43990e.zip" :
            HostPlatform.IsLinux   ? "7za-linux-x64/16.04-1.1.4_8d29a598ad3fe6b07e7bca627772720b53acb30d5da55feeebb6c8e1af63effa.zip" :
            throw new InvalidOperationException()
            )) { ExtractUsingSevenZip = false };

        private static readonly Dictionary<IBackend, StevedoreBackendSettings> s_Settings = new Dictionary<IBackend, StevedoreBackendSettings>();

        public static StevedoreBackendSettings Get(IBackend backend)
        {
            lock (s_Settings)
            {
                if (!s_Settings.TryGetValue(backend, out var settings))
                    settings = s_Settings[backend] = new StevedoreBackendSettings(backend);
                return settings;
            }
        }

        /// <summary>
        /// Stevedore configuration (including user and environment config).
        /// </summary>
        public StevedoreConfig Config { get; } = new StevedoreConfig();

        /// <summary>
        /// Specifies the 7za executable to use for unpacking. If null, the
        /// DefaultSevenZipForPlatform artifact will be used.
        /// </summary>
        public NPath SevenZipPath { get; set; }

        private StevedoreBackendSettings(IBackend backend)
        {
            Config.ReadEnvironmentDefaults(reportParseError: StevedoreProgram.EmitWarning);
        }
    }
}
