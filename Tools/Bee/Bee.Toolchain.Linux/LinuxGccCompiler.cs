using System.Collections.Generic;
using System.Linq;
using Bee.Toolchain.GNU;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux
{
    public class LinuxGccCompiler : GccCompiler
    {
        public override string PCHExtension { get; } = "h.gch";
        protected override bool SupportsResponseFile => !(Toolchain.Sdk is WSLSdk);

        //Unity's linux build agents have a system gcc that is too old for colored diagnostics
        public override bool SupportsColorDiagnostics => false;

        public LinuxGccCompiler(ToolChain toolchain) : base(toolchain)
        {
            DefaultSettings = new LinuxGccCompilerSettings(this)
                .WithDefines(new[]
            {
                "_FILE_OFFSET_BITS=64",         // Use 64 bit file system interface even on 32 bit architectures
                "_GLIBCXX_USE_CXX11_ABI=0",     // Use pre-gcc5 abi for interoperating with other c++ libs
                "__STDC_LIMIT_MACROS"           // Tell older versions of libstdc++ to enable limit macros
            })
                .WithPositionIndependentCode(Architecture is x64Architecture)
                .WithBinarySearchPath(LinuxSysrootHelper.BinarySearchPathsFor(toolchain.Sdk).ToArray());
        }
    }

    public class LinuxGccCompilerSettings : GccCompilerSettings
    {
        public LinuxGccCompilerSettings(GccLikeCompiler compiler) : base(compiler)
        {
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            foreach (var flag in base.CommandLineFlagsFor(target))
                yield return flag;

            if (Toolchain.Architecture is x64Architecture)
            {
                yield return "-m64";
            }
            if (Toolchain.Architecture is x86Architecture)
            {
                yield return "-mfpmath=sse";
                yield return "-m32";
            }

            // compile (and  link) with pthread by default
            yield return "-pthread";

            // Gcc is very sensitive and we have to tone it down a bit, so we allow some nonconforming code to compile.
            // Should this really be here? feels like an option that should be turned off by default
            if (Language == Language.Cpp)
                yield return "-fpermissive";

            // Add sysroot include directories (scanned by GCC after -isystem and -I paths)
            if (Toolchain.Sdk is LinuxSdk linuxSdk)
            {
                foreach (var lateIncludePath in linuxSdk.LateIncludePaths)
                    yield return "-idirafter " + lateIncludePath.InQuotes();
            }

            if (Toolchain.Sdk is LinuxGccSdk gccSdk && gccSdk.AssemblerRelaxRelocations)
            {
                yield return "-Wa,-mrelax-relocations=no";
            }
        }

        protected override IEnumerable<string> FlagsForLanguageVersion()
        {
            // on gcc some of our code constructs need GNU extensions for C++
            // to be enabled, hence using gnu++11 etc.
            if (Language == Language.Cpp || Language == Language.ObjectiveCpp)
                yield return $"-std=gnu++{CppLanguageVersion}";
        }
    }
}
