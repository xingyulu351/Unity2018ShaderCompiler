using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;
using Unity.BuildSystem.VisualStudio.MsvcVersions;

namespace Bee.Toolchain.VisualStudio
{
    public enum StandardLibrary
    {
        MultiThreadedDebug,
        MultiThreaded,
        MultiThreadedDebugDll,
        MultiThreadedDll,
    }

    public abstract class MsvcCompiler : CLikeCompiler
    {
        public override string ObjectExtension { get; } = "obj";
        public override string WarningStringForUnusedVariable => "4189";

        protected MsvcCompiler(VisualStudioToolchain toolchain) : base(toolchain) {}

        protected override NPath SetupOutputPCH(NPath outputPch,
            NPath inputPch,
            CLikeCompilerSettings settings,
            NPath artifactsPath)
        {
            //VisualStudio is the only toolchain that cannot compile a header into a precompiled header, it requires that you have a cpp file that includes
            //the header you want to compile. In order to hide this annoyances from NativeProgram users, we create the cpp file automatically for our users.
            var stubFile = outputPch.ChangeExtension("cpp");
            Backend.Current.AddWriteTextAction(stubFile, $"#include \"{inputPch}\"");


            //we do not recycle the base class' implementation of adding the action to the graph, because visualstudio when it creates the pch, actually creates
            //both a pch and an obj, and it's important for both targets to be registered. Also, it also has 2 input files instead of 1: both the header to compile
            //_and_ its stub cpp file.
            var stubObj = outputPch.ChangeExtension(ObjectExtension);
            var targetFiles = new[] {outputPch, stubObj};
            Backend.Current.AddAction(
                $"{ActionName}Pch",
                targetFiles,
                new[] {inputPch, stubFile},
                CppCompilerCommand,
                settings.CommandLineInfluencingCompilationFor(
                    outputObjectFile: stubObj,
                    inputCppFile: stubFile,
                    createOutputPchAt: outputPch,
                    headerToCreateOutputPchFrom: inputPch).ToArray()
            );


            Backend.Current.ScanCppFileForHeaders(outputPch, inputPch, settings.IncludeDirectories);

            //return the stubObj as an additional object file, so it can be included as a linker input,
            //which is a requirement for the visualstudio pch system and linker to work.
            return stubObj;
        }
    }

    public class MsvcCompilerSettings : CLikeCompilerSettings
    {
        public CodeGen CodeGen { get; protected set; }
        public bool Unicode { get; protected set; } = true;
        public NPath PDB { get; protected set; }
        public bool UseStaticRuntimeLibrary { get; protected set; } = false;

        public bool WinRTExtensions { get; private set; } = false;
        public NPath[] WinRTAssemblies { get; private set; } = Array.Empty<NPath>();

        public override NPath[] AdditionalInputs => WinRTAssemblies;

        public MsvcCompilerSettings WithPDB(NPath pdb) => this.With(c => c.PDB = pdb);
        public MsvcCompilerSettings WithCodegen(CodeGen codeGen) => this.With(c => c.CodeGen = codeGen);
        public MsvcCompilerSettings WithWinRTExtensions(bool value) => this.With(c => c.WinRTExtensions = value);
        public MsvcCompilerSettings WithWinRTAssembly(params NPath[] value) => this.With(c => c.WinRTAssemblies = c.WinRTAssemblies.Concat(value).ToArray());
        public MsvcCompilerSettings WithUnicode(bool value) => this.With(c => c.Unicode = value);
        public MsvcCompilerSettings WithStaticStandardLibrary(bool value) => this.With(c => c.UseStaticRuntimeLibrary = value);

        public MsvcCompilerSettings(CLikeCompiler compiler) : base(compiler)
        {
        }

        public override string[] OutputToSwallowFor(CLikeCompilationUnitInstruction cLikeCompilationUnitInstruction)
        {
            //Visual studio will print out the filename of what it is compiling, wether you like it or not. we don't like it, so we tell the backend
            //that if the output exactly matches the filename, and only that, that it should swallow the output.
            return new[] {$"^{cLikeCompilationUnitInstruction.InputFile.FileName}\\s*$"};
        }

        public sealed override IEnumerable<string> CommandLineInfluencingCompilationFor(
            NPath outputObjectFile,
            NPath inputCppFile,
            NPath createOutputPchAt,
            NPath headerToCreateOutputPCHFrom)
        {
            foreach (var a in CommandLineInfluencingCompilationForInternal(
                outputObjectFile,
                inputCppFile,
                createOutputPchAt,
                headerToCreateOutputPCHFrom))
            {
                if (a.Substring(1).Contains("/"))
                    throw new ArgumentException("Not allowed to pass paths with forward slashes in them, because the VS" +
                        "linker once refused to incrementally link whenever you passed a path with forward slashes, and now" +
                        " we're afraid: " + a);
                yield return a;
            }
        }

        protected virtual IEnumerable<string> CommandLineInfluencingCompilationForInternal(
            NPath outputObjectFile,
            NPath inputCppFile,
            NPath createOutputPchAt,
            NPath headerToCreateOutputPCHFrom)
        {
            // Specify source file first so it's easy to identify which file is being compiled in the task manager

            if (InputPCH != null)
            {
                if (OutputPCH == null)
                    throw new ArgumentException("input pch is not null but output pch is null!");
                if (createOutputPchAt != null)
                    throw new ArgumentException("You cannot create an output pch and use a pch at the same time");
                yield return $"/FI{InputPCH.MakeAbsolute().InQuotes(SlashMode.Native)}";
                yield return $"/Yu{InputPCH.MakeAbsolute().InQuotes(SlashMode.Native)}";
                yield return $"/Fp{OutputPCH.MakeAbsolute().InQuotes(SlashMode.Native)}";
            }
            else
            {
                if (OutputPCH != null)
                    throw new ArgumentException("input pch is null but output pch is not null!");
            }

            if (Toolchain.Architecture is ARMv7Architecture)
                yield return "/D__arm__";

            if (createOutputPchAt != null)
            {
                yield return $"/FI{headerToCreateOutputPCHFrom.ToString(SlashMode.Native)}";
                yield return $"/Yc{headerToCreateOutputPCHFrom.ToString(SlashMode.Native)}";
                yield return $"/Fp{createOutputPchAt.ToString(SlashMode.Native)}";
            }

            // Absolute path enables double clicking error messages in Visual Studio
            yield return inputCppFile.MakeAbsolute().InQuotes(SlashMode.Native);

            yield return FlagFor(GetStandardLibrary());

            //create only an .obj file, do not try to also link
            yield return "/c";
            yield return "/bigobj";

            //Use 2 compiler threads instead of 1 or the default 4; this was measured long ago to be the best option
            yield return "/cgthreads2";

            if (Optimization == OptimizationLevel.None)
            {
                yield return "/Od";

                // Enable run-time error checks
                yield return "/RTC1";
            }
            else
            {
                yield return (Optimization == OptimizationLevel.Speed || Optimization == OptimizationLevel.SpeedMax) ? "/Ox" : "/O1";

                // Package global data in individual COMDAT sections, so they could be stripped by linker individually
                yield return "/Gw";

                // Enable whole program optimization
                if (CodeGen == CodeGen.Master)
                    yield return "/GL";

                // Enhance Optimized Debugging
                yield return "/Zo";

                if (Compiler.Architecture is x86Architecture)
                {
                    // We don't want to omit frame pointers of x86, even in non-debug
                    yield return "/Oy-";
                }
            }

            if (Language == Language.C)
                yield return "/TC";
            if (Language == Language.Cpp)
                yield return "/TP";

            switch (FloatingPoint)
            {
                case FloatingPointOptimization.Default: break; // nothing
                case FloatingPointOptimization.Fast: yield return "/fp:fast"; break;
                default: throw new ArgumentOutOfRangeException();
            }

            //exception handling
            if (Exceptions)
                yield return "/EHsc";
            else
                yield return "/D_HAS_EXCEPTIONS=0";

            if (RTTI)
                yield return "/GR";
            else
                yield return "/GR-";

            // Package functions in individual COMDAT sections, so they could be stripped by linker individually
            // It also allows linker to reorganize functions individually which is important on ARM since their branch
            // instructions have limited range.
            yield return "/Gy";

            //dont litter stdout
            yield return "/nologo";

            if (CodeGen != CodeGen.Debug)
            {
                //eliminate duplicate strings
                yield return "/GF";
            }

            //debug info
            yield return "/Zi";

            if (Toolchain.Sdk.Version.Major >= 15)
                //When using /Zi, /Zf is an as-yet-undocumented (04/2018) feature that optimizes RPC calls to
                //mspdbsrv.exe. It seems to speed up our builds by 20-30% (more speedup for more cores,
                //I surmise, because the more cores, the more contention and traffic through mspdbsrv.exe)
                yield return "/Zf";

            // Don't use buffer security checks
            yield return "/GS-";


            yield return "/W3";
            foreach (var wap in WarningPolicies)
            {
                switch (wap.Policy)
                {
                    case WarningPolicy.Silent:
                        yield return $"/wd{wap.Warning}";
                        break;
                    case WarningPolicy.AsWarning:
                        yield return $"/w3{wap.Warning}";
                        break;
                    case WarningPolicy.AsError:
                        yield return $"/we{wap.Warning}";
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }

            if (Unicode)
            {
                yield return "/DUNICODE";
                yield return "/D_UNICODE";
            }
            else
            {
                yield return "/DMBCS";
                yield return "/D_MBCS";
            }

            foreach (var define in Defines) yield return $"/D{define}";
            foreach (var includeDir in IncludeDirectories) yield return $"/I{includeDir.MakeAbsolute().InQuotes(SlashMode.Native)}";

            // put system includes last
            foreach (var includeDir in Toolchain.Sdk.IncludePaths)
                yield return $"/I{includeDir.MakeAbsolute().InQuotes(SlashMode.Native)}";

            /*
             * it turns out that when you use pch's with visual studio and /Zi, three things are true:
             * - each .obj has to use the same pdb as the pch that it's compiled with, or it will error out
             * - no two compiled pch's can use the same pdb, or mspdbsrv.exe will intermittently fail to open the
             * pdb file and you will go insane
             * - you can use the same pch for cpp files that have different defines, but the behavior will be incorrect
             * if the pch ever depends on the different defines.
             *
             * so! we make multiple pch's as we do on all other platforms, and then here, we make exactly one pdb for
             * each pch, and then everything works.
             */
            var PCHNameForPDB = OutputPCH ?? createOutputPchAt;
            var specializedPDBName = PCHNameForPDB != null
                ? PDB.Parent
                    .Combine(PDB.FileNameWithoutExtension + "-" + PCHNameForPDB.FileNameWithoutExtension.Split('-')[1])
                    .ChangeExtension("pdb")
                    .MakeAbsolute()
                    .InQuotes(SlashMode.Native)
                : PDB.MakeAbsolute().InQuotes(SlashMode.Native);

            yield return
                $"/Fd{specializedPDBName}";

            if (OutputASM)
                yield return $"/Fa{outputObjectFile.ChangeExtension("s").MakeAbsolute().InQuotes(SlashMode.Native)}";

            yield return "/FS";

            if (CLikeCompiler.VerbosePerfOutput)
            {
                // https://blogs.msdn.microsoft.com/vcblog/2018/01/04/visual-studio-2017-throughput-improvements-and-advice/
                yield return "/Bt";
                yield return "/d2cgsummary";
                yield return "/showIncludes";
            }

            yield return $"/Fo{outputObjectFile.MakeAbsolute().InQuotes(SlashMode.Native)}";
        }

        private string FlagFor(StandardLibrary standardLibrary)
        {
            switch (standardLibrary)
            {
                case StandardLibrary.MultiThreadedDebug:
                    return "/MTd";
                case StandardLibrary.MultiThreaded:
                    return "/MT";
                case StandardLibrary.MultiThreadedDebugDll:
                    return "/MDd";
                case StandardLibrary.MultiThreadedDll:
                    return "/MD";
                default:
                    throw new ArgumentOutOfRangeException(nameof(standardLibrary), standardLibrary, null);
            }
        }

        private StandardLibrary GetStandardLibrary()
        {
            if (UseStaticRuntimeLibrary)
                return CodeGen == CodeGen.Debug ? StandardLibrary.MultiThreadedDebug : StandardLibrary.MultiThreaded;
            else
                return CodeGen == CodeGen.Debug ? StandardLibrary.MultiThreadedDebugDll : StandardLibrary.MultiThreadedDll;
        }

        protected override CLikeCompilerSettings CompilerSpecificCompilerSettingsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            NPath file,
            NPath artifactPath)
        {
            var settings = (MsvcCompilerSettings)base.CompilerSpecificCompilerSettingsFor(nativeProgram, config, file, artifactPath);
            settings = settings
                .WithCodegen(config.CodeGen)
                .WithPDB(artifactPath.Combine("DebugInfoForAllObjectFiles.pdb"))
                .WithDefines(new[]
                {
                    "_HAS_ITERATOR_DEBUGGING=0",
                    "_SECURE_SCL=0",
                    "_CRT_SECURE_NO_DEPRECATE",
                    "_SCL_SECURE_NO_DEPRECATE",
                    "_WINSOCK_DEPRECATED_NO_WARNINGS",
                    "_CRT_NONSTDC_NO_DEPRECATE",
                });

            if (Compiler.Architecture.Bits == 64)
                settings = settings.WithDefines(new[] { "_WIN64" });

            if (Compiler.Architecture is x64Architecture)
                settings = settings.WithDefines(new[] { "_AMD64_" });

            if (Compiler.Architecture is Arm64Architecture)
                settings = settings.WithDefines(new[] { "_ARM64_" });

            var winRT = nativeProgram.WinRTExtensions.For(config);
            if (winRT)
                settings = settings.WithWinRTExtensions(true);
            var winRTAssemblies = nativeProgram.WinRTAssemblies.For(config).ToArray();
            if (winRTAssemblies.Any())
                settings = settings.WithWinRTAssembly(winRTAssemblies);

            return settings;
        }
    }
}
