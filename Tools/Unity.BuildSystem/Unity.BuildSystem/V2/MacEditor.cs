using System.Collections.Generic;
using System.Linq;
using Bee.Toolchain.GNU;
using Bee.Toolchain.MacOS.Unity;
using Bee.Toolchain.Xcode;
using External.Jamplus.builds.bin;
using JamSharp.Runtime;
using Modules;
using NiceIO;
using Projects.Jam;
using Projects.Jam.Rules;
using Unity.BuildSystem.Common;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using static Unity.BuildSystem.NativeProgramSupport.NativeProgramConfiguration;

namespace Unity.BuildSystem.V2
{
    class UnityEditorBuildSystemTool : BuildSystemTool
    {
        public NPath EditorBinary { get; }

        public UnityEditorBuildSystemTool(NPath editorBinary)
        {
            EditorBinary = editorBinary;
        }

        public NPath SetupBuildExtraResources()
        {
            // TODO: hacked this because there are multiple actions registered to the same output folder
            var outputFile = $"build/MacEditor2/Unity.app/Contents/Resources/unity_builtin_extra";
            var args = new[] {"-nographics", "-noshadercheck", "-force-gfx-direct", "-buildBuiltinExtras", "-batchmode"};
            base.SetupInvocation(outputFile, EditorBinary, args, EditorBinary.ToString(SlashMode.Native));
            return outputFile;
        }
    }

    public sealed class MacEditor : EditorSupport
    {
        private readonly JamTarget EditorTarget = new JamTarget("MacEditor");
        private readonly JamTarget EditorAppTarget = new JamTarget("MacEditorApp");
        public override string PlatformName { get; } = "MacEditor";
        public override string VariationPrefix { get; } = "MacEditor";
        protected override string LegacyTargetName { get; } = "MacEditor";

        protected override string PlatformIdentifierForBindingsGenerator { get; } = "common";
        protected override string[] ModuleBlacklist { get; } = {"NScreen", "NScreenEditor"};

        // TODO: figure out why some Editor modules are special and need to be blacklisted to avoid RegisterModule_ link errors
        protected override string[] EditorModulesBlacklist { get; } = {"GraphViewEditor", "UIElementsDebuggerEditor", "GridEditor", "SpriteShapeEditor", "SpriteMaskEditor", "UIAutomationEditor" };

        protected override NPath FinalModuleAssemblyLocationFor(UnityPlayerConfiguration variation, ModuleBase module)
        {
            // TODO: hacked this because there are multiple actions registered to the same output folder
            return $"build/MacEditor2/Unity.app/Contents/Managed/UnityEngine/UnityEngine.{module.Name}Module.dll";
        }

        public override void Setup()
        {
            base.Setup();

            var targetAllSupportFilesDependThemselvesOn = new JamTarget("MacEditor");
            BuiltinRules.NotFile(targetAllSupportFilesDependThemselvesOn);
            EditorTarget.DependOn(targetAllSupportFilesDependThemselvesOn);
            EditorTarget.DependOn(EditorAppTarget);
            EditorTarget.DependOn(IvyPackager.DataContract.SetupDefault().Path);

            new JamTarget("MacEditorApp").DependOn(EditorAppTarget);

            var unityBuildSystemTool = new UnityEditorBuildSystemTool("build/MacEditor/Unity.app/Contents/MacOS/Unity");

            var setupBuildExtraResources = unityBuildSystemTool.SetupBuildExtraResources();
            new JamTarget("lucas123").DependOn(setupBuildExtraResources);
            EditorTarget.DependOn(setupBuildExtraResources);
        }

        protected override void SetupModule(ModuleBase module)
        {
            base.SetupModule(module);
            //move this to the modules' code themselves when these modules have been upgraded to new style
            switch (module.Name)
            {
                case "Audio":
                    module.Frameworks.Add(IsMac, "AudioToolbox", "CoreAudio", "MediaToolbox", "AVFoundation", "AudioUnit", "CoreFoundation", "CoreVideo", "Foundation", "CoreMedia");
                    break;
                case "Video":
                    module.Cpp.Add(c => c.ToolChain.Architecture is IntelArchitecture, UnityAsmUtilsLib.x86Andx64Files);
                    module.Cpp.Add(c => c.ToolChain.Architecture is x64Architecture, UnityAsmUtilsLib.x64Files);
                    module.Frameworks.Add(IsMac, "CoreVideo", "AVFoundation", "Foundation", "CoreMedia", "Quartz", "MediaToolbox", "OpenGL");
                    break;
                case "TextRendering":
                    module.Frameworks.Add(IsMac, "CoreFoundation");
                    break;
                case "Physics":
                    module.PrebuiltLibraries.Add(c => IsMac(c) && c.ToolChain.Architecture.Bits == 64, "External/PhysX3/builds/Lib/osx64/release/libLowLevelCloth.a");
                    module.PrebuiltLibraries.Add(c => IsMac(c) && c.ToolChain.Architecture.Bits == 32, "External/PhysX3/builds/Lib/osx32/release/libLowLevelCloth.a");
                    break;
                case "Cloth":
                    module.PrebuiltLibraries.Add(c => IsMac(c) && c.ToolChain.Architecture.Bits == 64,  "External/PhysX3/builds/Lib/osx64/release/libLowLevelCloth.a");
                    module.PrebuiltLibraries.Add(c => IsMac(c) && c.ToolChain.Architecture.Bits == 32,  "External/PhysX3/builds/Lib/osx32/release/libLowLevelCloth.a");
                    ModuleByName("Physics").PrebuiltLibraries.CopyTo(module.PrebuiltLibraries, c => c);
                    break;
                case "Vehicles":
                    ModuleByName("Physics").PrebuiltLibraries.CopyTo(module.PrebuiltLibraries, c => c);
                    break;
            }

            if (module is CoreModule)
            {
                module.Cpp.Add(RuntimeFiles.SourcesGfxNull.ToNPaths());
                module.Cpp.Add("Runtime/VirtualFileSystem/LocalFileSystemPosix.cpp");
                module.Cpp.Add(new NPath("PlatformDependent/OSX/HID_Utilities").Files("*.c"));
                module.Cpp.Add("PlatformDependent/CommonApple/ApplicationInfoApple.mm");
                //module.Cpp.Add(RuntimeFiles.SourcesDiagnostics.ToNPaths());
                module.Cpp.Add(RuntimeFiles.SourcesGI.ToNPaths());

                module.Cpp.Add(RuntimeFiles.SourcesNoPCH.ToNPaths());
                module.Cpp.Add(RuntimeFiles.SourcesGfxGLLegacy.ToNPaths());
                //module.Cpp.Add(RuntimeFiles.SourcesGfxNull.ToNPaths());
                module.Cpp.Add(RuntimeFiles.SourcesGfxThreaded.ToNPaths());
                module.Cpp.Add(RuntimeFiles.SourcesGfxMetal.ToNPaths());
                module.Cpp.Add(RuntimeFiles.SourcesCommonWeb.ToNPaths());
                module.Cpp.Add(RuntimeFiles.SourcesGfxOpenGLES.ToNPaths());
                module.Cpp.Add("PlatformDependent/CommonApple/CommonUtils.mm",
                    "PlatformDependent/CommonApple/HashFunctions.cpp",
                    "PlatformDependent/CommonApple/NSStringConvert.h",
                    "PlatformDependent/CommonApple/NSStringConvert.mm",
                    "PlatformDependent/CommonApple/PlatformSemaphore.cpp",
                    "PlatformDependent/CommonApple/PlayerPrefs.mm",
                    "PlatformDependent/CommonApple/CommonLibEntry.mm",
                    "PlatformDependent/iPhonePlayer/Trampoline/Classes/Unity/MetalHelper.mm");
            }
        }

        protected override IEnumerable<UnityPlayerConfiguration> ProvideVariations()
        {
            yield return new UnityPlayerConfiguration(CodeGen.Debug,
                new UnityMacToolChainForHostProvider().Provide(new x64Architecture(), "10.9"),
                true,
                ScriptingBackend.Mono,
                developmentPlayer: true,
                enableUnitTests: true);
        }

        protected override string[] PlatformDependentFolders { get; } = {"OSX"};

        protected override string PlayerBinaryName { get; } = "Unity";

        protected override JamList CsharpPlatformDefines { get; } =
            TargetPlatformRules.GetCsharpPlatformDefines("UNITY_EDITOR");

        protected override IEnumerable<string> Defines { get; } =
            TargetPlatformRules.GetPlatformDefines("UNITY_EDITOR").Elements
                .Append("UNITY_OSX=1");

        protected override IEnumerable<string> ModuleNames => ConvertedJamFile.Vars.RUNTIME_modules.Distinct();

        protected override void PostProcessNativeProgram(NativeProgram app)
        {
            base.PostProcessNativeProgram(app);

            // be sure to have platform-specific include dir *before* Projects/PrecompiledHeaders
            // this way PlatformDependent/OSX/Source/PlatformPrefixConfigure.h is guaranteed to take precedence
            app.IncludeDirectories.AddFront("PlatformDependent/OSX/Source");

            // TODO: See how we can include this only for Modules/AssetDatabase
            app.IncludeDirectories.Add("External/asio/include");

            //todo: figure out how to do this cleanly
            app.CompilerSettings().Add(s => s.WithSSE(SIMD.avx2), "Editor/Src/AssetPipeline/TextureImporting/ConvolveBRDF_AVX.cpp");

            app.CompilerSettings().Add(s => s.WithExceptions(true));

            app.PerFileIncludeDirectories.Add(new NPath[] { "External/FreeImage/builds"},
                "Editor/Src/AssetPipeline/TextureImporting/ImageConverter.cpp",
                "Editor/Src/GI/Enlighten/EnlightenJobHelpers.cpp");

            app.PerFileIncludeDirectories.Add(
                new NPath[] {"External/pubnub/core", "External/pubnub/cpp", "External/pubnub/openssl"},
                "Editor/Src/Collab/Channel/ChannelListener.cpp",
                "Editor/Src/Collab/Softlocks/CollabChannelListener.cpp");

            app.PerFileIncludeDirectories.Add(new NPath[] {"External/OpenSSL/builds/include"},
                "Editor/Src/AssetPipeline/MdFourGenerator.cpp",
                "Editor/Src/Collab/Collab.cpp",
                "Editor/Src/Collab/CollabCommon.cpp",
                "Editor/Src/Collab/CollabFileSystemOperations.cpp",
                "Editor/Src/Collab/Tests/RadixTreeTests.cpp",
                "Editor/Src/Collab/Tests/CollabTests.cpp",
                "Editor/Src/AssetStore/AssetStoreDownloadMessage.cpp",
                "PlatformDependent/CommonWebPlugin/Verification.cpp");

            foreach (var program in AllNativePrograms)
            {
                program.CompilerSettingsForMac().Add(s =>
                {
                    return s.WithRTTI(true)
                        .WithObjcArc(true)
                        .WithCppLanguageVersion(11);
                });

                program.CompilerSettingsForMac().Add(s => s.WithObjcArc(false),
                    "Editor/Platform/OSX/EditorWindows.mm",
                    "Editor/Platform/OSX/OSXProjectWizardUtilityHelper.mm",
                    "Editor/Platform/OSX/Utility/CocoaEditorUtility.mm",
                    "Editor/Src/Video/Media/AVFoundation/AVFoundationVideoMedia.mm",
                    "Editor/Src/Video/Media/AVFoundation/AVFoundationMediaAttributes.mm",
                    "Editor/Platform/OSX/AssetProgressbar.mm",
                    "Editor/Platform/OSX/EditorPrefsTests.mm",
                    "Editor/Platform/OSX/OSXNativeWebViewWindowHelper.mm",
                    "Editor/Platform/OSX/OSXWebViewWindowPrivate.mm"
                );
            }

            NPath[] filesWithMismatchedTagsWarnings =
            {
                "Editor/Platform/OSX/EditorUtility.mm",
                "Editor/Platform/OSX/EditorWindows.mm",
                "Editor/Platform/OSX/EditorDialogs.mm",
                "Editor/Platform/OSX/AssetProgressbar.mm",
                "Editor/Platform/OSX/Utility/GUIRenderView.mm",
                "Editor/Platform/OSX/RepaintController.mm",
                "Editor/Platform/OSX/Utility/CocoaEditorUtility.mm",
                "Editor/Platform/OSX/Utility/GUIRenderView.mm",
            };

            // tone down these errors as we're not going to fix them now (v1 didn't had warnings as errors!?)
            app.CompilerSettings().Add(s => s.WithWarningPolicies(WarningSettings_GccClang
                .Policies(s.Language, Compiler.Clang)
                .Where(p => p.Policy == WarningPolicy.AsError && p.Warning == "mismatched-tags")
                .Select(p => new WarningAndPolicy(p.Warning, WarningPolicy.AsWarning)).ToArray()
                ), filesWithMismatchedTagsWarnings);

            app.PrebuiltLibraries.Add(new NonSystemFramework(new NPath("External/SketchUp/builds/17.0/lib/macosx64/SketchUpAPI.framework")));

            app.NonLumpableFiles.Add(FilesThatStillHaveWarnings.FilesWithWarningsThatWeAreNotGoingToFix());
        }

        protected override void SetupVariation(UnityPlayerConfiguration variation)
        {
            base.SetupVariation(variation);

            var nativeProgramForEditorModule = NativeProgramForModule(ModuleByName("Editor"));

            EditorAppTarget.DependOn(PlayerProgram.SetupSpecificConfiguration(variation,
                variation.ToolChain.ExecutableFormat,
                targetDirectory: "build/MacEditor/Unity.app/Contents/MacOS"));

            NPath contents = "build/MacEditor/Unity.app/Contents";

            Jambase.ActiveTarget(EditorAppTarget);
            EditorFiles.SetupCopiesForEditorSupportFiles($"{contents}/Tools",
                contents.ToString(),
                "notusedonmac",
                "macosx64");

            // need to copy all non-system frameworks inside the app
            var variationVirtualTarget = VariationVirtualTargetFor(variation);
            var targetDir = contents.Combine("Frameworks");

            foreach (var framework in nativeProgramForEditorModule.PrebuiltLibraries.For(variation).Where(l => l is NonSystemFramework))
            {
                var files = framework.Path.Files(true).Select(f => f.RelativeTo(framework.Path));
                foreach (var file in files)
                {
                    CopyFile(variationVirtualTarget, framework.Path.Combine(file), targetDir.Combine(framework.Path.FileName).Combine(file));
                }
            }

            CopyFile(variationVirtualTarget, "Editor/Resources/OSX/UnityInfo-Stripped.plist", $"{EditorFiles.TargetPath}/Unity.app/Contents/Info.plist");
        }

        private static void CopyFile(JamTarget variationVirtualTarget, NPath fromFile, NPath targetFile)
        {
            variationVirtualTarget.DependOn(Copy.Setup(fromFile, targetFile));
        }

        protected override NPath[] FilterCoreModuleSources(IEnumerable<NPath> nPaths, UnityPlayerConfiguration unityPlayerConfiguration)
        {
            var blacklist = new[]
            {
                "PlatformDependent/OSX/VideoTexture.mm", // this gets included also from Audio module
                "PlatformDependent/OSX/NewInputOSX.mm"
            };
            return nPaths.Where(n => !blacklist.Any(b => n.ToString().Contains(b))).ToArray();
        }
    }
}
