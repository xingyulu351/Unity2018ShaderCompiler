using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.NativeProgramSupport;
using JamSharp.Runtime;
using Newtonsoft.Json.Linq;
using NiceIO;

namespace Bee.Toolchain.Xcode
{
    public abstract class XcodeSdkLocator<T> : SdkLocator<T> where T : XcodeSdk
    {
        // This is used for sdk folder lookups
        protected abstract string XcodePlatformName { get; }

        // Prefer an explicitly named Xcode 8.1 if it exists, for source code customers
        private NPath SystemXcodeApp => System.IO.Directory.Exists("/Applications/Xcode8.1.app") ? "/Applications/Xcode8.1.app" : "/Applications/Xcode.app";
        private NPath SystemXcodeAppPlatformSdksRoot => SystemXcodeApp.Combine(SdkRootPathRelativeXcodeApp);
        private NPath SdkRootPathRelativeXcodeApp => $"Contents/Developer/Platforms/{XcodePlatformName}.platform/Developer/SDKs";

        public override T UserDefault => DefaultSdkFromXcodeApp(SystemXcodeApp);

        protected override IEnumerable<T> ScanForSystemInstalledSdks() => SystemXcodeAppPlatformSdksRoot.DirectoryExists()
        ? SystemXcodeAppPlatformSdksRoot.Directories().Select(FromPath)
        : Array.Empty<T>();

        protected abstract T NewSdkInstance(NPath path, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null);
        protected override T NewSdkInstance(NPath path, Version version, bool isDownloadable) => NewSdkInstance(path, version, SystemXcodeApp, isDownloadable);

        protected T DefaultSdkFromXcodeApp(NPath xcodePath) =>
            FromPath(xcodePath.Combine($"{SdkRootPathRelativeXcodeApp}/{XcodePlatformName}.sdk"), xcodePath);

        protected T NewDefaultSdkInstanceFromXcodeApp(NPath xcodePath, Version version, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            => NewSdkInstance(xcodePath.Combine($"{SdkRootPathRelativeXcodeApp}/{XcodePlatformName}.sdk"), version, xcodePath, isDownloadable, backendRegistrables);

        public override T FromPath(NPath sdkPath) => FromPath(sdkPath, null);

        private T FromPath(NPath sdkPath, NPath xcodePath)
        {
            if (sdkPath == null)
                return null;

            if (xcodePath == null)
                xcodePath = sdkPath.MakeAbsolute().RecursiveParents.FirstOrDefault(f => f.FileName.Equals("Xcode.app"));

            var sdkSettingsPlist = sdkPath.Combine("SDKSettings.plist");
            if (!sdkSettingsPlist.Exists())
                return null;

            var result = Shell.Execute("/usr/bin/plutil", $"-convert json -o - -- {sdkSettingsPlist.InQuotes()}");
            Version.TryParse(JObject.Parse(result.StdOut)["Version"].ToString(), out var version);
            if (version == null)
                return null;

            return NewSdkInstance(sdkPath, version, xcodePath, isDownloadable: false);
        }
    }
}
