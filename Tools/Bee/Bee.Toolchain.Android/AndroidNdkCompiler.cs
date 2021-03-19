using System.Collections.Generic;
using Bee.Toolchain.GNU;
using Bee.Toolchain.LLVM;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Android
{
    public class AndroidNdkCompiler : ClangCompiler
    {
        protected override bool SupportsCCache => true;

        public AndroidNdkCompiler(AndroidNdkToolchain toolchain, int apiLevel) : base(toolchain)
        {
            DefaultSettings = new AndroidNdkCompilerSettings(this, apiLevel)
                .WithPositionIndependentCode(true);
        }
    }

    public class AndroidNdkCompilerSettings : ClangCompilerSettings
    {
        readonly int _apiLevel;

        public AndroidNdkCompilerSettings(AndroidNdkCompiler gccCompiler, int apiLevel) : base(gccCompiler)
        {
            _apiLevel = apiLevel;
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            // architecture specific flags
            if (Compiler.Architecture is ARMv7Architecture)
            {
                yield return "-march=armv7-a";
                yield return "-mfloat-abi=softfp";
                yield return "-mfpu=neon-fp16";
                yield return "-D__ARM_ARCH_7__";
                yield return "-D__ARM_ARCH_7A__";
                yield return "--target=armv7-none-linux-androideabi";
            }
            if (Compiler.Architecture is Arm64Architecture)
            {
                yield return "-march=armv8-a";
                yield return "--target=aarch64-none-linux-android";
            }
            if (Compiler.Architecture is x86Architecture)
            {
                yield return "-Di386=1";
                yield return "-mtune=atom";
                yield return "-mssse3";
                yield return "-mfpmath=sse";
                yield return "-mstackrealign";
                yield return "--target=i686-none-linux-android";
            }
            yield return $"-D__ANDROID_API__={_apiLevel}";

            // flags passed to everything on Android
            yield return "-DANDROID";
            // NDK has inttypes.h; e.g. JNI headers check for that
            yield return "-DHAVE_INTTYPES_H";

            yield return "-no-canonical-prefixes";
            yield return "-funwind-tables";
            yield return "-fstack-protector";
            yield return "-fdiagnostics-format=msvc";

            if (Optimization != OptimizationLevel.None)
            {
                // important for performance. Frame pointer is only useful for profiling, but
                // introduces additional instructions into the prologue and epilogue of each function
                // and leaves one less usable register.
                yield return "-fomit-frame-pointer";
            }

            // all other compile flags
            foreach (var flag in base.CommandLineFlagsFor(target))
                yield return flag;

            // Default to strict aliasing on, unless explicitly asked off. This is done after base class
            // flags, since that one emits "no strict aliasing" by default.
            if (Optimization != OptimizationLevel.None && StrictAliasing != StrictAliasingOptimization.Off)
            {
                // important for performance
                yield return "-fstrict-aliasing";
            }

            // help visual studio parse error messages
            yield return "-fdiagnostics-absolute-paths";
            yield return "-fdiagnostics-format=msvc";
        }
    }
}
