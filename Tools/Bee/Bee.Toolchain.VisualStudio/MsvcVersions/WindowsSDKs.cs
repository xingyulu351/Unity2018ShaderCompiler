using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Win32;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.VisualStudio.MsvcVersions
{
    public class Windows10Sdk
    {
        public NPath Path { get; }
        public string Version { get; }

        public NPath IncludeDirectory => Path.Combine("Include").Combine(Version);
        public NPath LibDirectory => Path.Combine("Lib").Combine(Version);

        public Windows10Sdk(string version, NPath path)
        {
            Path = path ?? throw new ArgumentNullException();
            Version = version ?? throw new ArgumentNullException();
        }

        public List<NPath> GetBinDirectories()
        {
            var binDirectories = new List<NPath>();
            var commonBinDirectory = Path.Combine("bin");
            // Cannot do this DirectoryExists check because the SDK has not been downloaded/unpacked yet
            var sdkSpecificBinDirectory = commonBinDirectory.Combine(Version);
            if (sdkSpecificBinDirectory.DirectoryExists())
               binDirectories.Add(sdkSpecificBinDirectory);
            binDirectories.Add(commonBinDirectory);
            return binDirectories;
        }
    }

    public static class WindowsSDKs
    {
        public static NPath PackagePath { get; } = $"{Paths.UnityRoot}/PlatformDependent/Win/External/NonRedistributable/win10sdk/builds.7z";
        public const string PackagedSDKVersion = "10.0.17134.0";

        private static Version TryParseVersion(string sdkVersionString)
        {
            Version sdkVersion;
            if (!Version.TryParse(sdkVersionString, out sdkVersion))
                return null;

            if (sdkVersion.Build == -1)
                sdkVersion = new Version(sdkVersion.Major, sdkVersion.Minor, 0, 0);
            else if (sdkVersion.Revision == -1)
                sdkVersion = new Version(sdkVersion.Major, sdkVersion.Minor, sdkVersion.Build, 0);
            return sdkVersion;
        }

        static Windows10Sdk _cachedSdk;

        public static Windows10Sdk GetWindows10Sdk()
        {
            if (_cachedSdk != null)
                return _cachedSdk;

            // use pre-packaged SDK if we have one
            if (PackagePath.FileExists())
            {
                _cachedSdk = new Windows10Sdk(PackagedSDKVersion, PackagePath.Parent.Combine("builds"));
                return _cachedSdk;
            }

            _cachedSdk = GetWindows10SdkFromRegistry();
            if (_cachedSdk != null)
                return _cachedSdk;

            // might not be in registry with some VS installation settings; try checking a known default location
            var programFilesX86 = Environment.GetEnvironmentVariable("ProgramFiles(x86)");
            if (!string.IsNullOrEmpty(programFilesX86))
            {
                var sdksRoot = programFilesX86.ToNPath().Combine("Windows Kits", "10");
                var checkDir = programFilesX86.ToNPath().Combine("Windows Kits", "10", "Include");
                if (checkDir.DirectoryExists())
                {
                    var sdkVersions = new List<Version>();
                    foreach (var sdkDirCandidate in checkDir.Directories())
                    {
                        var sdkVersion = TryParseVersion(sdkDirCandidate.FileName);
                        if (sdkVersion != null)
                            sdkVersions.Add(sdkVersion);
                    }
                    var key = sdkVersions.OrderByDescending(c => c).FirstOrDefault();
                    if (key != null)
                    {
                        _cachedSdk = new Windows10Sdk(key.ToString(), sdksRoot);
                        return _cachedSdk;
                    }
                }
            }

            return null;
        }

        static Windows10Sdk GetWindows10SdkFromRegistry()
        {
            if (!HostPlatform.IsWindows) return null;
            var key = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Windows\v10.0");
            if (key == null)
                return null;

            var sdkDir = (string)key.GetValue("InstallationFolder");

            if (string.IsNullOrEmpty(sdkDir))
                return null;

            var sdkVersion = TryParseVersion((string)key.GetValue("ProductVersion")) ?? new Version(10, 0, 17134);
            var sdkVersionString = sdkVersion.ToString();

            // Depending on the order of VS/SDK installations, registry might end up containing stale info (e.g. version
            // of SDK that is actually uninstalled by now). Check whether include/library folders actually exist before
            // trusting it.
            if (!new NPath($"{sdkDir}/Include/{sdkVersionString}").DirectoryExists() || !new NPath($"{sdkDir}/Lib/{sdkVersionString}").DirectoryExists())
            {
                Errors.PrintWarning($"Registry says that Windows 10 SDK version {sdkVersionString} should be installed at {sdkDir}, but we can't find it there. Will fallback to non-registry based SDK detection.");
                return null;
            }

            return new Windows10Sdk(sdkVersionString, sdkDir.ToNPath());
        }

        public static NPath GetDotNetFrameworkSDKDirectory()
        {
            if (!HostPlatform.IsWindows) return null;
            var key = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\NETFXSDK\4.6.1");

            if (key == null)
                key = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\NETFXSDK\4.6");

            if (key == null)
                return null;

            var netfxsdkDir = (string)key.GetValue("KitsInstallationFolder");
            if (string.IsNullOrEmpty(netfxsdkDir))
                return null;

            return netfxsdkDir.ToNPath();
        }

        public static NPath WinRTWindowsMetadataPath
        {
            get
            {
                var sdk = GetWindows10Sdk();
                if (sdk == null)
                    throw new Exception("Could not find Windows 10 SDK installed on the computer.");

                var sdkSpecific = sdk.Path.Combine("UnionMetadata", sdk.Version, "Windows.winmd");
                if (sdkSpecific.FileExists())
                    return sdkSpecific;

                var common = sdk.Path.Combine("UnionMetadata", "Windows.winmd");
                if (common.FileExists())
                    return common;

                throw new Exception($"Could not locate Windows.winmd file; looked at {common.InQuotes()} and {sdkSpecific.InQuotes()}");
            }
        }
    }
}
