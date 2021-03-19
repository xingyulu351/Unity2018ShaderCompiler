using System;
using System.Collections.Generic;
using System.Linq;
using Bee.NativeProgramSupport;
using Newtonsoft.Json.Linq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Lumin
{
    public class LuminSdkLocator : SdkLocator<LuminSdk>
    {
        protected override Platform Platform { get; } = new LuminPlatform();
        public override LuminSdk UserDefault => ScanForOverrideSdks().FirstOrDefault();

        protected override IEnumerable<LuminSdk> ScanForSystemInstalledSdks() =>
            ScanForOverrideSdks()
                .Concat(ScanForSDKDownloaderSdks())
                .Concat(ScanForInstalledSdks());

        static LuminSdkLocator()
        {
            TryReadCachedSDKDownloaderValues();
        }

        private static readonly IEnumerable<string> _EnvironmentVariables =
            new[]
        {
            "LUMINSDK_UNITY",
            "MLSDK_UNITY",
            "LUMINSDK",
            "MLSDK",
        };

        private static NPath _CachedSDKDownloaderValues => new NPath($"{Paths.ProjectRoot}/artifacts/generated/ml-sdk");

        private IEnumerable<NPath> GetPossibleEnvironmentLocations()
        {
            foreach (var env in _EnvironmentVariables)
            {
                var value = Environment.GetEnvironmentVariable(env);
                if (!string.IsNullOrEmpty(value)) yield return value;
            }
        }

        public override LuminSdk FromPath(NPath path)
        {
            if (path == null) return null;

            var manifest = path.Combine(".metadata").Combine("sdk.manifest");
            if (!manifest.FileExists()) return null;

            var json = JObject.Parse(manifest.ReadAllText());
            Version.TryParse(json["version"].ToString(), out var version);
            if (version == null) return null;

            return new LuminSdk(path, version, false);
        }

        protected override LuminSdk NewSdkInstance(NPath path, Version version, bool isDownloadable) =>
            new LuminSdk(path, version, isDownloadable);

        private static void TryReadCachedSDKDownloaderValues()
        {
            foreach (var envVar in _EnvironmentVariables)
            {
                var file = _CachedSDKDownloaderValues.Combine(envVar);
                if (file.FileExists())
                    Environment.SetEnvironmentVariable(file.FileNameWithoutExtension, file.ReadAllText().Trim());
            }
        }

        private IEnumerable<LuminSdk> ScanForInstalledSdks()
        {
            return ScanCommonLocationsFor("MagicLeap")
                .SelectMany(path => path.Combine("mlsdk").Directories())
                .Select(FromPath)
                .Where(sdk => sdk != null);
        }

        private IEnumerable<LuminSdk> ScanForSDKDownloaderSdks()
        {
            return ScanCommonLocationsFor("ML/SDK")
                .SelectMany(p => p.Directories())
                .Select(p => p.Combine("mlsdk"))
                .Select(FromPath)
                .Where(sdk => sdk != null);
        }

        private IEnumerable<LuminSdk> ScanForOverrideSdks()
        {
            return GetPossibleEnvironmentLocations()
                .Select(FromPath)
                .Where(sdk => sdk != null)
                .Append(Dummy);
        }
    }
}
