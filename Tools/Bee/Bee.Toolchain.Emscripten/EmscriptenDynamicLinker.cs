using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Emscripten
{
    public sealed class EmscriptenDynamicLinker : DynamicLinker
    {
        public override bool DynamicLibrary { get; set; } // currently ignored

        internal bool NoExitRuntime { private get; set; }
        internal bool SeperateAsm { private get; set; }
        internal bool MainModule { private get; set; }
        internal bool MemoryInitFile { private get; set; }
        internal bool EmitSymbolMap { private get; set; }
        internal string[] ExtraExportedRuntimeMethods { private get; set; } = Array.Empty<string>();
        internal string DebugLevel { private get; set; } = "2";
        internal string OptLevel { private get; set; } = "3";
        internal int LinkTimeOptLevel { private get; set; } = 0;
        internal string[] JSTransforms { private get; set; } = Array.Empty<string>();

        internal Dictionary<string, string> EmscriptenSettings { get; set; } = new Dictionary<string, string>
        {
            { "PRECISE_F32", "2" },
            { "DISABLE_EXCEPTION_CATCHING", "0" },
            { "ASSERTIONS", "1" },
            { "USE_WEBGL2", "1" },
            { "MEMFS_APPEND_TO_TYPED_ARRAYS", "1" },
            { "LEGACY_GL_EMULATION", "0" },
            { "ERROR_ON_UNDEFINED_SYMBOLS", "1" },
        };

        // these are LinkerSettings that have dedicated properties, and thus shouldn't
        // be set via LinkerSettings
        internal static HashSet<string> BlockedLinkerSettings { get; } = new HashSet<string>
        {
            "EXTRA_EXPORTED_RUNTIME_METHODS",
            "NO_EXIT_RUNTIME",
            "MAIN_MODULE",
            "BINARYEN",
        };

        protected override Dictionary<string, string> EnvironmentVariables => ((EmscriptenSdk)Toolchain.Sdk).EnvironmentVariables;

        public EmscriptenDynamicLinker(ToolChain toolChain) : base(toolChain)
        {
        }

        protected override IEnumerable<NPath> OutputFilesFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var path in base.OutputFilesFor(destination, codegen, objectFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return path;
            if (MemoryInitFile)
                yield return destination + ".mem";
            if (EmitSymbolMap)
                yield return destination + ".symbols";
            if (SeperateAsm)
                yield return destination.Parent.Combine(destination.FileNameWithoutExtension + ".asm.js");
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles, IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            // sanity check
            var badSettings = EmscriptenSettings.Keys.Intersect(BlockedLinkerSettings).ToArray();
            if (badSettings.Any())
            {
                throw new InvalidOperationException($"Emscripten LinkerSettings contains setting for which a dedicated parameter exists: {badSettings.SeparateWithComma()}");
            }

            yield return ((EmscriptenArchitecture)Toolchain.Architecture).EmccLinkerOptions;

            if (ExtraExportedRuntimeMethods.Length > 0)
                yield return $"-s \"EXTRA_EXPORTED_RUNTIME_METHODS=['{ExtraExportedRuntimeMethods.SeparateWith("','")}']\"";

            if (MainModule) yield return "-s MAIN_MODULE=1";
            if (SeperateAsm) yield return "--separate-asm";

            yield return $"--memory-init-file {(MemoryInitFile ? 1 : 0)}";
            yield return $"-O{OptLevel}";
            yield return $"-g{DebugLevel}";
            yield return $"--llvm-lto {LinkTimeOptLevel}";

            yield return $"-s NO_EXIT_RUNTIME={(NoExitRuntime ? 1 : 0)}";

            if (EmitSymbolMap)
                yield return $"--emit-symbol-map";

            foreach (var flag in EmscriptenSettings)
            {
                yield return $"-s \"{flag.Key}={flag.Value}\"";
            }

            foreach (var lib in allLibraries.Where(l => l is EmscriptenLibrary))
            {
                if (lib is JavascriptLibrary)
                    yield return "--js-library";
                if (lib is PreJsLibrary)
                    yield return "--pre-js";
                if (lib is PostJsLibrary)
                    yield return "--post-js";
                yield return lib.InQuotes();
            }

            foreach (var xform in JSTransforms)
                yield return $"--js-transform '{xform}'";

            foreach (var lib in nonToolchainSpecificLibraries)
                yield return lib.InQuotes();

            foreach (var o in objectFiles)
                yield return o.InQuotes();

            yield return "-o";
            yield return destination.InQuotes();
        }
    }
}
