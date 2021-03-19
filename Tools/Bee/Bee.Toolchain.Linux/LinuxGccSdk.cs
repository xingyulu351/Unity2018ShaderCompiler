using System;
using System.Text.RegularExpressions;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux
{
    public class LinuxGccSdk : LinuxSdk
    {
        public override NPath CppCompilerPath { get; }
        public override NPath DynamicLinkerPath { get; }
        public override NPath StaticLinkerPath { get; }

        public bool AssemblerRelaxRelocations { get; }

        public LinuxGccSdk(Architecture arch, NPath sysroot = null, NPath binaryPath = null, string target = null)
            : base(sysroot, binaryPath, arch, target)
        {
            CppCompilerPath = GetToolchainPathFor("g++");
            StaticLinkerPath = GetToolchainPathFor("ar");
            DynamicLinkerPath = GetToolchainPathFor("g++");

            // binutils assembler starting with 2.26 started emitting new type of relocation instructions,
            // which aren't supported by some of the older linkers we still use in some builds; ld fails with a:
            //     BFD (GNU Binutils for Ubuntu) 2.22 internal error, aborting at ../../bfd/reloc.c line 443 in bfd_get_reloc_size
            // So disable that until we move to a more modern linker version across the board
            AssemblerRelaxRelocations = FindAssemblerVersion() >= new Version(2, 26);
        }

        private string GetDefaultBinaryNameFor(string binary)
        {
            if (TargetDistribution != null && Architecture != null)
                return $"{DefaultArchitectureNameFor(Architecture)}-{TargetDistribution}-{binary}";
            else
                return binary;
        }

        // Returns the path in the form gccPath/bin/arch-target-binary if the information is available, e.g. x86_64-ubuntu14.04-linux-gnu-g++
        // Otherwise it defaults to just the exectuable name, e.g. gccPath/bin/g++.
        public override NPath GetToolchainPathFor(string binary)
        {
            return Path.Combine("bin").Combine(GetDefaultBinaryNameFor(binary));
        }

        Version FindAssemblerVersion()
        {
            var version = new Version(0, 0);
            var asPath = Path.Combine("as");
            if (!asPath.FileExists())
                return version;
            var res = JamSharp.Runtime.Shell.Execute(asPath, "--version");
            if (res.ExitCode != 0 || string.IsNullOrEmpty(res.StdOut))
                return version;
            var match = Regex.Match(res.StdOut, "GNU assembler \\(.*\\) (\\d+)\\.(\\d+)");
            if (!match.Success)
                return version;
            if (!Version.TryParse($"{match.Groups[1]}.{match.Groups[2]}", out version))
                return version;
            return version;
        }
    }

    public class LinuxSystemGccSdk : LinuxGccSdk
    {
        public override bool IsValid => true;

        public LinuxSystemGccSdk(Architecture arch)
            : base(arch)
        {
        }

        public override NPath GetToolchainPathFor(string binary) => new NPath(binary); // use $PATH
    }
}
