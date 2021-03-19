using System.Collections.Generic;
using System.Linq;
using Bee.Toolchain.LLVM;
using Bee.Toolchain.GNU;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux
{
    public class LinuxClangCompiler : ClangCompiler
    {
        protected override bool SupportsResponseFile => !(Toolchain.Sdk is WSLSdk);

        public LinuxClangCompiler(ToolChain toolchain) : base(toolchain)
        {
            DefaultSettings = new LinuxClangCompilerSettings(this)
                .WithDefines(new[]
            {
                "_FILE_OFFSET_BITS=64",         // Use 64 bit file system interface even on 32 bit architectures
                "__STDC_LIMIT_MACROS"           // Tell older versions of libstdc++ to enable limit macros
            })
                .WithPositionIndependentCode(Architecture is x64Architecture);
        }
    }

    public class LinuxClangCompilerSettings : ClangCompilerSettings
    {
        protected string TargetTriple => (Toolchain.Sdk as LinuxClangSdk)?.TargetTriple;
        protected override NPath GccToolchain => (Toolchain.Sdk as LinuxClangSdk)?.GccToolchain;

        public LinuxClangCompilerSettings(ClangCompiler compiler) : base(compiler)
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

            if (!string.IsNullOrEmpty(TargetTriple))
                yield return $"-target {TargetTriple}";

            // compile (and  link) with pthread by default
            yield return "-pthread";
        }

        protected override IEnumerable<string> FlagsForLanguageVersion()
        {
            // petele: todo: we're using GNU C++ for now. Eventually we'll break this out into
            // libc++ for clang (and it'll be optional).
            if (Language == Language.Cpp || Language == Language.ObjectiveCpp)
                yield return $"-std=gnu++{CppLanguageVersion}";
        }
    }
}
