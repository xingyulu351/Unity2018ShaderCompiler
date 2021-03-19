using System;
using System.Collections.Generic;
using System.Linq;
using Bee.ProjectGeneration;
using Bee.ProjectGeneration.XCode;
using Bee.Tools;
using JamSharp.Runtime;
using NiceIO;
using Projects.Jam;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.V2;
using Unity.BuildTools;
#if PLATFORM_IN_SOURCESTREE_IOS
using PlatformDependent.iPhonePlayer.Jam;
using iOSSupport = PlatformDependent.iPhonePlayer.Jam.iOSSupport;
#endif
using Unity.TinyProfiling;

namespace Unity.BuildSystem
{
    [Help(
        BuildTargetName,
        "IDE projects (C++)",
        category: Help.Category.Projects,
        order: 2,
        configs: Help.Configs.None)]

    public static class NativeProjectFiles
    {
        public const string BuildTargetName = "NativeProjectFiles";


        public static void Setup()
        {
            // Optimization; if generation of IDE projects is not requested then don't
            // go into calculation of all the things below
            if (!IsRequested)
                return;

            BuiltinRules.NotFile(BuildTargetName);

            SetupValidConfigurations();
            SetupCLionProjects();
            if (HostPlatform.IsOSX)
                SetupXCodeProjects();
            if (HostPlatform.IsWindows)
                SetupVisualStudioProjects();
        }

        // While working in source code, many developers want to do search/grepping across "mostly whole" codebase
        // (e.g. graphics API implementations for all platforms to be present in the IDE, instead of just the ones
        // in the platform they are on).
        //
        // This is the set of additional source files we make available in the IDE projects.
        //
        // This is not a simple list of root folders (like "Runtime", "Editor" etc.) since subfolders contain a lot
        // of files that either are not source files, or not really relevant (various 3rd party packages, large
        // auto-generated files etc.). Including them all makes the projects massive and slows down grepping.
        static NPath[] NonProjectSourceFiles()
        {
            using (TinyProfiler.Section("FindAdditionalSourceFiles"))
            {
                var fullFolders = new NPath[]
                {
                    "Editor/Mono",
                    "Editor/Platform",
                    "Editor/Src",
                    "Editor/Tests",
                    "Editor/Tools/GeometryToolbox",
                    "Editor/Tools/Unwrap",
                    "Modules",
                    "Projects/PrecompiledHeaders",
                    "Platforms",
                    "Shaders",
                };
                var platformPatterns = new[]
                {
                    "PlatformDependent/*/Source/**/*.cpp",
                    "PlatformDependent/*/Source/**/*.h",
                    "PlatformDependent/*/Source/**/*.bindings",
                    "PlatformDependent/*/Source/**/*.hlsl",
                    "PlatformDependent/*/Source/**/*.pssl",
                    "PlatformDependent/*/CgBatch/**/*.cpp",
                    "PlatformDependent/*/CgBatch/**/*.h",
                    "PlatformDependent/*/Editor/**/*.cpp",
                    "PlatformDependent/*/Editor/**/*.cs",
                    "PlatformDependent/*/Editor/**/*.h",
                };
                var files =
                    new NPath("Runtime").Files(new[] {"cpp", "h", "inl", "cs", "mm", "s", "asm", "bindings", "natvis", "shader", "hlsl", "ypp", "lpp"}, true)
                        .Concat(fullFolders.SelectMany(f => f.Files(true)))
                        .Concat(platformPatterns.SelectMany(Glob.Search).Select(f => new NPath(f)))
                        .Concat(new NPath("Projects/Jam").Files("*.jam.cs"))
                        .Where(p =>
                        {
                            var ext = p.Extension;
                            return
                            // don't want to include these folders into projects
                            !p.ToString().StartsWith("Editor/Src/AssetPipeline/ShaderImporting/ShaderUpgraderTests") &&
                            !p.ToString().StartsWith("Editor/Platform/Windows/res") &&
                            // due to how globbing works (via C# Directory.GetFiles patterns), "*.cpp" also incluces "*.cpp.preformat.bak" files; don't want to include those
                            ext != "bak" && ext != "orig" &&
                            // skip hidden files, e.g. ".DS_Store"
                            !p.FileName.StartsWith(".");
                        });
                return files.ToArray();
            }
        }

        static void SetupCLionProjects()
        {
            if (HostPlatform.IsWindows)
            {
                var capplicationNames = new List<String>() {"EditorApp"};
                var allTargetsProjects = CApplicationNativeProgram.Get(capplicationNames.ToArray())
                    .Concat(editorExtraProjects.Value)
                    .Concat(shaderCompilerProjects.Value)
                    .Append(WinStandaloneSupport.Instance.PlayerProgram)
#if PLATFORM_IN_SOURCESTREE_WEBGL
                    .Append(PlatformDependent.WebGL.Jam.WebGLSupport.Instance.PlayerProgram)
#endif
                    .ToArray();

                SetupCLionProject(allTargetsProjects, "Projects/CMake/AllTargets/CMakeLists.txt");
            }

            if (HostPlatform.IsOSX)
            {
                var allTargetsProjects = CApplicationNativeProgram.Get(new[] {"MacEditorApp"})
                    .Append(MacStandaloneSupport.Instance.PlayerProgram)
#if PLATFORM_IN_SOURCESTREE_IOS
                    .Append(iOSSupport.iOSInstance.PlayerProgram).Append(tvOSSupport.tvOSInstance.PlayerProgram)
#endif
#if PLATFORM_IN_SOURCESTREE_WEBGL
                    .Append(PlatformDependent.WebGL.Jam.WebGLSupport.Instance.PlayerProgram)
#endif
                    .Concat(editorExtraProjects.Value)
                    .Concat(shaderCompilerProjects.Value);

                SetupCLionProject(
                    allTargetsProjects.ToArray(),
                    "Projects/CMake/AllTargets/CMakeLists.txt");
            }

#if PLATFORM_IN_SOURCESTREE_LINUX
            if (HostPlatform.IsLinux)
            {
                SetupCLionProject(LinuxStandaloneSupport.Instance.PlayerProgram.AsEnumerable(),
                    "Projects/CMake/LinuxStandaloneSupport/CMakeLists.txt");
                SetupCLionProject(CApplicationNativeProgram.Get("LinuxEditorApp").AsEnumerable(),
                    "Projects/CMake/LinuxEditor/CMakeLists.txt");
            }
#endif
        }

        static void SetupCLionProject(IEnumerable<NativeProgram> programs, NPath path)
        {
            var validPrograms = programs.Where(np => np.ValidConfigurations.Any(c => c?.ToolChain?.Sdk != null));
            new CLionProjectFile(validPrograms, path);
        }

        private static readonly CodeGen[] commonConfigs = {CodeGen.Debug, CodeGen.Release};

        private static readonly Lazy<NativeProgram[]> editorExtraProjects = new Lazy<NativeProgram[]>(() =>
            CApplicationNativeProgram.Get("UnitTest++", "WebExtract")
                .Append(Binary2Text.NativeProgram)
                .Append(UnwrapCL.NativeProgram)
                .Append(AssetCacheServer.NativeProgram)
                .ToArray());

        private static readonly Lazy<NativeProgram[]> shaderCompilerProjects = new Lazy<NativeProgram[]>(() => new[] { CgBatch.NativeProgram, glslang.NativeProgram.Value, GlslOptimizer.NativeProgram.Value, hlslang.NativeProgram.Value, HLSLcc.NativeProgram.Value});

        // VisualStudio only; extra projects to include into solutions (mostly platform things)
        public static Dictionary<NativeProgram, NativeProgramConfiguration[]> VSExtraProjects { get; } = new Dictionary<NativeProgram, NativeProgramConfiguration[]>();

        // VisualStudio only; project folder hierarchy in the solution explorer
        public static readonly Dictionary<string, string> VSProjectNesting = new Dictionary<string, string>
        {
            {"glslang", "External"},
            {"GlslOptimizer", "External"},
            {"hlslang", "External"},
            {"HLSLcc", "External"},
            {"UnitTest++", "External"},
            {"UnityEngine", "Managed"},
            {"UnityEditor", "Managed"},
            {"DataContract", "Managed"},
            {"UNetWeaver", "Managed"},
            {"Unity.CecilTools", "Managed"},
            {"Unity.SerializationLogic", "Managed"},
            {"CgBatch", "Tools"},
            {"Binary2Text", "Tools"},
            {"WebExtract", "Tools"},
            {"UnwrapCL", "Tools"},
            {"UnityCrashHandler64", "Tools"},
            {"WebGLSupport", "Platforms/WebGL"},
            {"AssetCacheServer", "Tools"},
        };

        // For VisualStudio we want to generate multiple solution files referencing various sets of project files (e.g. AllTargets
        // with "everything", but also smaller solutions e.g. for PS4 only). Both for IDE loading times, and because
        // you'd sometimes want to debug both editor & console at once, so having two solutions is convenient.
        private static readonly Dictionary<string, SolutionProjects> VSSolutions = new Dictionary<string, SolutionProjects>();

        public static void AddVSSolutionProjects(string solution, params string[] projects)
        {
            AddVSSolutionProjects(solution, CApplicationNativeProgram.Get(projects).ToArray());
        }

        public static void AddVSSolutionProjects(string solution, params NativeProgram[] projects)
        {
            if (!VSSolutions.ContainsKey(solution))
                VSSolutions.Add(solution, new SolutionProjects());
            VSSolutions[solution].native.AddRange(projects);
        }

        private static void AddVSSolutionManagedProjects(string solution, params string[] projects)
        {
            if (!VSSolutions.ContainsKey(solution))
                VSSolutions.Add(solution, new SolutionProjects());
            VSSolutions[solution].managed.AddRange(projects);
        }

        public static void AddVSManagedProject(string targetName, string folder, string addToWorkspace = null)
        {
            VSProjectNesting.Add(targetName, folder);
            AddVSSolutionManagedProjects("AllTargets", targetName);
            if (addToWorkspace != null)
                AddVSSolutionManagedProjects(addToWorkspace, targetName);
        }

        static NPath GetStandaloneOutputFolder(NPath baseFolder, UnityPlayerConfiguration cfg)
        {
            var res = PlatformSupportUtils.GetStandaloneConfigBaseFolder(baseFolder, cfg.PlatformName, cfg.ScriptingBackend.ToString(), cfg.DevelopmentPlayer);
            if (cfg.ToolChain.LegacyPlatformIdentifier == "macosx32" || cfg.ToolChain.LegacyPlatformIdentifier == "macosx64")
                res = res.Combine("UnityPlayer.app/Contents/MacOS");
            return res;
        }

        static void SetupValidConfigurations()
        {
            if (HostPlatform.IsLinux)
            {
                var editorConfigs = Permute(commonConfigs, new[] {"linux64"});
                var editorApps = CApplicationNativeProgram.Get(new[] {"LinuxEditorApp"}).Concat(editorExtraProjects.Value).Concat(shaderCompilerProjects.Value).ToArray();
                ConfigureUnsetNativeProgramProperties(editorApps);
                foreach (var np in editorApps)
                    np.ValidConfigurations = editorConfigs;
            }
            if (HostPlatform.IsOSX)
            {
                var editorConfigs = Permute(commonConfigs, new[] {"macosx64"});
                var editorApps = CApplicationNativeProgram.Get(new[] {"MacEditorApp"}).Concat(editorExtraProjects.Value).Concat(shaderCompilerProjects.Value).ToArray();
                ConfigureUnsetNativeProgramProperties(editorApps);
                foreach (var np in editorApps)
                    np.ValidConfigurations = editorConfigs;
            }
            if (HostPlatform.IsWindows)
            {
                var editorConfigs = Permute(commonConfigs, new[] {"win64"});
                var editorApps = new[] {CApplicationNativeProgram.Get("EditorApp"), UnityCrashHandler.NativeProgram.Value}
                    .Concat(editorExtraProjects.Value)
                    .Concat(shaderCompilerProjects.Value)
                    .ToArray();

                foreach (var np in editorApps)
                    np.ValidConfigurations = editorConfigs;
                SetupWindowsPlatformProjects();

                // Do this after SetupWindowsPlatformProjects so we can figure out all the projects that we're actually
                // going to run projectgen for
                ConfigureUnsetNativeProgramProperties(VSSolutions.Values.SelectMany(s => s.native)
                    .Concat(editorApps)
#if PLATFORM_IN_SOURCESTREE_WEBGL
                    .Append(PlatformDependent.WebGL.Jam.WebGLSupport.Instance.PlayerProgram)
#endif
                    .Concat(VSExtraProjects.Keys)
                    .Distinct()
                );
            }
        }

        public static bool ShouldIncludePlatform(string platformName, string platformJamFile, string envVarToCheck)
        {
            // Source code must not be stripped out
            NPath jamPath = $"{Paths.UnityRoot}/PlatformDependent/{platformJamFile}";
            if (!jamPath.FileExists())
                return false;

            // Can explicitly disable by having an "DISABLE_{NAME}_IDE_PROJECT = 1 ;" in UserOveride.jam file
            if (GlobalVariables.Singleton[$"DISABLE_{platformName.ToUpperInvariant()}_IDE_PROJECT"].JamEquals("1"))
            {
                using (new ConsoleColorScope(ConsoleColors.Warning))
                    Console.WriteLine($"Skipping {platformName} IDE projects (disabled in UserOverride.jam)");
                return false;
            }

            // SDK has to be set up (checking via env var)
            if (!string.IsNullOrEmpty(envVarToCheck))
            {
                var envValue = Environment.GetEnvironmentVariable(envVarToCheck);
                if (string.IsNullOrEmpty(envValue) || !new NPath(envValue).DirectoryExists())
                {
                    using (new ConsoleColorScope(ConsoleColors.Warning))
                        Console.WriteLine($"Skipping {platformName} IDE projects (no {envVarToCheck})");
                    return false;
                }
            }

            return true;
        }

        private static void SetupVSExtraProject(NativeProgram np, string folder)
        {
            var arch = "win64";
            if (HostPlatform.IsOSX)
                arch = "macosx64";
            else if (HostPlatform.IsLinux)
                arch = "linux64";
            var configs = Permute(commonConfigs, new[] {arch});
            VSExtraProjects.Add(np, configs);
            VSProjectNesting.Add(np.Name, folder);
            np.ValidConfigurations = configs;
        }

        public static void SetupVSPlatformProject(string projectName, string platformName) => SetupVSPlatformProject(CApplicationNativeProgram.Get(projectName), platformName);
        private static void SetupVSPlatformProject(NativeProgram nativeProgramValue, string platformName) => SetupVSExtraProject(nativeProgramValue, $"Platforms/{platformName}");
        public static void SetupVSExtensionProject(string projectName) => SetupVSExtensionProject(CApplicationNativeProgram.Get(projectName));
        private static void SetupVSExtensionProject(NativeProgram nativeProgram) => SetupVSExtraProject(nativeProgram, "Extensions");

        private static void SetupWindowsPlatformProjects()
        {
#if PLATFORM_IN_SOURCESTREE_PS4
            if (ShouldIncludePlatform("PS4", "PS4/Jam/PS4Player.jam.cs", "SCE_ORBIS_SDK_DIR"))
            {
                PlatformDependent.PS4.Jam.PS4Player.TopLevel();
                AddVSManagedProject("PS4Il2CppPlugin", "Platforms/PS4", "PS4");
                AddVSManagedProject("PS4EditorExtensions", "Platforms/PS4", "PS4");
                SetupVSPlatformProject(PlatformDependent.PS4.Jam.PS4CgBatchPlugin.NativeProgram.Value, "PS4");
                SetupVSPlatformProject("PS4EditorExtensionsNative", "PS4");
                AddVSSolutionProjects("PS4", CApplicationNativeProgram.Get("PS4Player"), PlatformDependent.PS4.Jam.PS4CgBatchPlugin.NativeProgram.Value, CApplicationNativeProgram.Get("PS4EditorExtensionsNative"));
            }
#endif
#if PLATFORM_IN_SOURCESTREE_XBOXONE
            if (ShouldIncludePlatform("XboxOne", "XboxOne/Jam/XboxOnePlayer.jam.cs", "DurangoXDK"))
            {
                PlatformDependent.XboxOne.Jam.XboxOnePlayer.TopLevel();
                AddVSManagedProject("XboxOneIl2CppPlugin", "Platforms/XboxOne", "XboxOne");
                AddVSManagedProject("XboxOneEditorExtensions", "Platforms/XboxOne", "XboxOne");
                SetupVSPlatformProject(PlatformDependent.XboxOne.Jam.XboxOneCgBatchPlugin.NativeProgram.Value, "XboxOne");
                SetupVSPlatformProject("XboxOneEditorExtensionsNative", "XboxOne");
                AddVSSolutionProjects("XboxOne", CApplicationNativeProgram.Get("XboxOnePlayer"), PlatformDependent.XboxOne.Jam.XboxOneCgBatchPlugin.NativeProgram.Value, CApplicationNativeProgram.Get("XboxOneEditorExtensionsNative"));
            }
#endif
#if PLATFORM_IN_SOURCESTREE_SWITCH
            if (ShouldIncludePlatform("Switch", "Switch/Jam/SwitchPlayer.jam.cs", "NINTENDO_SDK_ROOT"))
            {
                PlatformDependent.Switch.Jam.SwitchPlayer.TopLevel();
                AddVSManagedProject("SwitchIl2CppPlugin", "Platforms/Switch", "Switch");
                AddVSManagedProject("SwitchEditorExtensions", "Platforms/Switch", "Switch");
                SetupVSPlatformProject(PlatformDependent.Switch.Jam.SwitchCgBatchPlugin.NativeProgram.Value, "Switch");
                AddVSSolutionProjects("Switch", CApplicationNativeProgram.Get("SwitchPlayer"), PlatformDependent.Switch.Jam.SwitchCgBatchPlugin.NativeProgram.Value);
            }
#endif
#if PLATFORM_IN_SOURCESTREE_WINRT
            if (ShouldIncludePlatform("UAP", "WinRT/WinRT.jam.cs", null))
            {
                MetroSupport.TopLevel();
                AddVSManagedProject("MetroEditorExtensions", "Platforms/UWP", "UWP");
                SetupVSPlatformProject("MetroEditorExtensionsNative", "UWP");
                AddVSSolutionProjects("UWP", "MetroEditorExtensionsNative");
            }
#endif
        }

        static void SetupXCodeProjects()
        {
            var macPlayer = MacStandaloneSupport.Instance.PlayerProgram;
            var allTargetsProjects =
                new[] {CApplicationNativeProgram.Get("MacEditorApp")}
                .Append(macPlayer);
#if PLATFORM_IN_SOURCESTREE_IOS
            allTargetsProjects = allTargetsProjects.Concat(new[] {iOSSupport.iOSInstance.PlayerProgram, tvOSSupport.tvOSInstance.PlayerProgram});
#endif
            allTargetsProjects = allTargetsProjects
                .Concat(editorExtraProjects.Value)
                .Concat(shaderCompilerProjects.Value)
#if PLATFORM_IN_SOURCESTREE_WEBGL
                .Append(PlatformDependent.WebGL.Jam.WebGLSupport.Instance.PlayerProgram)
#endif
                ;

                var extraSourceFiles = NonProjectSourceFiles();
                SetupXCodeProject(allTargetsProjects, "AllTargets", extraSourceFiles);

                var editorProjects = CApplicationNativeProgram.Get(new[] {"MacEditorApp"})
                    .Concat(editorExtraProjects.Value)
                    .Append(CgBatch.NativeProgram)
                    .ToArray();

                SetupXCodeProject(editorProjects, "MacEditor", extraSourceFiles);
                SetupXCodeProject(new[] {macPlayer}, "MacStandalone");
#if PLATFORM_IN_SOURCESTREE_WEBGL
                SetupXCodeProject(new[] {PlatformDependent.WebGL.Jam.WebGLSupport.Instance.PlayerProgram}, "WebGL");
#endif
#if PLATFORM_IN_SOURCESTREE_IOS
                SetupXCodeProject(new[] {iOSSupport.iOSInstance.PlayerProgram}, "iOS");
                SetupXCodeProject(new[] {tvOSSupport.tvOSInstance.PlayerProgram}, "AppleTV");
#endif
        }

        static void SetupXCodeProject(IEnumerable<NativeProgram> programs, string projectName, IEnumerable<NPath> extraSourceFiles = null)
        {
            var validPrograms = programs.Where(np => np.ValidConfigurations.Any(c => c?.ToolChain?.Sdk != null));
            new XCodeProjectFile(validPrograms, $"Projects/Xcode/{projectName}.xcodeproj/project.pbxproj", extraSourceFiles);
        }

        static void SetupVisualStudioProjects()
        {
            var extraProjects = VSExtraProjects.Keys.ToArray();
            var winStandaloneNativeProgram = WinStandaloneSupport.Instance.PlayerProgram;
            winStandaloneNativeProgram.Name = "StandalonePlayer";

            var allTargetsProjectsEnumerable = CApplicationNativeProgram.Get(new[] {"EditorApp"})
                .Append(UnityCrashHandler.NativeProgram.Value)
#if PLATFORM_IN_SOURCESTREE_WEBGL
                .Append(PlatformDependent.WebGL.Jam.WebGLSupport.Instance.PlayerProgram)
#endif

                .Concat(editorExtraProjects.Value)
                .Concat(shaderCompilerProjects.Value)
                .Concat(extraProjects)
                .Append(winStandaloneNativeProgram);
#if PLATFORM_IN_SOURCESTREE_UNIVERSALWINDOWS
            if (NewBuildCodeEnabled.ForUWP)
            {
                var uwpNativeProgram = UWPSupport.Instance.PlayerProgram;
                uwpNativeProgram.Name = "MetroSupport";
                allTargetsProjectsEnumerable = allTargetsProjectsEnumerable.Append(uwpNativeProgram);
            }
#endif

            var allTargetsProjects = allTargetsProjectsEnumerable.ToArray();
            AddVSSolutionProjects("AllTargets", allTargetsProjects);

            // Project for "minimal editor stuff" (just editor itself, unit testing library and root shader compiler)
            var editorProjects = CApplicationNativeProgram.Get(new[] {"EditorApp"})
                .Concat(editorExtraProjects.Value)
                .Append(CgBatch.NativeProgram)
                .ToArray();
            AddVSSolutionProjects("Editor", editorProjects);

            AddVSSolutionProjects("Standalone", WinStandaloneSupport.Instance.PlayerProgram);
            // When editing C# source files, VS only provides full decent intellisense if said files belong to actual C#
            // projects that are present in the solution. So we need to add a bunch of references to commonly needed
            // .csproj files to the AllTargets solution.
            var managedProjectReferences = new[]
            {
                LegacyCSharpSupport.UnityEditorProjectPath,
                LegacyCSharpSupport.UnityEngineProjectPath,
                // next 4 are referenced by UnityEditor.csproj, and need to be in solution for VS C# compile step to succeed
                LegacyCSharpSupport.DataContractProjectPath,
                LegacyCSharpSupport.UnitySerializationLogicProjectPath,
                LegacyCSharpSupport.UnityCecilToolsProjectPath,
                LegacyCSharpSupport.UNetWeaverProjectPath
            };
            // We also have to *not* include these .cs files into any C++ projects, since otherwise various C# tools
            // (e.g. Resharper) don't provide the full experience either.
            var managedSourceFiles = new JamList(
                ManagedProjectFiles.GetEngineCSFiles(),
                ManagedProjectFiles.GetEditorCSFiles()
                ).ToArray();

            new VisualStudioProjectFile(
                "Projects/VisualStudio",
                VSSolutions,
                NonProjectSourceFiles(),
                VSProjectNesting,
                managedProjectReferences, managedSourceFiles);
        }

        // not interested in majority of non-Lump configs; only keep release+nonlump out of them
        static IEnumerable<Config> RemoveUnusedLumpConfigs<Config>(IEnumerable<Config> configs) where Config : NativeProgramConfiguration
        {
            var configsArray = configs.ToArray();
            var lumped = configsArray.Where(c => c.Lump);
            var firstReleaseNonLump = configsArray.FirstOrDefault(c => !c.Lump && c.CodeGen == CodeGen.Release);
            return lumped.Append(firstReleaseNonLump);
        }

        static NativeProgramConfiguration[] Permute(CodeGen[] codegens, string[] platforms)
        {
            var codegen = codegens;
            var plats = platforms;
            var lump = new[] {true, false};

            var configs = codegen.SelectMany(c => plats.SelectMany(p => lump.Select(l => new NativeProgramConfiguration(c, CJamCompatibility.CreateToolChainEmulatingV1BuildCode(p), l))));
            configs = RemoveUnusedLumpConfigs(configs);
            return configs.ToArray();
        }

        public static UnityPlayerConfiguration[] PermutePlayer(CodeGen[] codegen, string[] platforms, ScriptingBackend[] backends)
        {
            var plats = platforms;
            var dev = new[] {true, false};

            var configs = codegen.SelectMany(c => plats.SelectMany(p => dev.SelectMany(d => backends.Select(b => new UnityPlayerConfiguration(c, V1BuildCodeEmulation.CreateToolChainEmulatingV1BuildCode(p), true, b, d)))));

            // Remove Debug+NonDevelopment configs; aren't interested
            configs = configs.Where(c => !(c.CodeGen == CodeGen.Debug && !c.DevelopmentPlayer));
            // Remove Master+Development configs; aren't interested
            configs = configs.Where(c => !(c.CodeGen == CodeGen.Master && c.DevelopmentPlayer));

            // Platforms that have Master config today do not have Release+NonDev
            if (codegen.Contains(CodeGen.Master))
            {
                configs = configs.Where(c => !(c.CodeGen == CodeGen.Release && !c.DevelopmentPlayer));
            }

            return configs.ToArray();
        }

        public static bool IsRequested
        {
            get
            {
                var cmdlineTargets = GlobalVariables.Singleton["JAM_COMMAND_LINE_TARGETS"];
                return cmdlineTargets.Contains(BuildTargetName) || cmdlineTargets.Contains(ProjectFiles.BuildTargetName);
            }
        }

        private static void ConfigureUnsetNativeProgramProperties(IEnumerable<NativeProgram> programs)
        {
            // Check the provided NativePrograms to fill in properties that might be missing which we need for project
            // files to be generated. It's preferable if we get all the properties to be set up when the NativeProgram
            // itself is being created, of course, but we miss some spots right now, especially in V1 code.

            bool useDowit = GlobalVariables.Singleton["USE_DOWIT"].JamEquals("1");

            foreach (var nativeProgram in programs)
            {
                if (!nativeProgram.CommandToBuild.IsSet)
                {
                    nativeProgram.CommandToBuild.Set(c =>
                    {
                        var targetNameForIDE = nativeProgram.TargetNameForIDE.For(c) ?? nativeProgram.Name;
                        var args = $"{c.JamArgs} {nativeProgram.ExtraJamArgs}";
                        var cmd = $"perl {Paths.UnityRoot}/jam.pl {targetNameForIDE} {args}";

                        if (useDowit)
                            cmd = $@"dowit buildproject -- {cmd}";

                        return cmd;
                    });
                }
            }
        }
    }
}
