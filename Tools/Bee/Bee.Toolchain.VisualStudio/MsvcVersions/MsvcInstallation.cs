using System;
using System.Collections.Generic;
using System.Linq;
using JamSharp.Runtime;
using Microsoft.Win32;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.VisualStudio.MsvcVersions
{
    public abstract class MsvcInstallation
    {
        private static readonly Dictionary<Version, MsvcInstallation> _installations;

        public static IEnumerable<MsvcInstallation> Installations => _installations.Values;
        public virtual NPath VisualStudioDirectory { get; set; }
        public Windows10Sdk Windows10Sdk { get; }
        protected bool HasCppSDK => Windows10Sdk != null && Windows10Sdk.Path.Exists();

        public Version Version { get; set; }
        public abstract string HumanName { get; }
        public abstract NPath[] GetIncludeDirectories(Architecture architecture);
        public abstract IEnumerable<NPath> GetLibDirectories(Architecture architecture, string sdkSubset = null);

        protected virtual IEnumerable<NPath> GetSDKBinDirectories() { yield return Windows10Sdk.Path.Combine("bin"); }
        public abstract NPath WinRTPlatformMetadata { get; }

        private readonly bool _use64BitTools;

        protected MsvcInstallation(Version visualStudioVersion, NPath visualStudioDir, Windows10Sdk windows10Sdk, bool use64BitTools = true)
        {
            VisualStudioDirectory = visualStudioDir;
            Version = visualStudioVersion;
            _use64BitTools = use64BitTools;
            Windows10Sdk = windows10Sdk;
        }

        public virtual string GetPathEnvVariable(Architecture architecture)
        {
            var paths = new List<NPath>();

            if (architecture is x86Architecture)
            {
                if (_use64BitTools)
                {
                    paths.Add(VisualStudioDirectory.Combine("bin", "amd64_x86"));
                    paths.Add(VisualStudioDirectory.Combine("bin", "amd64"));

                    if (Windows10Sdk != null)
                        paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x64")));
                }
                else
                {
                    paths.Add(VisualStudioDirectory.Combine("bin"));
                    paths.Add(VisualStudioDirectory.Combine("../Common7/IDE"));
                }

                if (Windows10Sdk != null)
                    paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x86")));
            }
            else if (architecture is ARMv7Architecture)
            {
                if (_use64BitTools)
                {
                    paths.Add(VisualStudioDirectory.Combine("bin", "amd64_arm"));
                    paths.Add(VisualStudioDirectory.Combine("bin", "amd64"));

                    if (Windows10Sdk != null)
                        paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x64")));
                }
                else
                {
                    paths.Add(VisualStudioDirectory.Combine("bin", "x86_arm"));
                    paths.Add(VisualStudioDirectory.Combine("bin"));
                }

                if (Windows10Sdk != null)
                    paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x86")));
            }
            else if (architecture is x64Architecture)
            {
                paths.Add(VisualStudioDirectory.Combine("bin", "amd64"));

                if (Windows10Sdk != null)
                {
                    paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x64")));
                    paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x86")));
                }
            }
            else
            {
                throw new NotSupportedException($"'{architecture.Name}' architecture is not supported.");
            }

            return paths.Select(p => p.ToString(SlashMode.Native)).SeparateWith(";");
        }

        public virtual NPath GetVSToolPath(Architecture architecture, string toolName)
        {
            var binFolder = VisualStudioDirectory.Combine("bin");

            if (architecture is x86Architecture)
                return _use64BitTools ? binFolder.Combine("amd64_x86", toolName) : binFolder.Combine(toolName);

            if (architecture is x64Architecture)
                return binFolder.Combine("amd64", toolName);

            if (architecture is ARMv7Architecture)
                return _use64BitTools ? binFolder.Combine("amd64_arm", toolName) : binFolder.Combine("x86_arm", toolName);

            throw new NotSupportedException("Can't find MSVC tool for " + architecture);
        }

        public virtual NPath GetSDKToolPath(string toolName)
        {
            var architecture = Architecture.BestThisMachineCanRun;
            string architectureFolder;

            if (architecture is x86Architecture)
            {
                architectureFolder = "x86";
            }
            else if (architecture is x64Architecture)
            {
                architectureFolder = "x64";
            }
            else
            {
                throw new NotSupportedException($"Invalid host architecture: {architecture.GetType().Name}");
            }

            foreach (var directory in GetSDKBinDirectories())
            {
                var toolPath = directory.Combine(architectureFolder, toolName);
                if (toolPath.FileExists())
                    return toolPath;
            }

            throw new NotSupportedException("Can't find MSVC tool for " + architecture);
        }

        public static IEnumerable<MsvcInstallation> InstallationsFor(Version version)
        {
            return Installations.Where(v => v.Version.Equals(version));
        }

        static MsvcInstallation()
        {
            _installations = new Dictionary<Version, MsvcInstallation>();

            var msvc14Version = new Version(14, 0);
            var msvc15Version = new Version(15, 0);

            var msvc14InstallationFolder = GetVisualStudioInstallationFolder(msvc14Version, Msvc14Installation.PackagePath);
            var msvc15InstallationFolder = GetVisualStudioInstallationFolder(msvc15Version, Msvc15Installation.PackagePath);
            var windows10Sdk = WindowsSDKs.GetWindows10Sdk();

            if (msvc14InstallationFolder != null)
            {
                var msvc14 = new Msvc14Installation(msvc14InstallationFolder, windows10Sdk, WindowsSDKs.GetDotNetFrameworkSDKDirectory());

                if (msvc14.HasCppSDK)
                    _installations.Add(msvc14Version, msvc14);
            }

            if (msvc15InstallationFolder != null)
            {
                var msvc15 = new Msvc15Installation(msvc15InstallationFolder, windows10Sdk);

                if (msvc15.HasCppSDK)
                    _installations.Add(msvc15Version, msvc15);
            }
        }

        public static NPath GetVisualStudioInstallationFolder(Version version, NPath packagePath = null)
        {
            if (!HostPlatform.IsWindows)
                return null;

            if (packagePath != null && packagePath.FileExists())
            {
                return packagePath.Parent.Combine("builds");
            }

            // 15.0 (VS2017) and later use vswhere based detection
            if (version.Major >= 15)
            {
                return GetVisualStudioInstallationFolderVswhere(version);
            }

            // older VS versions use registry or environment variables
            var key = Registry.CurrentUser.OpenSubKey($@"SOFTWARE\Microsoft\VisualStudio\{version.Major}.{version.Minor}_Config");

            var installationFolder = (string)key?.GetValue("InstallDir");

            if (string.IsNullOrEmpty(installationFolder))
            {
                // Couldn't retrieve it from the registry, so as a fallback, try the VS*COMNTOOLS env var
                installationFolder = Environment.GetEnvironmentVariable($"VS{version.Major}{version.Minor}COMNTOOLS");
            }

            if (!string.IsNullOrEmpty(installationFolder))
                return new NPath(installationFolder).Parent.Parent.Combine("VC"); // installationFolder points to <dir>\Common7\IDE (in registry) or <dir>\Common7\Tools\ (in env var)

            return null;
        }

        private static NPath GetVisualStudioInstallationFolderVswhere(Version version)
        {
            // Starting with VS2017, paths are no longer put into registry; see
            // "Finding installed Visual C++ tools for Visual Studio 2017"
            // https://blogs.msdn.microsoft.com/vcblog/2017/03/06/finding-the-visual-c-compiler-tools-in-visual-studio-2017/

            var programFiles = Environment.GetEnvironmentVariable("ProgramFiles(x86)");
            if (string.IsNullOrEmpty(programFiles))
                return null;
            var vswhere = programFiles.ToNPath().Combine("Microsoft Visual Studio", "Installer", "vswhere.exe");
            if (!vswhere.FileExists())
                return null;

            // As VS2017 supports side by side installs (i.e. Community, Professional & Enterprise), we use
            // -latest to ensure the newest and last installed version is returned. Note that limiting
            // the version range to 16.0 (exclusive) stops VS2019 from being selected if present (since it's
            // not supported for 2018.4).
            NPath path = Shell.Execute(vswhere, $"-property installationPath -prerelease -version [{version.Major}.{version.Minor},16.0) -latest").StdOut;
            if (path == null || path.Depth == 0)
                return null;

            var toolsRootPath = path.Combine("VC", "Tools", "MSVC");
            NPath toolsPath = null;

            // Inside of whole VS install, the compiler toolchain is in a subfolder with a specific version
            var versionFilePath = path.Combine("VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt");
            if (versionFilePath.FileExists())
            {
                var toolsVersion = versionFilePath.ReadAllText().Trim();
                toolsPath = toolsRootPath.Combine(toolsVersion);
            }
            else
            {
                // The version file may not exist in some installs (such as Visual Studio 2017 Community)
                // In this case just select the newest VC tools version based on directory name
                if (toolsRootPath.DirectoryExists())
                {
                    var directories = toolsRootPath.Directories();
                    Array.Sort(directories);
                    if (directories.Count() > 0)
                    {
                        toolsPath =  directories.Last();
                    }
                    else
                    {
                        Errors.Exit($"Error with Visual Studio 2017 installation. Can't find any MSVC subdirectories in {toolsRootPath}. Was C++ workload selected during install?");
                    }
                }
                else
                {
                    Errors.Exit($"Error with Visual Studio 2017 installation. Can't find MSVC directory at {toolsRootPath}. Was C++ workload selected during install?");
                }
            }

            return toolsPath;
        }

        public static MsvcInstallation GetLatestInstalled()
        {
            var key = _installations.Keys.OrderByDescending(k => k.Major).ThenByDescending(k => k.Minor).FirstOrDefault();

            if (key != null)
                return _installations[key];

            return null;
        }

        public static MsvcInstallation GetLatestInstallationAtLeast(Version version)
        {
            var key = _installations.Keys.OrderByDescending(v => v.Major).ThenByDescending(v => v.Minor).FirstOrDefault(v => v >= version);

            if (key != null)
                return _installations[key];

            throw new Exception($"MSVC Installation version {version.Major}.{version.Minor} or later is not installed on current machine!");
        }
    }
}
