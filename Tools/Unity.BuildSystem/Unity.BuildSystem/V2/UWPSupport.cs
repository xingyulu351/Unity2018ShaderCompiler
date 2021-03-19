using Bee.Toolchain.UWP;
using JamSharp.Runtime;
using Modules;
using NiceIO;
using Projects.Jam;
using Projects.Jam.Rules;
using System;
using System.Collections.Generic;
using System.Linq;
using Bee.CSharpSupport.Unity;
using Bee.DotNet;
using Bee.NativeProgramSupport.Building.FluentSyntaxHelpers;
using Bee.Toolchain.VisualStudio;
using Bee.Toolchain.Windows;
using External.Compression.lz4;
using External.libjpeg_turbo;
using External.libpng;
using External.zlib;
using Tools.rrw;
using Tools.SerializationWeaver;
using Unity.BuildSystem.Common;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;
using Unity.BuildSystem.VisualStudio.MsvcVersions;
using Unity.BuildTools;

namespace Unity.BuildSystem.V2
{
    public class UWPSupport : PlatformSupport
    {
        public override string[] CommandLinesOfInterest => new[] { "UWP" };

        public override string PlatformName => "UWP";

        public override string PlatformsFolder => "UniversalWindows";

        public override string VariationPrefix => "UWPPlayer";

        protected override string EditorExtensionSourceDirectory => @"PlatformDependent\MetroPlayer\Extensions\Managed";

        protected override string EditorExtensionAssembly => "UnityEditor.WSA.Extensions.dll";

        protected override string EditorExtensionBuildTarget => "UWPEditorExtensions";

        protected override string LegacyTargetName => "MetroSupport";

        protected override string PlatformIdentifierForBindingsGenerator => "Metro_IL2CPP";
        protected override bool SupportPerformanceTests => false;
        protected override NPath BuildFolder { get; } = "build/MetroSupport";
        protected override NPath UnityEngineLocationFor(UnityPlayerConfiguration variation)
        {
            return BuildFolder.Combine("Managed")
                .Combine(variation.ScriptingBackend.ToString().ToLower())
                .Combine("UnityEngine.dll");
        }

        public static UWPSupport Instance { get; private set; }

        public UWPSupport()
        {
            Instance = this;
        }

        protected override string[] PlatformDependentFolders => new[]
        {
            @"MetroPlayer",
            @"MetroPlayer\Bindings",
            @"MetroPlayer\HID",
            @"MetroPlayer\NewInput",
            @"MetroPlayer\Tests",
            @"MetroPlayer\Threads",
            @"MetroPlayer\UI",
            @"MetroPlayer\WebCam",
            @"MetroPlayer\XamlPages"
        };

        protected override IEnumerable<string> Defines =>
            TargetPlatformRules.GetPlatformDefines("PLATFORM_METRO").Elements
                .Append("PLATFORM_METRO=1")
                .Append("PLATFORM_WINRT=1")
                .Append("ENABLE_HOLOLENS_MODULE")
                .Append("ENABLE_HOLOLENS_MODULE_API")
                .Append("SUPPORT_MULTIPLE_DISPLAYS")
                .Append("ENABLE_RUNTIME_GI")
                .Append("UNITY_SYSTEMINFO_OVERRIDE_GETDEVICETYPE")
                .Append("_WINDLL");

        protected override string PlayerBinaryName => "UnityPlayer";

        protected override JamList CsharpPlatformDefines =>
            new JamList(
                TargetPlatformRules.GetCsharpPlatformDefines("PLATFORM_METRO")
                    .Elements
                    .Append("UNITY_WINRT_API")
                    .Append("UNITY_METRO_NATIVE_COMPILER"));

        protected override string[] ModuleBlacklist { get; } = { "ClusterRenderer", "ClusterInput" };

        protected override IEnumerable<UnityPlayerConfiguration> ProvideVariations()
        {
            yield return new UnityPlayerConfiguration(CodeGen.Debug, UWPToolChainX64, true, ScriptingBackend.IL2CPP,
                developmentPlayer: true, enableUnitTests: true);
        }

        protected override void SetupModule(ModuleBase module)
        {
            base.SetupModule(module);
            switch (module.Name)
            {
                case "Video":
                    module.Cpp.Add(c => c.ToolChain.Architecture is IntelArchitecture, UnityAsmUtilsLib.x86Andx64Files);
                    module.Cpp.Add(c => c.ToolChain.Architecture is x64Architecture, UnityAsmUtilsLib.x64Files);
                    break;
            }
        }

        protected override CSharpProgram SetupEditorExtension()
        {
            var program = base.SetupEditorExtension();
            program.Defines.Add("PLATFORM_METRO", "PLATFORM_WINRT", "UNITY_METRO", "UNITY_WINRT");
            program.Unsafe = true;
            program.SystemReferences.Add("System.Xml.Linq");
            program.PrebuiltReferences.Add(Cecil.Current35Ref);

            program.Sources.Add(@"PlatformDependent\Win\Extensions\Managed".ToNPath().Files("*.cs"));
            program.Sources.Add(
                @"Tools\Unity.NuGetPackageResolver\NuGetAssemblyResolver.cs",
                @"Tools\Unity.NuGetPackageResolver\SearchPathAssemblyResolver.cs");

            var np = new NativeProgram("UnityEditor.WSA.Extensions.Native.dll");

            np.Sources.Add(new NPath("PlatformDependent/MetroPlayer/Extensions/Native").Files(new[] {"cpp", "c", "cc"}));
            np.PrebuiltLibraries.Add(new SystemLibrary[] {"advapi32.lib", "ole32.lib"});
            np.OutputDirectory.Set(BuildFolder.Combine("x86_64"));
            np.CompilerSettings().Add(c => c.WithExceptions(true) /*.WithOptimizationLevel(OptimizationLevel.None)*/);

            var toolChain = (VisualStudioToolchain) new WindowsToolChainForHostProvider().Provide();
            new JamTarget(program.Path).DependOn(
                np.SetupSpecificConfiguration(
                    new NativeProgramConfiguration(CodeGen.Debug,
                        toolChain,
                        true),
                    new WindowsDynamicLibraryFormat(toolChain).WithLinkerSetting<WindowsDynamicLibraryFormat>(l =>
                        l.WithSubSystemType(SubSystemType.Windows))));

            return program;
        }

        private static UWPToolchain UWPToolChainX64 { get; } = new UWPToolchain(UWPSdk.Locatorx64.UserDefaultOrDummy);
        private static UWPToolchain UWPToolChainX86 { get; } = new UWPToolchain(UWPSdk.Locatorx86.UserDefaultOrDummy);
        //private static UWPToolchain UWPToolChainARM { get; } = new UWPToolchain(MsvcInstallation.Installations.OfType<Msvc15Installation>().Single(), new ARMv7Architecture());

        protected override void SetupVariation(UnityPlayerConfiguration variation)
        {
            var targetDirectory = BuildFolder.Combine(
                $@"Players\UAP\il2cpp\{GetCpuArchitectureName(variation.ToolChain.Architecture)}\{variation.CodeGen.ToString().ToLower()}");
            var specificConfiguration = PlayerProgram.SetupSpecificConfiguration(
                variation,
                ((UWPDynamicLibraryFormat)variation.ToolChain.DynamicLibraryFormat),
                targetDirectory: targetDirectory);
            var variationVirtualTarget = VariationVirtualTargetFor(variation);
            variationVirtualTarget.DependOn(specificConfiguration);

            variationVirtualTarget.DependOn(Copy.Setup(new NPath("Runtime/Resources/builds/resources_uwp"),
                BuildFolder.Combine("Data/Resources/unity default resources")));

            var logos = new[]
            {
                // UWP logos/splashscreen
                "UWP/SplashScreen.scale-200.png",
                "UWP/Square44x44Logo.scale-200.png",
                "UWP/Square44x44Logo.targetsize-24_altform-unplated.png",
                "UWP/Square150x150Logo.scale-200.png",
                "UWP/StoreLogo.scale-100.png",
                "UWP/Wide310x150Logo.scale-200.png",
            };
            foreach (var logo in logos)
            {
                variationVirtualTarget.DependOn(
                    Copy.Setup(
                        new NPath($"PlatformDependent/MetroPlayer/Resources/{logo}"),
                        BuildFolder.Combine($"Images/{logo}")));
            }
            if (!ProjectFiles.IsRequested)
            {
                variationVirtualTarget.DependOn(Copy.Setup(
                    new NPath("External/NuGet/builds/NuGet.exe"),
                    BuildFolder.Combine("Tools/nuget.exe")));
                variationVirtualTarget.DependOn(
                    Copy.Setup(
                        new NPath("PlatformDependent/MetroPlayer/Templates/UWP_.NET/project.json"),
                        BuildFolder.Combine("Tools/project.json")));
            }

            SetupCopyOfFolder(
                new NPath("PlatformDependent/MetroPlayer/Templates"),
                BuildFolder,
                variationVirtualTarget);
            var rrwAssembly = ReferenceRewriter.Program.Value.SetupSpecificConfiguration(
                new CSharpProgramConfiguration(CSharpCodeGen.Release, new UnityVersionedCsc()));

            variationVirtualTarget.DependOn(
                rrwAssembly.DeployTo(BuildFolder.Combine("Tools/rrw").ToString(SlashMode.Native)));


            var swAssembly = SerializationWeaver.Program.Value.SetupSpecificConfiguration(
                new CSharpProgramConfiguration(CSharpCodeGen.Release, new UnityVersionedCsc()));
            Console.WriteLine("swassembly path " + swAssembly.Path);
            variationVirtualTarget.DependOn(
                swAssembly.DeployTo(BuildFolder.Combine("Tools/SerializationWeaver").ToString(SlashMode.Native)));

            if (variation.CodeGen != CodeGen.Master && variation.ToolChain.Architecture is x64Architecture)
            {
                var winpix = new NPath("External/WinPIXEventRuntime/builds/bin/WinPixEventRuntime_UAP.dll");
                variationVirtualTarget.DependOn(Copy.Setup(
                    winpix,
                    targetDirectory.Combine(winpix.FileName)));
            }

            /*foreach (var module in Modules)
            {
                var moduleAssembly = FinalModuleAssemblyLocationFor(variation, module);
                variationVirtualTarget.DependOn(Copy.Setup(moduleAssembly, BuildFolder.Combine("Tools").Combine(moduleAssembly.FileName)));
                var mdbPath = new NPath(moduleAssembly + ".mdb");
                variationVirtualTarget.DependOn(Copy.Setup(mdbPath, BuildFolder.Combine("Tools").Combine(mdbPath.FileName)));
            }

            var enginePath = UnityEngineLocationFor(variation);
            variationVirtualTarget.DependOn(Copy.Setup(enginePath, BuildFolder.Combine("Tools").Combine(enginePath.FileName)));
            var enginemdb = new NPath(enginePath + ".mdb");
            variationVirtualTarget.DependOn(Copy.Setup(enginemdb, BuildFolder.Combine("Tools").Combine(enginemdb.FileName)));
*/

            foreach (var p in new NPath[]
                 {
                     "PlatformDependent/MetroPlayer/MetroPlayerRunner/MetroPlayerRunner/bin/MetroPlayerRunner.exe",
                     "PlatformDependent/MetroPlayer/WindowsPhonePlayerRunner/WindowsPhonePlayerRunner/bin/Release/WindowsPhonePlayerRunner.exe",
                     "PlatformDependent/MetroPlayer/MetroPlayerRunner/MetroPlayerRunner/bin/Runner.dll"
                 })
                variationVirtualTarget.DependOn(
                    Copy.Setup(
                        p,
                        BuildFolder.Combine("Tools").Combine(p.FileName)));


            foreach (var p in Cecil.Current35AllRefs)
                variationVirtualTarget.DependOn(Copy.Setup(p, BuildFolder.Combine("Tools").Combine(p.FileName)));
        }

        protected override void PostProcessNativeProgram(NativeProgram app)
        {
            base.PostProcessNativeProgram(app);
            app.DynamicLinkerSettingsForUWP().Add(l => l.WithGenerateWindowsMetadata());


            app.IncludeDirectories.Add(@"PlatformDependent\MetroPlayer");
            app.IncludeDirectories.Add(@"PlatformDependent\Win");
            app.IncludeDirectories.Add(c => $@"External\FMOD\builds\{GetJamPlatformName(c.ToolChain.Architecture)}\include");
            app.IncludeDirectories.Add(@"External\Enlighten\builds\LibSrc");

            app.Exceptions.Set(true);
            app.CompilerSettingsForMsvc().Add(s => s.WithWinRTExtensions(true));
            app.PerFilePchs.Add(
                (NPath)null,
                new NPath("PlatformDependent/WinRT/DebuggingTools").Files()
                    .Append("Runtime/Diagnostics/StacktraceTests.cpp")
                    .Append("Runtime/Diagnostics/Stacktrace.cpp")
                    .Append("PlatformDependent/Win/StackWalker.cpp"));


            app.Sources.Add(
                @"External\HashFunctions\sha1.cpp",
                @"External\HashFunctions\sha1.h",
                @"PlatformDependent\Win\Bindings\SpeechBindings.h",
                @"PlatformDependent\Win\KeyboardBindings.h",
                @"PlatformDependent\WinRT\ApplicationTrial.cpp",
                @"PlatformDependent\WinRT\ApplicationTrial.h",
                @"PlatformDependent\WinRT\FileWinHelpers.cpp",
                @"PlatformDependent\WinRT\FileWinHelpers.h",
                @"PlatformDependent\WinRT\LocalFileSystemWinRT.cpp",
                @"PlatformDependent\WinRT\LocalFileSystemWinRT.h",
                @"PlatformDependent\WinRT\LocationService.cpp",
                @"PlatformDependent\WinRT\NetworkManagerWinRT.cpp",
                @"PlatformDependent\WinRT\NativePlugins.cpp",
                @"PlatformDependent\WinRT\NativePlugins.h",
                @"PlatformDependent\WinRT\Resources\Version.rc",
                @"PlatformDependent\WinRT\Sensors.cpp",
                @"PlatformDependent\WinRT\Sensors.h",
                @"PlatformDependent\WinRT\SensorsAcceleration.cpp",
                @"PlatformDependent\WinRT\SensorsAcceleration.h",
                @"PlatformDependent\WinRT\SensorsGyrometer.cpp",
                @"PlatformDependent\WinRT\SensorsGyrometer.h",
                @"PlatformDependent\WinRT\SensorsOrientation.cpp",
                @"PlatformDependent\WinRT\SensorsOrientation.h",
                @"Runtime\Input\LocationService.h",
                @"Runtime\Input\SimulateInputEvents.cpp",
                @"Runtime\Input\SimulateInputEvents.h");

            app.Sources.Add(ConvertedJamFile.Vars.RUNTIME_SOURCES_MS_COMMON.ToNPaths());
            app.Sources.Add(RuntimeFiles.SourcesGfxD3D11.ToNPaths());
            app.Sources.Add(RuntimeFiles.SourcesGfxD3D12.ToNPaths());
            app.Sources.Add(RuntimeFiles.SourcesGfxThreaded.ToNPaths());
            app.Sources.Add(RuntimeFiles.SourcesCommonWeb.ToNPaths());
            app.Sources.Add(RuntimeFiles.SourcesNoPCH.ToNPaths());
            app.Sources.Add(RuntimeFiles.SourcesGI.ToNPaths());

            app.Sources.Add(IsDebugBuild,
                @"PlatformDependent\Win\StackWalker.h",
                @"PlatformDependent\Win\StackWalkerImpl.h",
                @"PlatformDependent\Win\StackWalker.cpp",
                @"PlatformDependent\Win\StackWalkerTests.cpp",
                @"PlatformDependent\WinRT\DebuggingTools\CrashHandler.cpp",
                @"PlatformDependent\WinRT\DebuggingTools\DbgTools.cpp",
                @"PlatformDependent\WinRT\DebuggingTools\DbgTools.h",
                @"PlatformDependent\WinRT\DebuggingTools\DbgToolsIncludes.h");

            app.Sources.Add(IsDebugBuild, RuntimeFiles.SourcesDiagnostics.ToNPaths());

            var unitTestPlusPlusSources = UnitTestPlusPlus.GetSourceFiles(true)
                .ToNPaths()
                .Select(src => @"External\UnitTest++\src".ToNPath().Combine(src));
            app.Sources.Add(IsDevelopmentBuild, unitTestPlusPlusSources);

            app.NonLumpableFiles.Add(NonLumpableFiles.NonLumpableFilesUWP);

            // Extract the build number from SDK version, with numbers a number like 10.0.10586.0
            string sdkVersion = ((VisualStudioSdk)UWPToolChainX64.Sdk).Installation.Windows10Sdk.Version;

            //WindowsSDKs.GetWindows10SDKDirectory(out sdkVersion);

            var splitSdkVersion = sdkVersion.Split('.');
            if (splitSdkVersion.Length < 3)
                throw new InvalidOperationException("Unrecognized Windows SDK: " + sdkVersion);

            var sdkVersionDefine = splitSdkVersion[2];
            app.Defines.Add($"UNITY_WINSDKVERSION={sdkVersionDefine}");

            // PixEventRuntime
            app.Defines.Add(UsePix, "USE_PIX");
            app.PrebuiltLibraries.Add(
                UsePix,
                new StaticLibrary(@"External\WinPIXEventRuntime\builds\bin\WinPixEventRuntime_UAP.lib"));

            app.PrebuiltLibraries.Add(c =>
            {
                var cpu = GetCpuArchitectureName(c.ToolChain.Architecture);
                var jamPlatformName = GetJamPlatformName(c.ToolChain.Architecture);
                var staticLibs = new List<NPath>();

                NPath opensslLibsFolder = $@"PlatformDependent\MetroPlayer\External\OpenSSL\builds\uwp\{cpu}\lib";
                NPath libcurlLibsFolder = $@"PlatformDependent\MetroPlayer\External\libcurl\builds\uwp\{cpu}\lib";

                // OpenSSL/libcurl
                staticLibs.Add(opensslLibsFolder.Combine("libeay32.lib"));
                staticLibs.Add(opensslLibsFolder.Combine("ssleay32.lib"));
                staticLibs.Add(libcurlLibsFolder.Combine("libcurl_a.lib"));

                // FMOD
                staticLibs.Add($@"External\FMOD\builds\{jamPlatformName}\lib\libfmodex.lib");

                // PhysX
                staticLibs.AddRange(
                    ConvertedJamFile.Vars.PhysX3Libraries.Select(
                        lib =>
                            $@"PlatformDependent\MetroPlayer\External\PhysX3\builds\{jamPlatformName}\release\lib{lib}.lib"
                                .ToNPath()));

                // Enlighten
                var staticLibConfig = c.CodeGen == CodeGen.Master ? "master" : "release";
                var enlightenLibs = new[] { "Enlighten3.lib", "Enlighten3HLRT.lib", "GeoBase.lib", "GeoCore.lib" };
                staticLibs.AddRange(
                    enlightenLibs.Select(
                        lib =>
                            $@"PlatformDependent\MetroPlayer\External\Enlighten\builds\{jamPlatformName}\{staticLibConfig}"
                                .ToNPath()
                                .Combine(lib)));

                // Theora
                staticLibs.Add($@"External\theora\libs\{jamPlatformName}\{staticLibConfig}\theora_full_static.lib");

                staticLibs.Add("PlatformDependent/WinRT/DebuggingTools/Libs/UWP/amd64/comsuppwd.lib");

                return staticLibs.Select(lib => new StaticLibrary(lib));
            });

            var systemLibraries = new[]
            {
                "d3d11.lib",
                "dxguid.lib",
                "mfuuid.lib"
            };

            app.PrebuiltLibraries.Add(systemLibraries.Select(lib => new SystemLibrary(lib)));
            app.StaticLibraries.Add(lz4.LZ4NativeProgramLibrary);
            app.StaticLibraries.Add(zlib.SetupZlibNativeProgramLibrary.Value);
            app.StaticLibraries.Add(libpng.PngNativeProgramLibrary);
            app.StaticLibraries.Add(libjpeg.LibJpgeNativeProgramLibrary.Value);
            app.StaticLibraries.Add(External.draco.draco.GetNativePrograms());


            app.PerFilePchs.Add(null, app.Sources.ForAny().Where(src => src.HasExtension(".c")));
        }

        protected override NPath FinalModuleAssemblyLocationFor(UnityPlayerConfiguration variation, ModuleBase module)
        {
            var subfolder = variation.ScriptingBackend == ScriptingBackend.IL2CPP ? "il2cpp" : "UAP";
            return BuildFolder.Combine($@"Managed\{subfolder}\{GetModuleDllName(module)}.dll");
        }

        private static bool UsePix(NativeProgramConfiguration c) =>
            c.ToolChain.Architecture is x64Architecture && IsDevelopmentBuild(c);
        private static bool IsDebugBuild(NativeProgramConfiguration c) => c.CodeGen == CodeGen.Debug;

        private static bool IsDevelopmentBuild(NativeProgramConfiguration c) =>
            c.CodeGen == CodeGen.Debug || c.CodeGen == CodeGen.Release;

        private static string GetJamPlatformName(Architecture architecture)
        {
            if (architecture is x64Architecture)
                return "uap_x64";

            if (architecture is x86Architecture)
                return "uap_x86";

            if (architecture is ARMv7Architecture)
                return "uap_arm";

            throw new ArgumentException("Unknown architecture: " + architecture);
        }

        private static string GetCpuArchitectureName(Architecture architecture)
        {
            if (architecture is x64Architecture)
                return "x64";

            if (architecture is x86Architecture)
                return "x86";

            if (architecture is ARMv7Architecture)
                return "ARM";

            throw new ArgumentException("Unknown architecture: " + architecture);
        }

        protected override void CopyVariationManagedAssembliesIntoTopLevelManagedFolder()
        {
            //do nothing, UWP's own variations already build straight into the
            //top level managed folder, so there's no extra copy we have to do
        }
    }
}
