using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using Bee.Core;
using Bee.NativeProgramSupport;
using Bee.NativeProgramSupport.Building;
using Bee.NativeProgramSupport.Building.FluentSyntaxHelpers;
using Bee.Stevedore;
using Bee.Toolchain.Emscripten;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Emscripten
{
    public abstract class EmscriptenArchitecture : Architecture
    {
        public abstract string EmccLinkerOptions { get; }
    }

    public sealed class AsmJsArchitecture : EmscriptenArchitecture
    {
        public override int Bits => 32;
        public override string Name => "asmjs";
        public override string DisplayName => "asmjs";
        public override string EmccLinkerOptions => "-s WASM=0";
    }

    public sealed class WasmArchitecture : EmscriptenArchitecture
    {
        public override int Bits => 32;
        public override string Name => "wasm";
        public override string DisplayName => "wasm";
        public override string EmccLinkerOptions => "-s WASM=1";
    }

    public abstract class EmscriptenLibrary : PrecompiledLibrary
    {
        public override WhenMultiplePolicy WhenMultiplePolicy =>
            Unity.BuildSystem.NativeProgramSupport.WhenMultiplePolicy.KeepFirst;
    }

    public sealed class JavascriptLibrary : EmscriptenLibrary
    {
        public override bool Static => true;
        public JavascriptLibrary(NPath path) => _Path = path;
    }

    public sealed class PreJsLibrary : EmscriptenLibrary
    {
        public override bool Static => true;
        public PreJsLibrary(NPath path) => _Path = path;
    }

    public sealed class PostJsLibrary : EmscriptenLibrary
    {
        public override bool Static => true;
        public PostJsLibrary(NPath path) => _Path = path;
    }

    public class EmscriptenSdk : Sdk
    {
        public static EmscriptenSdkLocator Locator { get; }  = new EmscriptenSdkLocator();

        public override bool SupportedOnHostPlatform => true;

        protected virtual NPath Emcc { get; }
        protected virtual string EmccCommand { get; }

        readonly NPath m_CachePath;
        readonly NPath m_ConfigPath;
        readonly NPath m_EmscriptenRoot;
        readonly NPath m_LlvmRoot;
        public NPath NodeExe { get; }
        readonly NPath m_OptimizerExe;
        readonly NPath m_PythonExe;

        protected EmscriptenSdk()
        {
        }

        public EmscriptenSdk(NPath emscriptenRoot, NPath llvmRoot, NPath pythonExe, NPath nodeExe, Version version, bool isDownloadable = false, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(backendRegistrables)
        {
            IsDownloadable = isDownloadable;
            Path = emscriptenRoot; // TODO: no single path defines an EmscriptenSdk
            Version = version;

            var emcc = Emcc = emscriptenRoot.Combine("emcc.py");

            m_EmscriptenRoot = emscriptenRoot;
            m_LlvmRoot = llvmRoot;
            m_OptimizerExe = llvmRoot.Combine("optimizer" + HostPlatform.Exe);
            m_PythonExe = pythonExe;
            NodeExe = nodeExe;

            EmccCommand = new[] { pythonExe.InQuotes(SlashMode.Native), emcc.InQuotes(SlashMode.Native)}.SeparateWithSpace();

            // Move the emscripten cache to the build artifacts instead of ~/.em_cache
            m_CachePath = new NPath("artifacts/emscripten-cache-" + version);

            // Use our custom EM_CONFIG file instead of ~/.emscripten
            m_ConfigPath = new NPath("artifacts/emscripten-config-" + version + ".py");

            TundraInputs = new[]
            {
                m_ConfigPath,
                emcc,
                nodeExe,
                m_OptimizerExe,
                pythonExe
            };
        }

        public override void Register(IBackend backend)
        {
            base.Register(backend);

            var Q = new Func<NPath, string>(path => "'" + path.MakeAbsolute().ToString(SlashMode.Native).Replace("\\", "\\\\").Replace("'", "\\'") + "'");

            // must determine config on-demand, as working dir might otherwise be wrong
            backend.AddWriteTextAction(m_ConfigPath, $@"
BINARYEN_ROOT = {Q(m_LlvmRoot.Combine("binaryen"))}
EMSCRIPTEN_ROOT = {Q(m_EmscriptenRoot)}
EMSCRIPTEN_NATIVE_OPTIMIZER = {Q(m_OptimizerExe)}
LLVM_ROOT = {Q(m_LlvmRoot)}
NODE_JS = {Q(NodeExe)}
PYTHON = {Q(m_PythonExe)}

COMPILER_ENGINE = NODE_JS
JS_ENGINES = [NODE_JS]

# (the above may also need JAVA='...' for the Closure compiler if we use it... probably desirable for release)
");
        }

        // must calculate these on-demand, as working dir might otherwise be wrong
        public virtual Dictionary<string, string> EnvironmentVariables => new Dictionary<string, string>()
        {
            { "EM_CACHE", m_CachePath.MakeAbsolute().ToString(SlashMode.Native) },
            { "EM_CONFIG", m_ConfigPath.MakeAbsolute().ToString(SlashMode.Native) },
        };

        public override IEnumerable<NPath> TundraInputs { get; }

        public override Platform Platform { get; } = new WebGLPlatform();
        public override NPath Path { get; }
        public override Version Version { get; }

        public override NPath CppCompilerPath => Emcc;
        public override NPath StaticLinkerPath => Emcc;
        public override NPath DynamicLinkerPath => Emcc;

        public override string CppCompilerCommand => EmccCommand;
        public override string StaticLinkerCommand => EmccCommand;
        public override string DynamicLinkerCommand => EmccCommand;
    }

    public class EmscriptenStaticLibraryFormat : NativeProgramFormat
    {
        public override string Extension => "bc";

        public EmscriptenStaticLibraryFormat(ToolChain toolchain, bool bundle) : base(
            new _Linker(toolchain, bundle))
        {
        }

        private sealed class _Linker : StaticLinker
        {
            private bool Bundle { get; set; }

            public _Linker(ToolChain toolchain, bool bundle) : base(toolchain)
            {
                Bundle = bundle;
            }

            protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
                IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
            {
                foreach (var o in objectFiles)
                    yield return o.InQuotes();

                if (Bundle)
                {
                    foreach (var l in nonToolchainSpecificLibraries)
                        yield return l.InQuotes();
                }

                yield return "-o";
                yield return destination.InQuotes();
            }

            protected override Dictionary<string, string> EnvironmentVariables => ((EmscriptenSdk)Toolchain.Sdk).EnvironmentVariables;
        }
    }

    public class EmscriptenToolchain : ToolChain
    {
        public override int SpeedScore => - 10;

        public override NativeProgramFormat ExecutableFormat { get; }
        public override NativeProgramFormat StaticLibraryFormat { get; }
        public override NativeProgramFormat DynamicLibraryFormat { get; }
        public override CLikeCompiler CppCompiler => new EmscriptenCompiler(this);

        public override string LegacyPlatformIdentifier { get; } = "webgl";

        public EmscriptenToolchain() : this(EmscriptenSdk.Locator.LatestSdk, new AsmJsArchitecture())
        {
        }

        public EmscriptenToolchain(EmscriptenSdk sdk, EmscriptenArchitecture architecture) : base(sdk, architecture)
        {
            ExecutableFormat = new EmscriptenExecutableFormat(this);
            StaticLibraryFormat = new EmscriptenStaticLibraryFormat(this, false);
            DynamicLibraryFormat = new EmscriptenStaticLibraryFormat(this, true); // emulate dynamic library by using a bundled static library
        }
    }

    class DownloadableEmscriptenSdkProvider : DownloadableSdkProvider
    {
        struct SpecificEmscripten
        {
            public StevedoreArtifact EmscriptenArtifact;
            public Version Version;
            public NPath Root;
        }
        public override IEnumerable<Sdk> Provide()
        {
            var emscripten = StevedoreArtifact.Public("emscripten/1.38.3_40e7cde98b9301a0708b00fc6f61238f80a115a23dccb515f9561a0ff26a4654.zip");
            var emscripten2 = StevedoreArtifact.Public("emscripten/1.38.6_11f6ff531ae1eb0ee78618d1292689d52de9095c41e1e978362642949fcba700.7z");

            var specificEmscriptens = new[]
            {
                new SpecificEmscripten()
                {
                    EmscriptenArtifact = emscripten,
                    Version = new Version(1, 38, 3),
                    Root = emscripten.Path.Combine("emscripten-1.38.3")
                },
                new SpecificEmscripten()
                {
                    EmscriptenArtifact = emscripten2,
                    Version = new Version(1, 38, 6),
                    Root = emscripten2.Path.Combine("emscripten-1.38.6")
                }
            };

            foreach (var specificEmscripten in specificEmscriptens)
            {
                if (HostPlatform.IsWindows)
                {
                    var llvm = StevedoreArtifact.Public(
                        "emscripten-llvm-win-x64/e1.38.3_c836dda116bc9d7bf55cf51f8bdfb98a9b6fed74db0aa4ae72a399b33a39e7e9.zip");
                    var python = StevedoreArtifact.Public(
                        "winpython2-x64/2.7.13.1Zero_740e3bbd4c2384963a0944dec446dc36ce7513df2786c243b417b93a2dff851e.zip");
                    var node = StevedoreArtifact.Public(
                        "node-win-x64/8.11.2_8bbd03b041f8326aba5ab754e4619eb3322907ddbfd77b93ddbcdaa435533ce0.7z");
                    yield return new EmscriptenSdk(
                        specificEmscripten.Root,
                        llvmRoot: llvm.Path,
                        pythonExe: python.Path.Combine("WinPython-64bit-2.7.13.1Zero/python-2.7.13.amd64/python.exe"),
                        nodeExe: node.Path.Combine("node-v8.11.2-win-x64/node.exe"),
                        version: specificEmscripten.Version,
                        isDownloadable: true,
                        backendRegistrables: new[] {specificEmscripten.EmscriptenArtifact, llvm, python, node}
                    );
                }

                if (HostPlatform.IsLinux)
                {
                    var llvm = StevedoreArtifact.Public(
                        "emscripten-llvm-linux-x64/e1.38.3_1382a129eb391db1db7aacaa3fa67268fcae81871f553920b2ec39550cddaefe.7z");
                    var node = StevedoreArtifact.Public(
                        "node-linux-x64/8.11.2_a4fccf17e141ddf01dee7fb8e6bf7cd59adb38f86927b7ec6a96d1e455d7197f.7z");
                    yield return new EmscriptenSdk(
                        specificEmscripten.Root,
                        llvmRoot: llvm.Path.Combine("emscripten-llvm-e1.38.3"),
                        pythonExe: "/usr/bin/python2",
                        nodeExe: node.Path.Combine("node-v8.11.2-linux-x64/bin/node"),
                        version: specificEmscripten.Version,
                        isDownloadable: true,
                        backendRegistrables: new[] {specificEmscripten.EmscriptenArtifact, llvm, node}
                    );
                }

                if (HostPlatform.IsOSX)
                {
                    var llvm = StevedoreArtifact.Public(
                        "emscripten-llvm-mac-x64/e1.38.3_19d048b0e8b4617f7fd9f169fd2f4e7848761fcd06bc0499605f70d67fc03405.7z");
                    var node = StevedoreArtifact.Public(
                        "node-mac-x64/8.11.2_3bb1156b6cba0f9d96e78f256d3c88a3dd8b8e38922995874e1fb02863d1abf2.7z");
                    yield return new EmscriptenSdk(
                        emscriptenRoot: specificEmscripten.Root,
                        llvmRoot: llvm.Path.Combine("emscripten-llvm-e1.38.3"),
                        pythonExe: "/usr/bin/python",
                        nodeExe: node.Path.Combine("node-v8.11.2-darwin-x64/bin/node"),
                        version: specificEmscripten.Version,
                        isDownloadable: true,
                        backendRegistrables: new[] {specificEmscripten.EmscriptenArtifact, llvm, node}
                    );
                }
            }
        }
    }

    public sealed class FluentEmscriptenSdk
    {
        private EmscriptenSdk Sdk { get; }
        internal FluentEmscriptenSdk(EmscriptenSdk sdk) => Sdk = sdk;

        public EmscriptenToolchain AsmJS() => new EmscriptenToolchain(Sdk, new AsmJsArchitecture());
    }
}

public static class FluentEmscriptenExtensions
{
    public static FluentEmscriptenSdk Sdk_1_38_3(this FluentPlatform<WebGLPlatform> _) => Find(new Version(1, 38, 3));
    public static FluentEmscriptenSdk Sdk_1_38_6(this FluentPlatform<WebGLPlatform> _) => Find(new Version(1, 38, 6));

    private static FluentEmscriptenSdk Find(Version version) => new FluentEmscriptenSdk(EmscriptenSdk.Locator.FindSdkInSystemOrDownloads(version));
}
