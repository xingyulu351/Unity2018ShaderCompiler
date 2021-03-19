using System;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using System.Linq;
using Unity.BuildTools;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using JamSharp.Runtime;

namespace Bee.Toolchain.Linux
{
    public static class LinuxSysrootHelper
    {
        public static IEnumerable<string> BinarySearchPathsFor(Sdk sdkBase)
        {
            LinuxSdk sdk = sdkBase as LinuxSdk;
            if (sdk == null)
                return Enumerable.Empty<string>();

            return sdk.RootedLibraryPaths.Select(path => path.InQuotes());
        }

        public static IEnumerable<string> LibrarySearchPathsFor(Sdk sdk)
        {
            yield return "'$ORIGIN'";

            foreach (var path in BinarySearchPathsFor(sdk))
                yield return path;
        }
    }

    public abstract class LinuxSdk : Sdk
    {
        private static readonly Regex GccLikeVersionMatch = new Regex(@"^(?<major>[0-9])(\.(?<minor>[0-9])(\.(?<build>\[0-9]))?)?", RegexOptions.Compiled);

        public override Version Version
        {
            get
            {
                if (CppCompilerPath != null)
                {
                    string version = string.Empty;
                    try
                    {
                        version = Shell.Execute($"{CppCompilerPath} -dumpversion").StdOut;
                    }
                    catch
                    {
                        return new Version();
                    }

                    var matches = GccLikeVersionMatch.Matches(version);
                    if (matches.Count > 0)
                    {
                        var match = matches[0];
                        var major = match.Groups["major"];
                        var minor = match.Groups["minor"];
                        var build = match.Groups["build"];
                        if (!major.Success)
                            throw new ArgumentException($"Malformed version string from {CppCompilerPath}: {version}");

                        if (build.Success)
                            return new Version(
                                int.Parse(major.Value),
                                minor.Success ? int.Parse(minor.Value) : 0,
                                int.Parse(build.Value));
                        else
                            return new Version(
                                int.Parse(major.Value),
                                minor.Success ? int.Parse(minor.Value) : 0);
                    }
                }
                return new Version();
            }
        }

        public override Platform Platform => new LinuxPlatform();

        public Architecture Architecture { get; } // can be null for architecture-independent SDKs
        public string TargetDistribution { get; } // can be null for distro-independent SDKs

        public string TargetTriple => Architecture != null && TargetDistribution != null
        ? $"{DefaultArchitectureNameFor(Architecture)}-{TargetDistribution}"
        : null;

        public override NPath Path { get; }
        public override NPath SysRoot { get; }

        // Returns sysroot include paths
        public IEnumerable<NPath> LateIncludePaths => SysRootPathsFor(LinuxIncludePaths);

        // Returns fully-qualified library paths in the form /sysroot/libdir
        public IEnumerable<NPath> RootedLibraryPaths => SysRootPathsFor(LinuxLibraryPaths, true);

        // Returns GCC-syntax library paths in the form =/libdir
        public override IEnumerable<NPath> LibraryPaths => SysRootPathsFor(base.LibraryPaths);

        // Returns GCC-syntax include paths in the form =/incdir
        public override IEnumerable<NPath> IncludePaths => SysRootPathsFor(base.IncludePaths);

        public override bool SupportedOnHostPlatform => HostPlatform.IsLinux;

        public static IEnumerable<string> ArchitectureNamesFor(Architecture arch)
        {
            if (arch is x64Architecture)
                return new string[] { "x86_64" };
            if (arch is x86Architecture)
                return new string[] { "i686", "i386" };

            throw new NotImplementedException($"Unsupported architecture: {arch.Name}");
        }

        public static string DefaultArchitectureNameFor(Architecture arch) => ArchitectureNamesFor(arch).FirstOrDefault();

        // Returns the full path to the exectuable
        public abstract NPath GetToolchainPathFor(string binary);

        // Returns a fully qualified path to 'path' inside the sysroot
        protected NPath FullSysRootPathFor(NPath path)
        {
            // If there is no sysroot or the path is already relative, return the path as-is
            if (SysRoot == null || path.IsRelative)
                return path;
            return SysRoot.Combine($".{path.ToString()}");
        }

        // Returns a GCC-syntax path to 'path' inside the sysroot
        protected NPath GccSysRootPathFor(NPath path)
        {
            // If there is no sysroot or the path is already relative, return the path as-is
            if (SysRoot == null || path.IsRelative)
                return path;
            return new NPath($"={path.ToString()}");
        }

        private IEnumerable<NPath> SysRootPathsFor(IEnumerable<NPath> paths, bool fullyQualified = false)
        {
            if (SysRoot == null)
                return Enumerable.Empty<NPath>();
            return paths.Select(path => fullyQualified ? FullSysRootPathFor(path) : GccSysRootPathFor(path));
        }

        private IEnumerable<string> LinuxLibraryPathsFor(Architecture arch, string distro, string targetTriple)
        {
            foreach (var libDir in new string[] { "/usr/lib", "/lib" })
            {
                // specific architecture and distro
                if (targetTriple != null)
                    yield return $"{libDir}/{targetTriple}";

                if (arch != null)
                {
                    // fall back to all arch names, specific distro
                    if (distro != null)
                    {
                        foreach (var n in ArchitectureNamesFor(arch))
                            yield return $"{libDir}/{n}-{distro}";
                    }

                    // fall back to all arch names, generic distro
                    foreach (var n in ArchitectureNamesFor(arch))
                        yield return $"{libDir}/{n}-linux-gnu";

                    // fall back to bitness, generic distro
                    yield return $"{libDir}{arch.Bits}";
                }

                // generic arch and distro
                yield return libDir;
            }
        }

        private IEnumerable<NPath> LinuxLibraryPaths =>
            LinuxLibraryPathsFor(Architecture, TargetDistribution, TargetTriple)
                .Distinct()
                .Select(path => new NPath(path))
                .Where(path => path.DirectoryExists());

        protected IEnumerable<NPath> LinuxIncludePaths => new NPath[]
        {
            "/usr/local/include",
            "/usr/include",
            "/include"
        };

        public virtual bool IsValid
        {
            get
            {
                return (Path != null) && (Path.IsRoot || Path.DirectoryExists())
                    && (SysRoot == null || SysRoot.DirectoryExists())
                    && CppCompilerPath != null && (CppCompilerPath.IsRelative || CppCompilerPath.FileExists())
                    && StaticLinkerPath != null && (StaticLinkerPath.IsRelative || StaticLinkerPath.FileExists())
                    && DynamicLinkerPath != null && (DynamicLinkerPath.IsRelative || DynamicLinkerPath.FileExists());
            }
        }

        public LinuxSdk(NPath sysroot, NPath binaryPath, Architecture arch, string targetDistribution = null)
        {
            SysRoot = sysroot != null && !sysroot.IsRoot ? sysroot : null;
            Path = binaryPath ?? new NPath("/");
            Architecture = arch;
            TargetDistribution = targetDistribution;
        }
    }
}
