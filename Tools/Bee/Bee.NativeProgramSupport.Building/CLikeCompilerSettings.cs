using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public enum SIMD
    {
        sse,
        sse2,
        sse3,
        ssse3,
        sse4,
        sse4_1,
        sse4_2,
        avx,
        avx2
    }

    public enum OptimizationLevel
    {
        None, // don't optimize (gcclike: -Od, msvc: /Od)
        Speed, // optimize for speed (gcclike: -O2, msvc: /Ox)
        SpeedMax, // optimize for speed even more (gcclike: -O3, msvc: /Ox)
        Size, // optimize for size (gcclike: -Os, msvc: /O1)
    }

    public enum FloatingPointOptimization
    {
        Default,
        Fast
    }

    public abstract class CLikeCompilerSettings
    {
        protected CLikeCompilerSettings(CLikeCompiler compiler)
        {
            Compiler = compiler;
        }

        public bool Exceptions { get; internal set; } = false;
        public string[] Defines { get; internal set; } = Array.Empty<string>();
        public NPath[] IncludeDirectories { get; internal set; } = Array.Empty<NPath>();
        public Language Language { get; internal set; } = Language.Cpp;
        public bool RTTI { get; internal set; }
        public WarningAndPolicy[] WarningPolicies { get; internal set; } = Array.Empty<WarningAndPolicy>();
        public int CppLanguageVersion { get; internal set; } = 11;
        public OptimizationLevel Optimization { get; internal set; } = OptimizationLevel.Speed;
        public FloatingPointOptimization FloatingPoint { get; internal set; } = FloatingPointOptimization.Default;
        public SIMD Simd { get; internal set; } = SIMD.sse2; // default for x64 but we also want sse2 for x86 as well

        public ToolChain Toolchain => Compiler.Toolchain;
        public virtual NPath[] AdditionalInputs => Array.Empty<NPath>();
        public NPath OutputPCH { get; internal set; }
        public NPath InputPCH { get; internal set; }

        public bool OutputASM { get; internal set; }

        public virtual IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            yield break;
        }

        public abstract IEnumerable<string> CommandLineInfluencingCompilationFor(NPath outputObjectFile,
            NPath inputCppFile, NPath createOutputPchAt = null, NPath headerToCreateOutputPchFrom = null);
        public CLikeCompiler Compiler { get; }

        public object MemberWiseClone() => base.MemberwiseClone();

        public virtual ulong HashSettingsRelevantForCompilation()
        {
            var knuth = new KnuthHash();
            knuth.Add(Exceptions);
            knuth.Add(Defines);
            knuth.Add(IncludeDirectories);
            knuth.Add(RTTI);
            knuth.Add((int)Language);
            knuth.Add(CppLanguageVersion);
            knuth.Add(AdditionalInputs);
            knuth.Add((int)Simd);
            knuth.Add(InputPCH);
            knuth.Add((int)Optimization);
            knuth.Add((int)FloatingPoint);

            foreach (var wap in WarningPolicies)
            {
                knuth.Add(wap.Warning);
                knuth.Add((int)wap.Policy);
            }
            return knuth.Value;
        }

        protected virtual CLikeCompilerSettings CompilerSpecificCompilerSettingsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            NPath file,
            NPath artifactPath)
        {
            return
                this.WithOptimizationLevel(config.CodeGen == CodeGen.Debug
                ? OptimizationLevel.None
                : OptimizationLevel.Speed)
                    .WithExceptions(nativeProgram.Exceptions.For(config))
                    .WithRTTI(nativeProgram.RTTI.For(config));
        }

        public CLikeCompilerSettings CompilerSettingsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            NPath file,
            NPath artifactPath)
        {
            var settings = CompilerSpecificCompilerSettingsFor(nativeProgram, config, file, artifactPath);
            return nativeProgram.CompilerSettings().For(config, settings, file);
        }

        public virtual string[] OutputToSwallowFor(CLikeCompilationUnitInstruction cLikeCompilationUnitInstruction) => null;
    }

    public static class CLikeCompilerSettingsExtensions
    {
        //The With___ methods of base classes have to be done in an extension method like this, as it's the only
        //way to make them return the actual type of the derived class.
        public static T WithExceptions<T>(this T _this, bool value) where T : CLikeCompilerSettings
        {
            return _this.Exceptions == value ? _this : _this.With(c => c.Exceptions = value);
        }

        public static T WithDefines<T>(this T _this, string[] value) where T : CLikeCompilerSettings
        {
            return _this.Defines == value ? _this : _this.With(c => c.Defines = c.Defines.Concat(value).ToArray());
        }

        public static T WithIncludeDirectories<T>(this T _this, params NPath[] value) where T : CLikeCompilerSettings
        {
            return _this.IncludeDirectories == value
                ? _this
                : _this.With(c => c.IncludeDirectories = c.IncludeDirectories.Concat(value).ToArray());
        }

        public static T WithLanguage<T>(this T _this, Language value) where T : CLikeCompilerSettings
        {
            return _this.Language == value ? _this : _this.With(c => c.Language = value);
        }

        public static T WithRTTI<T>(this T _this, bool value) where T : CLikeCompilerSettings
        {
            return _this.RTTI == value ? _this : _this.With(c => c.RTTI = value);
        }

        public static T WithSSE<T>(this T _this, SIMD value) where T : CLikeCompilerSettings
        {
            return _this.Simd == value ? _this : _this.With(c => c.Simd = value);
        }

        public static T WithOptimizationLevel<T>(this T _this, OptimizationLevel value) where T : CLikeCompilerSettings
        {
            return _this.With(m => m.Optimization = value);
        }

        public static T WithFloatingPoint<T>(this T _this, FloatingPointOptimization value) where T : CLikeCompilerSettings
        {
            return _this.With(m => m.FloatingPoint = value);
        }

        public static T WithCppLanguageVersion<T>(this T _this, int value) where T : CLikeCompilerSettings
        {
            return _this.CppLanguageVersion == value ? _this : _this.With(c => c.CppLanguageVersion = value);
        }

        public static T WithWarningPolicies<T>(this T _this, WarningAndPolicy[] value) where T : CLikeCompilerSettings
        {
            return _this.WarningPolicies == value ? _this : _this.With(c => c.WarningPolicies = value);
        }

        public static T WithOutputPCH<T>(this T _this, NPath value) where T : CLikeCompilerSettings
        {
            return _this.OutputPCH == value ? _this : _this.With(c => c.OutputPCH = value);
        }

        public static T WithInputPCH<T>(this T _this, NPath value) where T : CLikeCompilerSettings
        {
            return _this.InputPCH == value ? _this : _this.With(c => c.InputPCH = value);
        }

        public static T WithOutputASM<T>(this T _this, bool value) where T : CLikeCompilerSettings
        {
            return _this.OutputASM == value ? _this : _this.With(c => c.OutputASM = value);
        }

        public static T With<T>(this T _this, Action<T> callback) where T : CLikeCompilerSettings
        {
            var copy = (T)_this.MemberWiseClone();
            callback(copy);
            return copy;
        }
    }
}
