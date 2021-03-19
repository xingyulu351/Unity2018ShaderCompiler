using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Android
{
    public class AndroidNdkLocator : SdkLocator<AndroidNdk>
    {
        protected override Platform Platform { get; } = new AndroidPlatform();

        private Architecture Architecture { get; }

        public override AndroidNdk Dummy => NewSdkInstance("/__unable_to_locate_android_ndk__", new Version(), false);
        public override AndroidNdk UserDefault
        {
            get
            {
                var embeddedNDKPath = new NPath("External/Android/NonRedistributable/ndk/builds").MakeAbsolute();
                return FromPath(embeddedNDKPath) ??
                    FromPath(Environment.GetEnvironmentVariable("ANDROID_NDK_HOME")) ??
                    FromPath(Environment.GetEnvironmentVariable("ANDROID_NDK_ROOT"));
            }
        }

        public AndroidNdkLocator(Architecture architecture) => Architecture = architecture;

        private static Regex ParseDescriptionRegex => new Regex(@"Pkg\.Desc\s*=\s(.*)");
        private static Regex ParseRevisionRegex => new Regex(@"Pkg\.Revision\s*=\s*(.*)");

        public override AndroidNdk FromPath(NPath path)
        {
            if (path == null)
                return null;

            var sourceProperties = path.Combine("source.properties");
            if (!sourceProperties.FileExists())
                return null;

            var description = ParseDescriptionRegex.Match(sourceProperties.ReadAllText());
            if (!description.Success || "Android NDK" != description.Groups[1].Value)
                return null;

            var revision = ParseRevisionRegex.Match(sourceProperties.ReadAllText());
            if (!revision.Success)
                return null;

            Version.TryParse(revision.Groups[1].Value, out var version);
            if (version == null || version.Major < 16)
                return null;

            return NewSdkInstance(path, version, false);
        }

        protected override AndroidNdk NewSdkInstance(NPath path, Version version, bool isDownloadable)
        {
            return new AndroidNdk(Architecture, path, version, isDownloadable);
        }

        protected override IEnumerable<AndroidNdk> ScanForSystemInstalledSdks() =>
            ScanCommonLocationsFor("Android/Sdk/ndk-bundle/source.properties")
                .Select(file => file.Parent)
                .Select(FromPath)
                .Where(sdk => sdk != null);
    }
}
