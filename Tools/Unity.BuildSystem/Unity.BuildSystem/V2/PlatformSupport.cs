using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using Bee.DotNet;
using Bee.Toolchain.Xcode;
using External.Compression.lz4;
using External.libwebsockets;
using External.pubnub;
using External.TextureCompressors.Crunch;
using External.udis86;
using JamSharp.Runtime;
using Modules;
using NiceIO;
using Projects.Jam;
using Tools.BindingsGenerator;
using Tools.RegistrationGenerator;
using Tools.TypeForwarderGenerator;
using Tools.UnityBindingsParser.BindingsToCsAndCpp;
using Unity.BuildSystem.Common;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using Unity.TinyProfiling;
using static Unity.BuildSystem.NativeProgramSupport.NativeProgramConfiguration;
using External.TextureCompressors.Basis;

namespace Unity.BuildSystem.V2
{
    public abstract class PlatformSupport : UnityRuntimeBuildCode
    {
        public virtual string PlatformsFolder => PlatformName;
        protected virtual NPath IvyXmlTemplateLocation => $"PlatformDependent/{PlatformDependentFolders.First()}/ivy.xml";
        protected abstract string EditorExtensionSourceDirectory { get; }
        protected abstract string EditorExtensionAssembly { get; }
        protected abstract string EditorExtensionBuildTarget { get; }
        protected virtual JamList DeveloperTargetAliases => new JamList();
        public abstract string[] CommandLinesOfInterest { get; }

        protected override IEnumerable<string> ModuleNames => ConvertedJamFile.Vars.RUNTIME_modules.Distinct();//.Append("SharedInternals");

        public override void Setup()
        {
            using (TinyProfiler.Section($"{nameof(PlatformSupport)}.{nameof(Setup)}"))
            {
                SetupRuntimeModules2.Reset();
                SetupRuntimeModules2.LoadModules($"Platforms/{PlatformsFolder}/Modules".AsEnumerable(), Defines);

                base.Setup();
                foreach (var a in DeveloperTargetAliases.AsJamTargets())
                    a.DependOn(VariationVirtualTargetFor(DefaultDeveloperConfiguration));
                CommonVariationDependencies.DependOn(IvyPackager.Instance()
                    .SetupInvocation(BuildFolder.Combine("ivy.xml"), IvyXmlTemplateLocation));

                var editorExtension = SetupEditorExtensionBuild();
                CommonVariationDependencies.DependOn(editorExtension.Path);

                CommonVariationDependencies.DependOn(TextFile.Setup(BuildFolder.Combine("modules.asset"),
                    ModuleAssetsContent()));

                Help.AddHelpForSpecificPlatform(CommandLinesOfInterest, PrintHelp);
            }
        }

        protected virtual CSharpProgram SetupEditorExtensionBuild()
        {
            var ext = SetupEditorExtension();
            if (!ProjectFiles.IsRequested)
                ext.SetupDefault();
            new JamTarget(EditorExtensionBuildTarget).DependOn(ext.Path);
            new JamTarget("AllEditorAssemblies").DependOn(ext.Path);
            return ext;
        }

        protected virtual CSharpProgram SetupEditorExtension()
        {
            var ext = new CSharpProgram
            {
                Path = $"{BuildFolder}/{EditorExtensionAssembly}",
                Sources = {EditorExtensionSourceDirectory},
                CopyReferencesNextToTarget = false,
                Framework = Framework.Framework35,
                LanguageVersion = "6",
                WarningsAsErrors = false,
            };
            ext.Defines.Add(CsharpPlatformDefines);
            if (!ProjectFiles.IsRequested)
            {
                ext.PrebuiltReferences.Add(
                    new JamList("DefaultEngineDll").GetBoundPath().Single(),
                    new JamList("DefaultEditorDll").GetBoundPath().Single()
                );
            }
            else
            {
                // When generating IDE project files, we want editor extension to reference .csprojs of engine/editorFilter
                // assemblies, as well as change build event to invoke our own build system.
                ext.CsprojReferences.Add(LegacyCSharpSupport.UnityEngineProjectReference, LegacyCSharpSupport.UnityEditorProjectReference);
                if (HostPlatform.IsWindows)
                {
                    ext.ProjectFilePath = $"Projects/VisualStudio/Projects/{EditorExtensionBuildTarget}.csproj";
                    ext.CprojBuildEvents = LegacyCSharpSupport.GetCsProjBuildEvents(EditorExtensionBuildTarget);
                }
                LegacyCSharpSupport.RegisterProgram(EditorExtensionBuildTarget, ext);
            }

            return ext;
        }

        void PrintHelp()
        {
            Console.WriteLine($"Build player and supporting files for {PlatformName}.");
            Help.Heading();
            Console.Write($"The following commands are the different ways you can build for {PlatformName}");
            Help.Normal();
            Console.WriteLine();
            foreach (var variation in AllPossibleVariations)
            {
                var flags = JamCmdLineForFlagRequirement(variation.FlagRequirement);
                Console.WriteLine($"{Help.JamCmd} {VariationVirtualTargetFor(variation)} {flags}");
            }
        }

        private static string JamCmdLineForFlagRequirement(FlagRequirement variationFlagRequirement)
        {
            if (variationFlagRequirement == null)
                return string.Empty;

            if (variationFlagRequirement.Flag == "CONFIG" && variationFlagRequirement.Value == "debug")
                return string.Empty;

            return $"-s{variationFlagRequirement.Flag}={variationFlagRequirement.Value}";
        }

        protected override void PostProcessNativeProgram(NativeProgram app)
        {
            base.PostProcessNativeProgram(app);
            foreach (var nativeProgram in AllNativePrograms)
            {
                nativeProgram.Defines.Add(c => ((UnityPlayerConfiguration)c).DevelopmentPlayer,
                    "ENABLE_PROFILER=1",
                    "UNITY_DEVELOPER_BUILD=1");
                nativeProgram.Defines.Add(c => !((UnityPlayerConfiguration)c).DevelopmentPlayer,
                    "ENABLE_PROFILER=0",
                    "UNITY_DEVELOPER_BUILD=0");
            }
        }
    }

    // TODO: maybe move this out into a new file?
    public abstract class EditorSupport : UnityRuntimeBuildCode
    {
        protected override IEnumerable<ModuleBase> ProvideModules()
        {
            var runtimeModules = base.ProvideModules();

            var bigEditorModule = new EditorModule();
            bigEditorModule.Dependencies.Add(runtimeModules.Select(m => m.Name));

            foreach (var module in ConvertedJamFile.Vars.EDITOR_modules
                     .Where(m => !ModuleBlacklist.Contains(m))
                     .Select(m => LegacyModule.CreateModuleFromLegacyModule(m, LegacyTargetName, VariationsToSetup)))
            {
                bigEditorModule.Cs.Add(module.Cs.ForAny());
                bigEditorModule.LegacyBindings.AddRange(module.LegacyBindings);
                bigEditorModule.Cpp.Add(module.Cpp.ForAny());
                bigEditorModule.IncludeDirectories.Add(module.IncludeDirectories.ForAny());
                bigEditorModule.PrebuiltLibraries.Add(module.PrebuiltLibraries.ForAny());
            }

            yield return bigEditorModule;
            foreach (var module in runtimeModules)
                yield return module;
        }

        protected override void PostProcessNativeProgram(NativeProgram app)
        {
            base.PostProcessNativeProgram(app);

            // TODO: The word "Player" from PlayerProgram doesn't make sense for the Editor
            PlayerProgram.Defines.Add(c => c.CodeGen == CodeGen.Debug,
                "DEBUG"
            );

            // TODO: The word "Player" from UnityPlayerConfiguration doesn't make sense for the Editor
            PlayerProgram.Defines.Add(c => ((UnityPlayerConfiguration)c).ScriptingBackend == ScriptingBackend.DotNet,
                "ENABLE_SERIALIZATION_BY_CODEGENERATION=1");
            PlayerProgram.Defines.Add(c => ((UnityPlayerConfiguration)c).ScriptingBackend != ScriptingBackend.DotNet,
                "ENABLE_SERIALIZATION_BY_CODEGENERATION=0");

            // TODO: better set these in EditorModule but we don't have yet the ability to set per file includes
            PlayerProgram.PerFileIncludeDirectories.Add(IsWindows, new NPath[] {"External/videoInput/dshow/include"},
                "External/videoInput/videoInput.cpp");
            PlayerProgram.PerFileIncludeDirectories.Add(new NPath[] {"External/OpenCL/builds/include"},
                new NPath("Editor/Src/GI").Files(true));
            PlayerProgram.PerFileIncludeDirectories.Add(new NPath[] {"External/SpeedTree/builds/Include"},
                "Editor/Src/AssetPipeline/SpeedTreeImporter.cpp");
            PlayerProgram.PerFileIncludeDirectories.Add(new NPath[] {"External/Umbra/interface"},
                "Editor/Src/OcclusionCulling.cpp", "Editor/Src/OcclusionCullingVisualization.cpp");
        }

        protected override IEnumerable<ModuleBase> ModulesForICallRegistration =>
            Modules.Where(m => !m.IsEditorModule).Concat(ProvideSmallEditorModules());

        protected virtual string[] EditorModulesBlacklist { get; } = Array.Empty<string>();

        private IEnumerable<ModuleBase> ProvideSmallEditorModules()
        {
            return ConvertedJamFile.Vars.EDITOR_modules
                .Where(m => !EditorModulesBlacklist.Contains(m))
                .Where(m => !ModuleBlacklist.Contains(m))
                .Select(m => LegacyModule.CreateModuleFromLegacyModule(m, LegacyTargetName, VariationsToSetup));
        }

        protected override void SetupModule(ModuleBase module)
        {
            base.SetupModule(module);

            if (module is EditorModule bigEditorModule)
            {
                var editorModuleClassRegistrationCpp = RegistrationGeneratorTool.Instance().SetupInvocation(
                    ArtifactsDir.Combine($"modules/{bigEditorModule.Name}/ClassRegistrations.gen.cpp"),
                    new[] {FinalModuleAssemblyLocationFor(DefaultDeveloperConfiguration, bigEditorModule)},
                    bigEditorModule.Name,
                    BuildVariationDirFor(DefaultDeveloperConfiguration).ToString());

                bigEditorModule.Defines.Add("UNITY_MODULE_EDITOR");
                bigEditorModule.Cpp.Add(editorModuleClassRegistrationCpp);

                foreach (var file in Cecil.Current35WithSymbolRefs.Append(NRefactory.Net35Assembly))
                {
                    new JamTarget(editorModuleClassRegistrationCpp).DependOn(Copy.Setup(
                        new JamTarget(file),
                        BuildVariationDirFor(DefaultDeveloperConfiguration).Combine(file.FileName)));
                }

                foreach (var smallEditorModule in ProvideSmallEditorModules())
                {
                    bigEditorModule.Cpp.Add(TextFile.Setup(
                        ArtifactsDir.Combine($"modules/{smallEditorModule.Name}/ICallRegistration.cpp"),
                        ModuleICallRegistrationFileBodyFor(smallEditorModule)));

                    var moduleClassRegistrationCpp = RegistrationGeneratorTool.Instance().SetupInvocation(
                        ArtifactsDir.Combine($"modules/{smallEditorModule.Name}/ClassRegistrations.gen.cpp"),
                        new[] {FinalModuleAssemblyLocationFor(DefaultDeveloperConfiguration, bigEditorModule)},
                        smallEditorModule.Name,
                        BuildVariationDirFor(DefaultDeveloperConfiguration).ToString());

                    bigEditorModule.Cpp.Add(moduleClassRegistrationCpp);
                }
            }
        }

        protected override void SetupVariation(UnityPlayerConfiguration variation)
        {
            base.SetupVariation(variation);

            var nativeProgramForCore = NativeProgramForModule(CoreModule);
            foreach (var staticLib in new[]
                 {
                     UnitTestPlusPlus.NativeProgram,
                     udis86lib.Udis86NativeProgramLibrary.Value,
                 })
            {
                nativeProgramForCore.PrebuiltLibraries.Add(c => c == variation,
                    (StaticLibrary)staticLib.SetupSpecificConfiguration(variation,
                        variation.ToolChain.StaticLibraryFormat));
            }

            var nativeProgramForEditorModule = NativeProgramForModule(ModuleByName("Editor"));
            foreach (var staticLib in new[]
                 {
                     pubnub.NativeProgram.Value,
                     libcrunch.LibCrunchNativeProgram,
                     libbasis.LibBasisNativeProgram,
                     UnityYAMLMergeLib.UnityYAMLMergeNativeProgram
                 })
            {
                nativeProgramForEditorModule.PrebuiltLibraries.Add(c => c == variation,
                    (StaticLibrary)staticLib.SetupSpecificConfiguration(variation,
                        variation.ToolChain.StaticLibraryFormat));
            }

            foreach (var staticLib in new[]
                 {
                     libwebsockets.SetupLibWebSocketsNativeProgramLibrary.Value
                 })
            {
                var nativeProgramForModule = NativeProgramForModule(ModuleByName("UNET"));
                nativeProgramForModule.PrebuiltLibraries.Add(c => c == variation,
                    (StaticLibrary)staticLib.SetupSpecificConfiguration(variation,
                        variation.ToolChain.StaticLibraryFormat));
            }

            //we build lz4 always in release, as it can be slow, and it's very unlikely we want to debug it.
            foreach (var np in new[] {lz4.LZ4HCNativeProgramLibrary, lz4.LZ4NativeProgramLibrary})
            {
                var releaseVariation = variation.WithCodeGen(CodeGen.Release);
                var artifactsPath = PlayerProgram.DefaultArtifactsPathFor(variation).Combine("staticlibs/" + np.Name);
                nativeProgramForCore.PrebuiltLibraries.Add(c => c == variation, (StaticLibrary)
                    np.SetupSpecificConfiguration(releaseVariation,
                        releaseVariation.ToolChain.StaticLibraryFormat, artifactsPath: artifactsPath));
            }

            VariationVirtualTargetFor(variation)
                .DependOn(TextFile.Setup(EditorFiles.ResourcesPath.Combine("modules.asset"),
                ModuleAssetsContent()));
        }
    }

    public abstract class UnityRuntimeBuildCode
    {
        protected ModuleBase[] Modules { get; private set; }
        protected JamTarget CommonVariationDependencies { get; private set; }

        public abstract string PlatformName { get; }
        private readonly Dictionary<ModuleBase, NativeProgram> _modulePrograms = new Dictionary<ModuleBase, NativeProgram>();

        protected IEnumerable<NativeProgram> ModuleNativePrograms => _modulePrograms.Values;

        protected ModuleBase ModuleByName(string name) => Modules.Single(m => m.Name == name);

        protected virtual NPath DefaultPrefixPch => "Projects/PrecompiledHeaders/UnityPrefix.h";

        protected UnityRuntimeBuildCode()
        {
            AllPossibleVariations = ProvideVariations().ToArray();
            foreach (var variation in AllPossibleVariations)
                variation.HumanTargetName = HumanTargetNameFor(variation, VariationPrefix, AllPossibleVariations);

            /* fear not! we are not actually going to build the default developer configuration,
             * we're only going to set it up. We need to set it up so that we know how to build the managed dll's
             * that are shared across variations, because by law of the land those must be built from the first
             * variation.
             */
            VariationsToSetup = FilteredVariationsForCurrentCommandLine(AllPossibleVariations.Where(HasRequiredFlags))
                .Append(DefaultDeveloperConfiguration)
                .Distinct()
                .ToArray();
            DummyCodeGeneratorHeader = $"{PlatformName}DummyCodeGeneratorHeader";
        }

        public abstract string VariationPrefix { get; }

        static bool HasRequiredFlags(UnityPlayerConfiguration configuration)
        {
            if (configuration.FlagRequirement == null)
                return true;
            return GlobalVariables.Singleton[configuration.FlagRequirement.Flag] == configuration.FlagRequirement.Value;
        }

        protected ModuleBase CoreModule => Modules.Single(m => m is CoreModule);
        protected virtual bool SupportPerformanceTests => true;

        // Some platforms have just one set of managed assemblies for editor use; whereas others
        // have a separate copy per scripting backend.
        protected virtual bool ManagedAssembliesForEditorPerScriptingBackend => false;
        protected virtual NPath ManagedAssembliesLocationForEditor(UnityPlayerConfiguration variation)
        {
            return ManagedAssembliesForEditorPerScriptingBackend
                ? $"Variations/{variation.ScriptingBackend.ToString().ToLowerInvariant()}/Managed"
                : "Managed";
        }

        protected virtual NativeProgram NativeProgramForModule(ModuleBase m)
        {
            if (ModuleBuildModeFor(m) == ModuleBuildMode.Source)
                return m == CoreModule ? PlayerProgram : NativeProgramForModule(CoreModule);

            NativeProgram result;
            if (_modulePrograms.TryGetValue(m, out result))
                return result;

            result = new NativeProgram(m.Name);

            _modulePrograms[m] = result;
            return result;
        }

        protected virtual bool UseRTTIFor(UnityPlayerConfiguration arg)
        {
            return arg.EnableUnitTests;
        }

        protected virtual ModuleBuildMode ModuleBuildModeFor(ModuleBase m) => ModuleBuildMode.Source;

        protected IEnumerable<string> SetupKatanaTarget(string targetNameForKatana, Func<UnityPlayerConfiguration, bool> selector)
        {
            var configs = AllPossibleVariations.Where(HasRequiredFlags).Where(selector)
                .Select(c => VariationVirtualTargetFor(c).JamTargetName);
            BuiltinRules.Depends(targetNameForKatana, new JamList(configs));
            return configs.AsEnumerable();
        }

        public virtual void Setup()
        {
            using (TinyProfiler.Section($"{nameof(UnityRuntimeBuildCode)}.{nameof(Setup)}"))
            {
                CommonVariationDependencies = new JamTarget($"{PlatformName}VariationDependencies");
                BuiltinRules.NotFile(CommonVariationDependencies);


                Modules = ProvideModules()
                    .OrderByDependencies()
                    .ToArray();

                Modules = Modules
                    .ForEach(SetupModule)
                    .OrderByDependencies()
                    .ToArray();

                var sharedInternalsVisibleToFile = TextFile.Setup(ArtifactsDir.Combine("InternalsVisibleTo.cs"),
                    InternalsVisibleToFileContentsFor(ModulesWithManagedCode));

                var codeGenerator = GenerateNativeDependenciesForManagedCode.CodeGeneratorTool.Instance();

                BuiltinRules.NotFile(DummyCodeGeneratorHeader);

                PlayerProgram = new NativeProgram(PlayerBinaryName);
                PlayerProgram.TargetNameForIDE.Set(config => ((UnityPlayerConfiguration)config).HumanTargetName);

                var bindingsCsFilesPerModule = new Dictionary<ModuleBase, List<NPath>>();

                foreach (var module in Modules)
                {
                    NativeProgramForModule(module);

                    foreach (var variation in VariationsToSetup)
                        module.NonLumpableFiles.Add(c => c == variation, module.Cpp.For(variation)
                            .Where(f => !NonLumpableFiles.IsLumpableFile(f.ToString())));

                    //todo: when we get rid of the .inc files, we only have to setup icall registrations for modules that
                    //actually have managed code.
                    NativeProgramForModule(module).Sources.Add(TextFile.Setup(
                        ArtifactsDir.Combine($"modules/{module.Name}/ICallRegistration.cpp"),
                        ModuleICallRegistrationFileBodyFor(module)));
                }

                foreach (var module in ModulesWithManagedCode)
                {
                    var bindingsCsFiles = new List<NPath>();
                    bindingsCsFilesPerModule[module] = bindingsCsFiles;
                    var targetProgramForModules = NativeProgramForModule(module);

                    foreach (var binding in module.LegacyBindings)
                    {
                        var result = LegacyBindingsConverterTool.Instance()
                            .SetupInvocationToGenerateCsAndCpp(BindingOutputDirectoryFor(module, ArtifactsDir), binding,
                                PlatformIdentifierForBindingsGenerator);
                        targetProgramForModules.Sources.Add(result.CppFile.Path);
                        var includeDirs = AllPossibleVariations.SelectMany(v => module.IncludeDirectories.For(v)).Distinct();
                        if (includeDirs.Any())
                            targetProgramForModules.PerFileIncludeDirectories.Add(includeDirs, result.CppFile.Path);

                        var defines = AllPossibleVariations.SelectMany(v => module.Defines.For(v)).Distinct();
                        if (defines.Any())
                            targetProgramForModules.PerFileDefines.Add(defines, result.CppFile.Path);

                        bindingsCsFiles.Add(result.CsFile.Path);
                    }

                    var args = codeGenerator.SetupAllInvocations(
                        new GenerateNativeDependenciesForManagedCode.CodeGeneratorTool.
                            SetupAllInvocationArgumentsForModule()
                        {
                            OutputCpp = $"{CodeGeneratorDir.Combine($"{module.Name}")}ScriptingClasses.cpp",
                            OutputHeader = $"{CodeGeneratorDir.Combine($"{module.Name}")}ScriptingClasses.h",
                            OutputWhiteList = $"{BuildFolder}/Whitelists/{module.Name}.xml",
                            InputAssembly = FinalModuleAssemblyLocationFor(DefaultDeveloperConfiguration, module),
                        });

                    BuiltinRules.Includes(DummyCodeGeneratorHeader, args.OutputHeader);
                    targetProgramForModules.Sources.Add(args.OutputCpp);
                    CommonVariationDependencies.DependOn(args.OutputWhiteList);
                }

                foreach (var module in ModulesWithManagedCode.Where(m => !m.IsEditorModule && m.Name != "SharedInternals"))
                {
                    NativeProgramForModule(module).Sources.Add(RegistrationGeneratorTool.Instance().SetupInvocation(
                        ArtifactsDir.Combine($"modules/{module.Name}/ClassRegistrations.gen.cpp"),
                        new[] {FinalModuleAssemblyLocationFor(DefaultDeveloperConfiguration, module)}, module.Name, ""));
                }

                foreach (var nativeProgram in AllNativePrograms)
                {
                    nativeProgram.IncludeDirectories.Add(GlobalIncludeDirectories());
                    nativeProgram.RTTI.Set(c => UseRTTIFor((UnityPlayerConfiguration)c));
                    nativeProgram.Defines.Add(Defines);
                    nativeProgram.Defines.Add("UNITY_BUILDPROGRAM_V2=1");

                    nativeProgram.Defines.Add(c => ((UnityPlayerConfiguration)c).EnableUnitTests, "ENABLE_UNIT_TESTS=1");
                    nativeProgram.Defines.Add(c => !((UnityPlayerConfiguration)c).EnableUnitTests, "ENABLE_UNIT_TESTS=0");

                    nativeProgram.Defines.Add(c => $"ENABLE_ASSERTIONS={(((UnityPlayerConfiguration)c).EnableAsserts ? "1" : "0")}");

                    nativeProgram.Defines.Add(c => ((UnityPlayerConfiguration)c).EnableUnitTests && SupportPerformanceTests , "ENABLE_PERFORMANCE_TESTS=1");
                    nativeProgram.Defines.Add(c => !(((UnityPlayerConfiguration)c).EnableUnitTests && SupportPerformanceTests), "ENABLE_PERFORMANCE_TESTS=0");

                    nativeProgram.ValidConfigurations = AllPossibleVariations;
                }

                foreach (var variation in VariationsToSetup)
                {
                    var variationVirtualTarget = VariationVirtualTargetFor(variation);
                    variationVirtualTarget.DependOn(CommonVariationDependencies);

                    foreach (var module in Modules)
                    {
                        var moduleCpp = module.Cpp.For(variation).ToArray();

                        var nativeProgramForModule = NativeProgramForModule(module);
                        nativeProgramForModule.Sources.Add(c => c == variation, moduleCpp);
                        ApplyIncludeDirsAndDefinesOfModuleToCpp(variation, module, moduleCpp);

                        nativeProgramForModule.PrebuiltLibraries.Add(c => c == variation, module.PrebuiltLibraries.For(variation).Select(l => new LegacyCApplicationLibrary(l)));
                        nativeProgramForModule.PrebuiltLibraries.Add(c => c == variation, module.Frameworks.For(variation).Select(l => new SystemFramework(l)));
                        nativeProgramForModule.NonLumpableFiles.Add(c => c == variation, module.NonLumpableFiles.For(variation));

                        nativeProgramForModule.ExtraDependenciesForAllObjectFiles.Add(c => c == variation, module.ExtraDependenciesForAllObjectFiles.For(variation));
                    }

                    foreach (var module in ModulesWithManagedCode)
                    {
                        var targetProgramForModule = NativeProgramForModule(module);
                        var finalModuleDllLocation = FinalModuleAssemblyLocationFor(variation, module);

                        var unprocessedTarget =
                            new NPath(
                                $"{ArtifactsDir}/{variation.Identifier}/Modules/{GetModuleDllName(module)}.dll");

                        var moduleAssembly = new CSharpAssemblyBuilder()
                            .WithPath(unprocessedTarget)
                            .WithSource(module.Cs.For(variation).ToArray())
                            .WithSource(bindingsCsFilesPerModule[module].ToArray())
                            .WithSource(sharedInternalsVisibleToFile)
                            .WithDefines(ScriptingBackendDefineFor(variation.ScriptingBackend))
                            .WithDefines(CsharpPlatformDefines.Elements)
                            .WithDefines("UNITY_ASSERTIONS")
                            .WithDefines(
                                module.Defines.For(variation).Where(d => !d.ToString().Contains('=')))
                            .WithPrebuiltReference(module.Dependencies.For(variation)
                                .Select(ModuleByName)
                                .Where(d => d.HasManagedCode)
                                .Select(d => FinalModuleAssemblyLocationFor(variation, d))
                                .ToArray())
                            .WithSystemReference(module.ManagedSystemReference.For(variation).ToArray())
                            .WithPrebuiltReference(module.ManagedPrebuiltReference.For(variation).ToArray())
                            .WithUnsafe()
                            .WithFramework(Framework.Framework35)
                            .WithIgnoredWarning(246, 626)
                            .WithLanguageVersion("6")
                            .WithCopyReferencesNextToTarget(false)
                            .Complete();

                        NPath bindingsGeneratorPatcherInputFile;
                        var managedConfig = new CSharpProgramConfiguration(CSharpCodeGen.Debug, null);
                        if (module == CoreModule)
                        {
                            bindingsGeneratorPatcherInputFile = unprocessedTarget.Parent.Combine("Patched")
                                .Combine(unprocessedTarget.FileName);
                            UnsafeUtilityPatcherTool.Instance.SetupInvocation(bindingsGeneratorPatcherInputFile,
                                unprocessedTarget);
                        }
                        else
                        {
                            bindingsGeneratorPatcherInputFile = moduleAssembly.Path;
                        }

                        var patcherReferences = moduleAssembly.PrebuiltReferences.ForAny().Select(a => new JamTarget(a)).ToArray();
                        var moduleDefines = moduleAssembly.Defines.For(managedConfig).ToArray();
                        BindingsGeneratorTool.Instance().SetupPatchInvocation(
                            outputFile: finalModuleDllLocation,
                            inputAssembly: bindingsGeneratorPatcherInputFile,
                            defines: moduleDefines,
                            patcherReferences: patcherReferences,
                            scriptingBackend: variation.ScriptingBackend,
                            profile: null);

                        foreach (var csFile in CSharpProgram.SourcesIn(moduleAssembly.Sources.ForAny()).Where(s =>
                            s.ToString().EndsWith(".bindings.cs")))
                        {
                            var withoutExtension = ArtifactsDir.Combine($"modules/{module.Name}/{variation.Identifier}", csFile.FileNameWithoutExtension);
                            NPath generatedCpp = $"{withoutExtension}.gen.cpp";
                            NPath generatedHeader = $"{withoutExtension}.gen.h";
                            BindingsGeneratorTool.Instance().SetupInvocation(
                                generatedCpp,
                                generatedHeader,
                                moduleAssembly.SetupDefault().Path,
                                csFile,
                                patcherReferences,
                                moduleDefines, variation.ScriptingBackend, null
                            );

                            targetProgramForModule.Sources.Add(c => c == variation, generatedCpp);
                            ApplyIncludeDirsAndDefinesOfModuleToCpp(variation, module, generatedCpp);
                        }

                        variationVirtualTarget.DependOn(finalModuleDllLocation);
                    }

                    foreach (ScriptingBackend value in Enum.GetValues(typeof(ScriptingBackend)))
                    {
                        foreach (var np in AllNativePrograms)
                            np.Defines.Add(c => ((UnityPlayerConfiguration)c).ScriptingBackend == value,
                                $"{ScriptingBackendDefineFor(value)}=1");
                    }

                    var modulesWithManagedCodeAssemblyLocations = ModulesWithManagedCode
                        .Where(m => !m.IsEditorModule)
                        .Select(m => FinalModuleAssemblyLocationFor(variation, m))
                        .ToArray();

                    var unityEngineTypeForwarders = TypeForwarderGeneratorTool.Instance()
                        .SetupInvocation(
                            ArtifactsVariationDirFor(variation).Combine("UnityEngineTypeForwarders.cs"),
                            modulesWithManagedCodeAssemblyLocations);

                    var unityEngine = new CSharpAssemblyBuilder()
                        .WithPath(UnityEngineLocationFor(variation))
                        .WithSource(unityEngineTypeForwarders)
                        .WithPrebuiltReference(modulesWithManagedCodeAssemblyLocations)
                        .WithCopyReferencesNextToTarget(false)
                        .Complete();

                    new JamTarget("AllEngineAssemblies").DependOn(unityEngine.Path);
                    variationVirtualTarget.DependOn(unityEngine.Path);
                }

                CopyVariationManagedAssembliesIntoTopLevelManagedFolder();

                PostProcessNativeProgram(PlayerProgram);

                AllNativePrograms.ForEach(np => np.ExtraDependenciesForAllObjectFiles.Add(DummyCodeGeneratorHeader));

                PlayerProgram.NonLumpableFiles.Add(NonLumpableFiles.LumpingBlacklist.ToNPaths());

                var buildCmdPrefix = $"perl {Paths.UnityRoot}/jam.pl";
                if (GlobalVariables.Singleton["USE_DOWIT"].JamEquals("1"))
                    buildCmdPrefix = $@"dowit buildproject -- {buildCmdPrefix}";
                PlayerProgram.CommandToBuild.Set(c => $"{buildCmdPrefix} {VariationVirtualTargetFor((UnityPlayerConfiguration)c)} -sCONFIG={c.CodeGen.ToString().ToLower()}");

                foreach (var variation in VariationsToSetup)
                    using (TinyProfiler.Section($"SetupVariation {variation}"))
                        SetupVariation(variation);
            }
        }

        protected virtual void CopyVariationManagedAssembliesIntoTopLevelManagedFolder()
        {
            var firstVariationPerScriptingBackend =
                VariationsToSetup.GroupBy(v => v.ScriptingBackend).ToDictionary(v => v.Key, v => v.First());

            // Copy managed assemblies into a place where the editor would find them.
            var variationsToCopyManagedCodeFrom = ManagedAssembliesForEditorPerScriptingBackend
                ? firstVariationPerScriptingBackend.Values.ToArray()
                : new[] {VariationsToSetup.First()};

            foreach (var variation in variationsToCopyManagedCodeFrom)
            {
                var managedPath = ManagedAssembliesLocationForEditor(variation);
                foreach (var module in ModulesWithManagedCode)
                {
                    var assemblyLocation = FinalModuleAssemblyLocationFor(variation, module);
                    CommonVariationDependencies.DependOn(Copy.Setup(assemblyLocation,
                        BuildFolder.Combine(managedPath, assemblyLocation.FileName)));
                }

                CommonVariationDependencies.DependOn(Copy.Setup(UnityEngineLocationFor(variation),
                    BuildFolder.Combine(managedPath, "UnityEngine.dll")));
            }
        }

        protected static string GetModuleDllName(ModuleBase module)
        {
            if (module.Name == "Editor")
                return "UnityEditor";

            var moduleDllPrefix = module.IsEditorModule ? "UnityEditor" : "UnityEngine";
            var moduleDllSuffix = module.Name.EndsWith("Editor") ? module.Name.StripEnd("Editor") : module.Name;
            return $"{moduleDllPrefix}.{moduleDllSuffix}Module";
        }

        protected static string ModuleICallRegistrationFileBodyFor(ModuleBase module)
        {
            var bindingFunctionNames = module.LegacyBindings
                .Select(l => l.FileNameWithoutExtension + "Bindings").Concat(module.Cs.ForAny()
                    .Where(cs => cs.ToString().EndsWith(".bindings.cs"))
                    .Select(cs2 => cs2.FileName.StripEnd(".bindings.cs") + "BindingsNew"));

            var body = new StringBuilder();
            body.AppendLine("#include \"UnityPrefix.h\"");
            body.AppendLine("#if UNITY_EDITOR");
            body.AppendLine("#include \"Editor/Src/BuildPipeline/ModuleMetadata.h\"");
            body.AppendLine("#endif");

            foreach (var bindingFunctionName in bindingFunctionNames)
                body.AppendLine($"void Export{bindingFunctionName}();");

            body.AppendLine($"void RegisterModuleICalls_{module.Name}()");
            body.AppendLine("{");
            body.AppendLine("#if !INTERNAL_CALL_STRIPPING");
            body.AppendLine("#if UNITY_EDITOR");
            body.AppendLine($"\tModuleMetadata::Get().SetActiveModule(\"{module.Name}\");");
            body.AppendLine("#endif");

            foreach (var bindingFunctionName in bindingFunctionNames)
                body.AppendLine($"\tExport{bindingFunctionName}();");

            body.AppendLine("#if UNITY_EDITOR");
            body.AppendLine($"\tModuleMetadata::Get().SetActiveModule(NULL);");
            body.AppendLine("#endif");
            body.AppendLine("#endif");
            body.AppendLine("}");
            return body.ToString();
        }

        /*
         * N.B. This is the buildsystem V2 version of SetupRuntimeModules2.BuildModuleRegistrationCpp. If you modify this, please
         * make the corresponding change in V1, to keep things consistent. This is a temporary situation
         * while we phase out V1.
         */
        protected string ModuleRegistrationCppFileBodyFor(IEnumerable<ModuleBase> modules)
        {
            var moduleRegistrationContents = new StringBuilder();

            moduleRegistrationContents.Append("#include \"UnityPrefix.h\"\n");
            moduleRegistrationContents.Append("#include \"Runtime/Modules/ModuleRegistration.h\"\n");

            foreach (var module in modules)
                moduleRegistrationContents.Append($"MODULEMANAGER_REGISTER_LINKAGE void RegisterModule_{module.Name}();\n");

            moduleRegistrationContents.Append("void RegisterModulesGenerated() {\n");
            foreach (var module in modules)
            {
                var mode = ModuleBuildModeFor(module);

                if (mode == ModuleBuildMode.DynamicLibary)
                    moduleRegistrationContents.Append($"\tRegisterModuleInDynamicLibrary(\"{module.Name}\");\n");
                else
                {
                    if (module.IsStrippable)
                        moduleRegistrationContents.Append($"\t#if !SUPPORTS_GRANULAR_MODULE_REGISTRATION\n");
                    moduleRegistrationContents.Append($"\tRegisterModule_{module.Name}();\n");
                    if (module.IsStrippable)
                        moduleRegistrationContents.Append($"\t#endif\n");
                }
            }
            moduleRegistrationContents.Append("}\n");

            moduleRegistrationContents.Append($"#if SUPPORTS_GRANULAR_CLASS_REGISTRATION\n");
            foreach (var module in modules)
                moduleRegistrationContents.Append($"void RegisterModuleClasses_{module.Name}();\n");

            moduleRegistrationContents.Append("void RegisterStaticallyLinkedModuleClasses() {\n");

            foreach (var module in modules)
                moduleRegistrationContents.Append($"\tRegisterModuleClasses_{module.Name}();\n");

            moduleRegistrationContents.Append("}\n");
            moduleRegistrationContents.Append("#endif\n");


            return moduleRegistrationContents.ToString();
        }

        private IEnumerable<NPath> TemporarilyRequiredDependenciesOf()
        {
            //The registrations generator is written using reflection instead of using cecil.
            //this has the unfortunate side effect that all of each's assemblies dependencies needs
            //to be resolvable. The real fix here is to rewrite registrationgenerator to use cecil.
            //when that is done, we can remove all of the dependencies that we go find here.
            var deps = ModulesWithManagedCode.SelectMany(m => m.ManagedPrebuiltReference.ForAny()).Distinct();
            var systemDeps = ModulesWithManagedCode.SelectMany(m => m.ManagedSystemReference.ForAny()).Distinct();
            var absoluteSystemDeps =
                deps.Concat(systemDeps.Select(d => new NPath($"External/Mono/builds/monodistribution/lib/mono/2.0/{d}.dll")));
            return absoluteSystemDeps;
        }

        protected virtual IEnumerable<ModuleBase> ProvideModules()
        {
            var provideModules = ModuleNames
                .Where(m => !ModuleBlacklist.Contains(m))
                .Select(m =>
                    SetupRuntimeModules2.NewStyleModules.FirstOrDefault(module => module.Name == m) ??
                    LegacyModule.CreateModuleFromLegacyModule(m, LegacyTargetName, VariationsToSetup))
                .Append(new CoreModule())
                .ToList();

            /*
             * Todo: figure out why v1 buildcode adds AssemblyInfo.cs to every module. We need to do that here now too, as ther's a RegistrationGeneratorTool
             * thing that doesn't seem to work with modules that have no managed code, and the fact that AssemblyInfo.cs gets added everywhere means that every
             * assembly has managed code so we never notice this problem. for now let's just keep v2 in line with v1, and clean this up later.
             */
            foreach (var module in provideModules)
            {
                if (!module.Cs.ForAny().Contains("Runtime/Export/AssemblyInfo.cs"))
                    module.Cs.Add("Runtime/Export/AssemblyInfo.cs");
            }

            return provideModules;
        }

        protected abstract IEnumerable<string> ModuleNames { get; }

        private void ApplyIncludeDirsAndDefinesOfModuleToCpp(UnityPlayerConfiguration variation, ModuleBase module, params NPath[] moduleCpp)
        {
            var nativeProgramForModule = NativeProgramForModule(module);

            nativeProgramForModule.PerFileDefines.Add(c => c == variation,
                module.Defines.For(variation), moduleCpp);
            nativeProgramForModule.PerFileIncludeDirectories.Add(c => c == variation,
                module.IncludeDirectories.For(variation), moduleCpp);
        }

        // V1 buildcode target name for this platform; mostly used to make
        // old style build command lines map to new ones: e.g.
        // "MacStandalonePlayer -sSCRIPTING_BACKEND=il2cpp" --> "MacPlayerIL2CPP"
        protected abstract string LegacyTargetName { get; }

        // Some platforms have more that one legacy/V1 style build target name,
        // e.g. iOS has both "iOSPlayer" and "iOSSimulatorPlayer" that should setup
        // build of some variations
        protected virtual string[] AllBuildLegacyTargetNames => new[] {LegacyTargetName};


        protected virtual void SetupModule(ModuleBase module)
        {
            if (module is CoreModule coreModule)
            {
                coreModule.Cpp.Add(c => c.ScriptingBackend == ScriptingBackend.Mono,
                    c => FilterCoreModuleSources(RuntimeFiles.RuntimeSourcesList("runtime", "mono", "todo", null, true).Select(e => new NPath(e)), c));

                coreModule.Cpp.Add(c => c.ScriptingBackend == ScriptingBackend.IL2CPP,
                    c => FilterCoreModuleSources(RuntimeFiles.RuntimeSourcesList("runtime", "il2cpp", "todo", null, true).Select(e => new NPath(e)), c));

                coreModule.Cpp.Add(c => FilterCoreModuleSources(PlatformDependentFolders
                    .SelectMany(p => new NPath("PlatformDependent").Combine(p).Files()).ToArray(), c));

                coreModule.Cpp.Add(TextFile.Setup(
                    CodeGeneratorDir.Combine("ModuleRegistration.cpp"),
                    ModuleRegistrationCppFileBodyFor(ModulesForICallRegistration)
                ));
            }
        }

        protected virtual IEnumerable<ModuleBase> ModulesForICallRegistration => ProvideModules();

        protected IEnumerable<NativeProgram> AllNativePrograms => ModuleNativePrograms.Append(PlayerProgram);

        protected NPath CodeGeneratorDir => ArtifactsDir.Combine("codegenerator");

        private NPath[] GlobalIncludeDirectories()
        {
            return new[] {"", "Projects/PrecompiledHeaders", CodeGeneratorDir};
        }

        protected JamTarget VariationVirtualTargetFor(UnityPlayerConfiguration variation) => new JamTarget(variation.HumanTargetName);

        protected virtual NPath UnityEngineLocationFor(UnityPlayerConfiguration variation) => BuildVariationDirFor(variation).Combine("Data/Managed/UnityEngine.dll");

        protected NPath ArtifactsVariationDirFor(UnityPlayerConfiguration config) => $"{ArtifactsDir}/Variations/{config.Identifier}";

        protected virtual NPath BuildVariationDirFor(UnityPlayerConfiguration config) => BuildVariationDirFor(config.Identifier);

        protected NPath BuildVariationDirFor(string configIdentifier) => $"{BuildFolder}/Variations/{configIdentifier}";

        protected IEnumerable<ModuleBase> ModulesWithManagedCode => Modules.Where(m => m.HasManagedCode);

        protected virtual NPath FinalModuleAssemblyLocationFor(UnityPlayerConfiguration variation, ModuleBase module) =>
            BuildVariationDirFor(variation).Combine($"Data/Managed/{GetModuleDllName(module)}.dll");

        protected string ScriptingBackendDefineFor(ScriptingBackend backend)
        {
            switch (backend)
            {
                case ScriptingBackend.Mono:
                    return "ENABLE_MONO";
                case ScriptingBackend.IL2CPP:
                    return "ENABLE_IL2CPP";
                case ScriptingBackend.DotNet:
                    return "ENABLE_DOTNET";
                default:
                    throw new ArgumentOutOfRangeException(nameof(backend), backend, null);
            }
        }

        protected abstract string PlatformIdentifierForBindingsGenerator { get; }

        public readonly UnityPlayerConfiguration[] VariationsToSetup;

        public NativeProgram PlayerProgram;
        protected string DummyCodeGeneratorHeader;
        protected UnityPlayerConfiguration[] AllPossibleVariations;
        public UnityPlayerConfiguration DefaultDeveloperConfiguration => AllPossibleVariations.First(HasRequiredFlags);

        protected abstract IEnumerable<UnityPlayerConfiguration> ProvideVariations();

        protected NPath ArtifactsDir => new NPath($"artifacts/{PlatformName}");

        protected virtual string[] ModuleBlacklist { get; } = Array.Empty<string>();

        protected abstract string[] PlatformDependentFolders { get; }

        protected virtual void PostProcessNativeProgram(NativeProgram app)
        {
            foreach (var nativeProgram in AllNativePrograms)
            {
                WarningSettings.SetupWarningFlagsFor(nativeProgram);
                nativeProgram.Defines.Add(c => c.CodeGen == CodeGen.Debug, "UNITY_RELEASE=0", "DEBUGMODE=1");
                nativeProgram.Defines.Add(c => c.CodeGen == CodeGen.Release, "UNITY_RELEASE=1", "DEBUGMODE=0");
            }
        }

        protected abstract IEnumerable<string> Defines { get; }

        protected abstract string PlayerBinaryName { get; }

        protected virtual void SetupVariation(UnityPlayerConfiguration variation)
        {
            SetupSpecificProgramsForModules(variation);
        }

        protected virtual NPath BuildFolder => new NPath($"build/{GetType().Name}");

        protected abstract JamList CsharpPlatformDefines { get; }
        protected static FlagRequirement DebugFlagRequirement { get; } = new FlagRequirement() {Flag = "CONFIG", Value = "debug"};
        protected static FlagRequirement ReleaseFlagRequirement { get; } = new FlagRequirement() {Flag = "CONFIG", Value = "release"};

        static string InternalsVisibleToFileContentsFor(IEnumerable<ModuleBase> modulesWithManagedCode)
        {
            var sb = new StringBuilder("//This is a generated file");
            sb.AppendLine();
            sb.AppendLine("using System.Runtime.CompilerServices;");

            sb.AppendLine($"[assembly: InternalsVisibleTo(\"UnityEngine\")]");
            foreach (var module in modulesWithManagedCode)
            {
                sb.AppendLine($"[assembly: InternalsVisibleTo(\"{GetModuleDllName(module)}\")]");
            }
            return sb.ToString();
        }

        static NPath BindingOutputDirectoryFor(ModuleBase module, NPath artifactsDir) =>
            artifactsDir.Combine($"modules/{module.Name}");

        protected virtual NPath[] FilterCoreModuleSources(IEnumerable<NPath> nPaths, UnityPlayerConfiguration unityPlayerConfiguration)
        {
            return nPaths.ToArray();
        }

        protected void SetupSpecificProgramsForModules(UnityPlayerConfiguration variation)
        {
            var variationVirtualTarget = VariationVirtualTargetFor(variation);
            var specificPrograms = new Dictionary<ModuleBase, JamTarget>();
            foreach (var module in Modules.OrderByDependencies())
            {
                var mode = ModuleBuildModeFor(module);
                var nativeProgramForModule = NativeProgramForModule(module);
                nativeProgramForModule.DefaultPCH = DefaultPrefixPch;

                var staticLibraryDependencies = module.StaticLibraries.For(variation).ToList();
                var dynamicLibraryDependencies = module.DynamicLibraries.For(variation).ToList();


                nativeProgramForModule.StaticLibraries.Add(staticLibraryDependencies);
                nativeProgramForModule.PrebuiltLibraries.Add(dynamicLibraryDependencies.Select(l => new DynamicLibrary(l)));

                nativeProgramForModule.PerFilePchs.Add(null, RuntimeFiles.SourcesNoPCH.ToNPaths());

                if (mode == ModuleBuildMode.Source)
                    continue;

                foreach (var d in module.Dependencies.For(variation))
                {
                    nativeProgramForModule.PrebuiltLibraries.Add(new LegacyCApplicationLibrary(specificPrograms[ModuleByName(d)]));
                }

                var format = mode == ModuleBuildMode.StaticLibrary
                    ? variation.ToolChain.StaticLibraryFormat
                    : variation.ToolChain.DynamicLibraryFormat;


                var specific = nativeProgramForModule.SetupSpecificConfiguration(variation, format);

                specificPrograms[module] = specific;

                variationVirtualTarget.DependOn(specific);

                if (mode == ModuleBuildMode.StaticLibrary)
                {
                    PlayerProgram.PrebuiltLibraries.Add(c => c == variation, module.Frameworks.For(variation).Select(l => new SystemFramework(l)));
                    PlayerProgram.PrebuiltLibraries.Add(c => c == variation, module.PrebuiltLibraries.For(variation).Select(l => new LegacyCApplicationLibrary(l)));
                    PlayerProgram.PrebuiltLibraries.Add(c => c == variation, (StaticLibrary)specific);
                }
                else
                    PlayerProgram.PrebuiltLibraries.Add(c => c == variation, (DynamicLibrary)specific);
            }
        }

        IEnumerable<UnityPlayerConfiguration> FilteredVariationsForCurrentCommandLine(IEnumerable<UnityPlayerConfiguration> variations)
        {
            // when generating project files or native deps, don't return all possible variations;
            // the code will proceed to setup only default one, which is enough for our purposes
            if (ProjectFiles.IsRequested)
                return Array.Empty<UnityPlayerConfiguration>();

            var legacyTarget = GetLegacyCommandLineTarget();
            if (!string.IsNullOrEmpty(legacyTarget))
            {
                var matchingLegacy = variations.Where(v => HasRequiredFlags(v) && MatchVariationWithLegacyCommandLineArgs(v)).ToArray();
                if (matchingLegacy.Any())
                {
                    var targetNames = new JamList(matchingLegacy.Select(v => VariationVirtualTargetFor(v).JamTargetName));
                    BuiltinRules.Depends(legacyTarget, targetNames);
                    Errors.PrintWarning($"Legacy {PlatformName} build target name used, please use './jam {string.Join(" ", targetNames)}' instead.");
                }
                else
                {
                    Errors.Exit($"{PlatformName} build; given legacy build arguments don't map to any player variation");
                }
                return matchingLegacy;
            }

            var targets = ConvertedJamFile.Vars.JAM_COMMAND_LINE_TARGETS;
            var matching = variations.Where(v => targets.Contains(v.HumanTargetName));
            return matching;
        }

        string GetLegacyCommandLineTarget()
        {
            // is legacy target name requested to build?
            var targets = ConvertedJamFile.Vars.JAM_COMMAND_LINE_TARGETS;
            var legacyTargets = targets.Intersect(AllBuildLegacyTargetNames).ToArray();
            if (!legacyTargets.Any())
                return null;

            var legacyTarget = legacyTargets.First();

            // if legacy had its own target name that is different from V2 ones,
            // then we know that legacy is requested here
            var legacyNameSameAsV2 = VariationPrefix == legacyTarget;
            if (!legacyNameSameAsV2)
                return legacyTarget;

            // both legacy and V2 use the same names; have to look at additional
            // arguments to see what's been requested
            if (AreLegacyBuildFlagsUsed())
                return legacyTarget;

            return null;
        }

        protected virtual bool AreLegacyBuildFlagsUsed()
        {
            var usedDevel = ConvertedJamFile.Vars.DEVELOPMENT_PLAYER != "1";
            var usedScripting = ConvertedJamFile.Vars.SCRIPTING_BACKEND.ToString().ToLowerInvariant() != "mono";
            return usedDevel || usedScripting;
        }

        protected virtual bool MatchVariationWithLegacyCommandLineArgs(UnityPlayerConfiguration variation)
        {
            return false;
        }

        protected void SetupCopyOfFolder(NPath directoryToCopy, NPath newDirectoryToHostCopy, JamTarget targetDependingOnCopy)
        {
            foreach (var file in directoryToCopy.Files(true))
                targetDependingOnCopy.DependOn(Copy.Setup(file,
                    newDirectoryToHostCopy.Combine(directoryToCopy.FileName)
                        .Combine(file.RelativeTo(directoryToCopy))));
        }

        private static string HumanTargetNameFor(UnityPlayerConfiguration configToCalculateNameFor, string prefix, UnityPlayerConfiguration[] allConfigs)
        {
            var result = new StringBuilder(prefix);
            var firstArch = allConfigs.FirstOrDefault()?.ToolChain.Architecture;
            var allArchs = allConfigs.Select(c => c.ToolChain.Architecture).DistinctBy(a => a.GetType()).ToArray();
            if (allArchs.Length > 0 && firstArch != null)
            {
                var arch = configToCalculateNameFor.ToolChain.Architecture;
                if (firstArch.GetType() == arch.GetType())
                {
                    // nothing to do; first (default) architecture does not get any suffix to make the name shorter
                }
                else if (firstArch.Bits != arch.Bits && firstArch.GetType().BaseType == arch.GetType().BaseType)
                {
                    // differ in bitness from default one, but same class of architectures (e.g. ARM 32 vs 64 bit): add bits as suffix
                    result.Append(arch.Bits);
                }
                else
                {
                    // other kind of difference: append full arch name (e.g. armv7 default; x86 present too: add x86)
                    result.Append(arch.Name);
                }
            }
            result.Append(DescriptionOfPropertyIfRequired(allConfigs, configToCalculateNameFor, c => c.ScriptingBackend, backend => backend.ToString().ToUpper()));
            result.Append(DescriptionOfPropertyIfRequired(allConfigs, configToCalculateNameFor, c => c.DevelopmentPlayer, development => development ? "Development" : "NoDevelopment"));

            //for unit tests, we do something special: the default for "unittests" is more complicated than looking at the default config. for most platforms it goes hand in hand with "developmentplayer".
            //there are some platforms though where this is not true (webgl). To check if this is one of those platforms, we say "the default for unit tests is enabled if both developmentplayer is enabled on that
            //config _and_ the default configuration also has unit tests enabled.
            result.Append(DescriptionOfPropertyIfRequired(allConfigs, configToCalculateNameFor, c => c.EnableUnitTests, v => v ? "UnitTests" : "NoUnitTests", c => c.DevelopmentPlayer && allConfigs[0].EnableUnitTests));

            result.Append(DescriptionOfPropertyIfRequired(allConfigs, configToCalculateNameFor, c => c.HeadlessPlayer, v => v ? "Headless" : "NoHeadless"));
            result.Append(DescriptionOfPropertyIfRequired(allConfigs, configToCalculateNameFor, c => c.IsThreaded, v => v ? "Threaded" : "NoThreads"));
            result.Append(DescriptionOfPropertyIfRequired(allConfigs, configToCalculateNameFor, c => c.StaticLibraryPlayer, v => v ? "Static" : "NoStatic"));
            result.Append(DescriptionOfPropertyIfRequired(allConfigs, configToCalculateNameFor, c => c.Lump, v => v ? "Lump" : "NoLump"));

            return result.ToString();
        }

        private static string DescriptionOfPropertyIfRequired<T>(UnityPlayerConfiguration[] allConfigs, UnityPlayerConfiguration configToCalculateNameFor, Func<UnityPlayerConfiguration, T> propertySelector, Func<T, string> resultStringProviderFunc, Func<UnityPlayerConfiguration, T> specialDefaultValueProvider = null)
        {
            var allBackends = allConfigs.Select(propertySelector).Distinct().ToArray();

            //We look at a few things when we calculate a targetname. for each property we look if all of the configurations that exist have the same value for that property.
            //if that is the case we don't need the property value part of the target name. (webgl only has il2cpp, its targetnames do not need to include il2cpp)
            if (!allBackends.Any()) return string.Empty;

            var defaultValue = specialDefaultValueProvider == null
                ? propertySelector(allConfigs[0])
                : specialDefaultValueProvider(configToCalculateNameFor);

            //if there are different configurations that have different values for this property, then we only want to specify the propertyvalue if it's different from the default.
            if (propertySelector(configToCalculateNameFor).Equals(defaultValue))
                return string.Empty;

            return resultStringProviderFunc(propertySelector(configToCalculateNameFor));
        }

        public static void HackDisableHololensModule()
        {
            // Herein lies a heinous hack.
            //
            // For reasons having to do with the difficulty of enabling and disabling modules per
            // platform (and per sub-platform, like hololens), the hololens people resorted to
            // setting this define to true in a lot of scenarios in
            // PlatformDependent/Win/Jam/PlatformDefines.jam.cs.
            //
            // That means that when we build on Windows, but the thing were are building has nothing to do with
            // hololens (e.g. WebGL or Android), it will pick up the hololens defines, and then tries and fails to
            // compile the hololens files.
            //
            // So, until we fix the situation with modules so that hololens does not have to
            // do that, this hack goes here.
            if (!ProjectFiles.IsRequested)
                ConvertedJamFile.Vars.ENABLE_HOLOLENS_MODULE.Assign("0");
        }

        /*
         * N.B. This is the buildsystem V2 version of SetupRuntimeModules2.BuildModuleList. If you modify this, please
         * make the corresponding change in V1, to keep things consistent. This is a temporary situation
         * while we phase out V1.
         */
        protected string ModuleAssetsContent()
        {
            var sb = new StringBuilder(@"%YAML 1.1
%TAG !u! tag:unity3d.com,2011:
--- !u!877146078 &1
PlatformModuleSetup:
  m_ObjectHideFlags: 0
  modules:
");
            foreach (var module in Modules)
            {
                sb.AppendLine($"  - name: {module.Name}");
                var moduleDependencies = module.Dependencies.For(DefaultDeveloperConfiguration).ToList();

                if (!moduleDependencies.Any())
                {
                    sb.AppendLine($"    dependencies: []");
                }
                else
                {
                    sb.AppendLine($"    dependencies:");
                    foreach (var moduleDep in moduleDependencies)
                        sb.AppendLine($"      - {moduleDep}");
                }

                sb.AppendLine($"    strippable: {(module.IsStrippable ? "1" : "0")}");
                sb.AppendLine($"    controlledByBuiltinPackage: {(module.GenerateBuiltinPackage ? "1" : "0")}");
            }

            return sb.ToString();
        }
    }
}
