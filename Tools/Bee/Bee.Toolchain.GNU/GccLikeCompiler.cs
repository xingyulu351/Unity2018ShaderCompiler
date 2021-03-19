using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.GNU
{
    public enum DebugMode
    {
        None,
        MinimalInformation,
        LineTablesOnly,
        IncludeDebugSymbols,
    }

    public enum Visibility
    {
        Default,
        Hidden
    }

    public enum StrictAliasingOptimization
    {
        Default, // strict aliasing optimizations off in most targets/platforms, since they are generally too crazy/extreme
        On,
        Off,
    }

    public class GccLikeCompilerSettings : CLikeCompilerSettings
    {
        public Visibility Visibility { get; internal set; } = Visibility.Hidden;
        private bool? _objcArc;
        public bool ObjcArc
        {
            get => _objcArc ?? DefaultForObjcArc();
            internal set => _objcArc = value;
        }

        public bool EmbedBitcode { get; internal set; }

        public DebugMode DebugMode { get; internal set; } = DebugMode.IncludeDebugSymbols;
        public bool PositionIndependentCode { get; internal set; }
        public StrictAliasingOptimization StrictAliasing { get; internal set; } = StrictAliasingOptimization.Default;

        public override ulong HashSettingsRelevantForCompilation()
        {
            var knuth = new KnuthHash();
            knuth.Add(base.HashSettingsRelevantForCompilation());
            knuth.Add((int)Visibility);
            knuth.Add((int)DebugMode);
            knuth.Add(ObjcArc);
            knuth.Add(EmbedBitcode);
            return knuth.Value;
        }

        public GccLikeCompilerSettings(GccLikeCompiler compiler) : base(compiler)
        {
        }

        private bool DefaultForObjcArc()
        {
            if (Language != Language.ObjectiveCpp)
                return false;

            return Toolchain.Architecture.Bits == 64;
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            foreach (var p in FlagsForLanguageVersion()) yield return p;

            if (ObjcArc && (Language == Language.ObjectiveC || Language == Language.ObjectiveCpp))
                yield return "-fobjc-arc";

            if (EmbedBitcode && Toolchain.Architecture is ArmArchitecture)
                yield return "-fembed-bitcode";

            foreach (var wap in WarningPolicies)
            {
                switch (wap.Policy)
                {
                    case WarningPolicy.Silent:
                        yield return $"-Wno-{wap.Warning}";
                        break;
                    case WarningPolicy.AsWarning:
                        yield return $"-W{wap.Warning}";
                        break;
                    case WarningPolicy.AsError:
                        yield return $"-Werror={wap.Warning}";
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }

            switch (Visibility)
            {
                case Visibility.Default:
                    yield return "-fvisibility=default";
                    break;
                case Visibility.Hidden:
                    yield return "-fvisibility=hidden";
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            // NOTE: we want to limit this for IntelArchitecture as other architectures (like WebGL's AsmJs) fail linking if you pass it
            // Even if values like -msse2 could be the default on some architectures (x64) we want to explicitly pass it to ensure that.
            if (Toolchain.Architecture is IntelArchitecture)
                yield return $"-m{Simd.ToString().Replace("_", ".")}";

            if (Toolchain.Sdk.SysRoot != null)
                yield return $"--sysroot={Toolchain.Sdk.SysRoot.InQuotes()}";

            if (Language == Language.Cpp || Language == Language.ObjectiveCpp)
            {
                yield return Exceptions ? "-fexceptions" : "-fno-exceptions";
                yield return RTTI ? "-frtti" : "-fno-rtti";
            }

            switch (DebugMode)
            {
                case DebugMode.None:
                    break;
                case DebugMode.MinimalInformation:
                    yield return "-g1";
                    break;
                case DebugMode.IncludeDebugSymbols:
                    yield return "-g"; // equivalent with -g2
                    break;
                case DebugMode.LineTablesOnly:
                    yield return "-gline-tables-only";
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            switch (Optimization)
            {
                case OptimizationLevel.None: yield return "-O0"; break;
                case OptimizationLevel.Speed: yield return "-O2"; break;
                case OptimizationLevel.SpeedMax: yield return "-O3"; break;
                case OptimizationLevel.Size: yield return "-Os"; break;
                default: throw new ArgumentOutOfRangeException();
            }

            switch (FloatingPoint)
            {
                case FloatingPointOptimization.Default: break; // nothing
                case FloatingPointOptimization.Fast: yield return "-ffast-math"; break;
                default: throw new ArgumentOutOfRangeException();
            }

            if (PositionIndependentCode)
                yield return "-fPIC";

            if (StrictAliasing != StrictAliasingOptimization.On)
                yield return "-fno-strict-aliasing";

            if (!EmbedBitcode) // separate function/data sections are incompatible with bitcode
            {
                yield return "-ffunction-sections";
                yield return "-fdata-sections";
            }

            yield return "-fmessage-length=0"; // do not linewrap compiler output (default is 72 chars for g++)
            yield return "-pipe"; // use pipes instead of temp files for some compilation internals

            foreach (var i in Toolchain.Sdk.IncludePaths)
                yield return "-isystem " + i.InQuotes();

            foreach (var d in Defines.Select(d => "-D" + d))
                yield return d;
            foreach (var i in IncludeDirectories.Select(i =>
                "-I" + i.InQuotes()))
                yield return i;

            yield return "-o";
            yield return target.InQuotes();
        }

        public override IEnumerable<string> CommandLineInfluencingCompilationFor(NPath outputObjectFile, NPath inputCppFile, NPath createOutputPchAt = null, NPath headerToCreateOutputPchFrom = null)
        {
            foreach (var flag in CommandLineFlagsFor(createOutputPchAt ?? outputObjectFile))
                yield return flag;

            yield return "-c";


            foreach (var p in GetLanguageArgument(createOutputPchAt)) yield return p;

            yield return (headerToCreateOutputPchFrom ?? inputCppFile).MakeAbsolute().InQuotes();
        }

        protected virtual IEnumerable<string> GetLanguageArgument(NPath createOutputPchAt)
        {
            var buildingPchPostFix = createOutputPchAt != null ? "-header" : String.Empty;
            switch (Language)
            {
                case Language.C:
                    yield return "-x c" + buildingPchPostFix;
                    break;
                case Language.Cpp:
                    yield return "-x c++" + buildingPchPostFix;
                    break;
                case Language.ObjectiveC:
                    yield return "-x objective-c" + buildingPchPostFix;
                    break;
                case Language.ObjectiveCpp:
                    yield return "-x objective-c++" + buildingPchPostFix;
                    yield return "-fobjc-call-cxx-cdtors";
                    break;
                case Language.AssemblerWithCpp:
                    yield return "-x assembler-with-cpp";
                    break;
                default:
                    throw new NotSupportedException($"Unsupported language: {Language}");
            }
        }

        protected virtual IEnumerable<string> FlagsForLanguageVersion()
        {
            if (Language == Language.Cpp || Language == Language.ObjectiveCpp)
                yield return $"-std=c++{CppLanguageVersion}";
        }
    }

    public static class GccLikeCompilerSettingsExtensions
    {
        public static T WithVisibility<T>(this T _this, Visibility value) where T : GccLikeCompilerSettings
        {
            return _this.With(m => m.Visibility = value);
        }

        public static T WithObjcArc<T>(this T _this, bool value) where T : GccLikeCompilerSettings
        {
            return _this.With(m => m.ObjcArc = value);
        }

        public static T WithEmbedBitcode<T>(this T _this, bool value) where T : GccLikeCompilerSettings
        {
            return _this.With(m => m.EmbedBitcode = value);
        }

        public static T WithDebugMode<T>(this T _this, DebugMode value) where T : GccLikeCompilerSettings
        {
            return _this.With(m => m.DebugMode = value);
        }

        public static T WithPositionIndependentCode<T>(this T _this, bool value) where T : GccLikeCompilerSettings =>
            _this.With(m => m.PositionIndependentCode = value);

        public static T WithStrictAliasing<T>(this T _this, StrictAliasingOptimization value) where T : GccLikeCompilerSettings => _this.With(m => m.StrictAliasing = value);
    }

    public abstract class GccLikeCompiler : CLikeCompiler
    {
        public override string ObjectExtension { get; } = "o";
        public override string[] SupportedExtensions { get; } = {"cpp", "c", "s", "cc"};
        public override string WarningStringForUnusedVariable => "unused-variable";

        // When ccache executable path is set, and compiler returns true for SupportsCCache,
        // the actual invocations will be routed through the given ccache executable
        public static NPath CCachePath { get; set; }
        protected virtual bool SupportsCCache => false;

        bool WillUseCCache => SupportsCCache && CCachePath != null;

        public virtual bool SupportsColorDiagnostics => true;

        public override string CppCompilerCommand
        {
            get
            {
                if (WillUseCCache)
                    return $"{CCachePath.InQuotes(SlashMode.Native)} {base.CppCompilerCommand}";
                return base.CppCompilerCommand;
            }
        }

        // ccache has bad interactions with precompiled headers; prefer ccache instead of pch
        public override bool SupportsPCH => !WillUseCCache;

        protected GccLikeCompiler(ToolChain toolchain) : base(toolchain) {}
    }
}
