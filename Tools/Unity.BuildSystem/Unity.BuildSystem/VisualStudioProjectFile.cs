using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;
using Unity.BuildTools;
using Unity.TinyProfiling;
using ProjectFile = Unity.BuildSystem.NativeProgramSupport.ProjectFile;

namespace Unity.BuildSystem
{
    public class VisualStudioProjectFile
    {
        // Platform toolchains that use VS as the IDE can register how their name (e.g. "win32") maps to platform
        // that is defined in VS solution & project. Things to note:
        //
        // Platforms defined in the solution file can be arbitrary, but the IDE picks the first
        // alphabetically sorted, case insensitive one from the list as the default. So if you have 32 & 64 bit Windows
        // and want 64 bit to be default, then naming them as "x64" and "x86" or similar does the trick (but naming them "win32" and
        // "win64" would mean that 32 bit is default since it sorts first).
        //
        // Platforms defined in the project files can not be arbitrary names; their names have to be tied into supported
        // compiler toolchains (e.g. "Win32" and "x64" that come out of the box with VS).
        //
        // Note: If adding new platforms, check if FindClosestConfig logic needs to be updated.
        //
        // TODO/Note:
        // would be nice to register the names in a more modular way (e.g. from platform compiler toolchains), but
        // today only the platform being "globally active" is ever evaluated/executed, making it not know about for example
        // both "win32" and "win64" mapping at the same time.
        static readonly Dictionary<string, Tuple<string, string>> platformToVSSolutionAndProjectPlatformName = new Dictionary<string, Tuple<string, string>>()
        {
            // Windows
            { "win32", Tuple.Create("x86", "Win32") },
            { "win64", Tuple.Create("x64", "x64") },
            // WinStore/Phone
            { "uap_arm", Tuple.Create("xARM", "ARM") }, // xARM so that it sorts after x64/x86 for VS default platform pick
            { "uap_arm64", Tuple.Create("xARM64", "ARM64") }, // xARM64 so that it sorts after x64/x86 for VS default platform pick
            { "uap_x64", Tuple.Create("x64", "x64") },
            { "uap_x86", Tuple.Create("x86", "Win32") },
            // other platforms
            { "android-arm7", Tuple.Create("xARM", "ARM") },
            { "android-arm8", Tuple.Create("xARM64", "ARM64") },
            { "android-x86", Tuple.Create("x86", "Win32") },
            { "android-x64", Tuple.Create("x64", "x64") },
            { "webgl", Tuple.Create("x64", "x64") },
            { "orbis", Tuple.Create("x64", "ORBIS") },
            { "durango", Tuple.Create("x64", "Durango") },
            { "nx64", Tuple.Create("x64", "NX64") },
        };

        static string PlatformToVSSolutionPlatform(string plat)
        {
            if (platformToVSSolutionAndProjectPlatformName.ContainsKey(plat))
                return platformToVSSolutionAndProjectPlatformName[plat].Item1;
            throw new ArgumentException($"VS solution platform name for '{plat}' is not known. Is it included in platformToVSSolutionAndProjectPlatformName?");
        }

        static string PlatformToVSProjectPlatform(string plat)
        {
            if (platformToVSSolutionAndProjectPlatformName.ContainsKey(plat))
                return platformToVSSolutionAndProjectPlatformName[plat].Item2;
            throw new ArgumentException($"VS project platform name for '{plat}' is not known. Is it included in platformToVSProjectPlatform?");
        }

        // Some platforms/toolsets might need to generate extra things or customize what is written into generated project files
        class PlatformVSSettings
        {
            public string vcxprojUserFileContents = "";
            // if VS2017 is installed on the system (not in our versioned toolchains!), assume that's
            // what will be used to edit the project files, and generate them with v141 toolset version.
            // Makes VS IDE stop showing "visual studio 2015" next to each project, and silences the
            // "Microsoft.Cpp.props(31,3): warning MSB4011: Microsoft.Makefile.props cannot be imported again" IDE warning.
            public string platformToolset =  MsvcInstallation.GetVisualStudioInstallationFolder(new Version(15, 0)) != null ? "v141" : "v140";
            public string msbuildPlatformImport = "";
            public string extraGlobalTags = "";
            public string extraTailTags = "";
            public string outDir = Paths.UnityRoot.Combine("artifacts").ToString(SlashMode.Backward);
        }

        static string GetVcxprojUserContents(NativeProgram np, NPath projectsFolder, string debuggerType = "WindowsLocalDebugger")
        {
            var defaultConfig = np.ValidConfigurations.First();
            NPath workingDir = np.OutputDirectory.For(defaultConfig);
            workingDir = workingDir.RelativeTo(projectsFolder);

            // .vcxproj.user file with default debug settings pointing to correct location
            return $@"<?xml version=""1.0"" encoding=""utf-8""?>
<Project ToolsVersion=""12.0"" xmlns=""http://schemas.microsoft.com/developer/msbuild/2003"">
<PropertyGroup>
<LocalDebuggerWorkingDirectory>$(ProjectDir){workingDir}</LocalDebuggerWorkingDirectory>
<DebuggerFlavor>{debuggerType}</DebuggerFlavor>
</PropertyGroup>
</Project>
";
        }

        public static string XdkVersion { get; set; } = "";

        PlatformVSSettings GetPlatformSettingsFor(NativeProgram np)
        {
            var defaultConfig = np.ValidConfigurations.First();
            // Base various settings on the platform of default build configuration. So far this covers the cases we
            // actually end up having.
            var platform = defaultConfig.ToolChain.LegacyPlatformIdentifier;
            switch (platform)
            {
                case "orbis": // PS4 specific settings
                    return new PlatformVSSettings
                    {
                        vcxprojUserFileContents = GetVcxprojUserContents(np, ProjectsFolderPath, "ORBISDebugger"),
                        platformToolset = "v110", // default v140 makes VS give warnings about "missing clang"
                        msbuildPlatformImport =
                            "<Import Condition=\"'$(ConfigurationType)' == 'Makefile' and Exists('$(VCTargetsPath)\\Platforms\\$(Platform)\\SCE.Makefile.$(Platform).targets')\" Project=\"$(VCTargetsPath)\\Platforms\\$(Platform)\\SCE.Makefile.$(Platform).targets\" />"
                    };
                case "durango": // XboxOne specific settings
                    return new PlatformVSSettings
                    {
                        extraGlobalTags = $"<TargetRuntime>Native</TargetRuntime><ApplicationEnvironment>title</ApplicationEnvironment><XdkEditionTarget>{XdkVersion}</XdkEditionTarget>",
                        extraTailTags = "<Target Name=\"GeneratePackageRuntimeMetadata\"/>\n",
                        // do not set OutDir to actual artifacts folder; doing that would make VS copy everything under it into "Layout" folder to be pushed onto the device
                        outDir = Paths.UnityRoot.Combine("artifacts/XboxOneDummyOutputFolder").ToString(SlashMode.Backward)
                    };

                default:
                    if (platform.StartsWith("android")) // Android specific settings
                    {
                        return new PlatformVSSettings
                        {
                            // Visual Studio Makefile projects don't support ARM64 platform. Workaround by setting PlatformTargetsFound property to True.
                            // Source: https://stackoverflow.com/questions/38821968/custom-platforms-for-visual-studio-makefile-project
                            msbuildPlatformImport = "<PropertyGroup><PlatformTargetsFound>True</PlatformTargetsFound></PropertyGroup>",
                        };
                    }
                    return new PlatformVSSettings();
            }
        }

        // Configurations present in external .csproj files we might want to add
        static readonly string[] ManagedProjectConfigs = {"Debug", "Release"};

        private IEnumerable<NPath> ExtraSourceFiles { get; }
        private string[] ManagedSourceFiles { get; }
        private NPath SolutionFolder { get; }
        private const string ProjectsFolderName = "Projects";
        private NPath ProjectsFolderPath => $"{SolutionFolder}/{ProjectsFolderName}";

        public VisualStudioProjectFile(NPath solutionFolder,
                                       Dictionary<string, SolutionProjects> solutions,
                                       IEnumerable<NPath> extraSourceFiles,
                                       Dictionary<string, string> folderStructure,
                                       string[] managedProjects,
                                       string[] managedSourceFiles)
        {
            SolutionFolder = solutionFolder;
            ExtraSourceFiles = extraSourceFiles ?? Array.Empty<NPath>();
            ManagedSourceFiles = managedSourceFiles;
            using (TinyProfiler.Section("VSProject"))
            {
                GenerateProjectFiles(solutions, folderStructure, managedProjects);
            }
        }

        static void WriteContentsToFile(NPath path, string contents)
        {
            Backend.Current.AddWriteTextAction(path, contents);
            Backend.Current.AddAliasDependency("NativeProjectFiles", path);
        }

        void GenerateProjectFiles(Dictionary<string, SolutionProjects> solutions, Dictionary<string, string> folderStructure, string[] managedProjects)
        {
            var allPrograms = solutions.SelectMany(s => s.Value.native).Distinct().ToArray();

            foreach (var p in allPrograms)
            {
                if (p.ValidConfigurations == null)
                    throw new Exception($"Native program '{p.Name}' has no valid configurations set up");
            }

            var managedSourceFiles = ManagedSourceFiles.Select(p => new NPath(p)).ToList();
            foreach (var sln in solutions)
            {
                var slnPrograms = sln.Value.native.ToArray();
                managedSourceFiles.AddRange(GenerateSolutionFile(sln.Key, slnPrograms, folderStructure, sln.Value.managed.ToArray(), managedProjects));
            }

            // From the "extra source files to include", we only want to add to EditorApp project if no other project
            // already contained that file. So first go over all other projects, and then over EditorApp one.
            var editorProgram = allPrograms.Where(p => p.Name == "EditorApp").ToArray();
            var alreadyIncludedSources = new HashSet<NPath>();

            alreadyIncludedSources.UnionWith(managedSourceFiles);

            foreach (var p in allPrograms.Except(editorProgram))
            {
                var sourceFiles = GenerateProjectFile(p, false, alreadyIncludedSources);
                GenerateFiltersFile(p, sourceFiles);
            }
            foreach (var p in editorProgram)
            {
                var sourceFiles = GenerateProjectFile(p, true, alreadyIncludedSources);
                GenerateFiltersFile(p, sourceFiles);
            }
        }

        // Visual Studio has no way to indicate which configuration should be default in a freshly opened solution,
        // but uses logic along the lines of http://stackoverflow.com/a/41445987 to pick default one:
        //
        // Out of all configurations present:
        // 1. Sort them (almost) alphabetically, case insensitive,
        // 2. But put configs that start with "debug" before all others,
        // 3. Also config that is another one with a " Whatever" or "_Whatever" added, go before it. So "A_B" goes before
        //    "A"; or "Debug All" goes before "Debug" (but "DebugAll" goes after "Debug").
        // 4. And now pick the first one from the list!
        //
        // Our build configs are generally underscore-separated things, e.g. "debug_dev_il2cpp".
        // To make "debug" be default, transform names to use CamelCase and remove underscores.
        string ConfigurationToVSName(NativeProgramConfiguration config)
        {
            var name = config.IdentifierNoPlatform;
            var sb = new StringBuilder(name.Length);
            var uppercase = true;
            foreach (var c in name)
            {
                if (c == '_')
                {
                    uppercase = true;
                }
                else
                {
                    sb.Append(uppercase ? char.ToUpperInvariant(c) : c);
                    uppercase = false;
                }
            }
            return sb.ToString();
        }

        // Returns list of source files in the project
        List<NPath> GenerateProjectFile(NativeProgram program, bool includeExtraSourceFiles, HashSet<NPath> alreadyIncludedSources)
        {
            var configurations = program.ValidConfigurations.ToArray();

            var root = Paths.UnityRoot;
            var projectFolder = ProjectsFolderPath;

            var projectConfigs = new StringBuilder();
            var projectConfigSettings = new StringBuilder();

            var sourceFiles = program.Sources.ForAny().SelectMany(ProjectFile.SourcePathToFiles).ToList();
            if (includeExtraSourceFiles)
            {
                var extraFiles = ExtraSourceFiles
                    .Where(p => !alreadyIncludedSources.Contains(p))
                    .ToArray();
                sourceFiles.AddRange(extraFiles);
            }
            sourceFiles = sourceFiles.Distinct().ToList();

            sourceFiles.Sort(); // VS seems to load projects a bit faster if file/folder lists are sorted
            alreadyIncludedSources.UnionWith(sourceFiles);

            var platformSettings = GetPlatformSettingsFor(program);

            foreach (var config in configurations)
            {
                var cfgPlatform = PlatformToVSProjectPlatform(config.ToolChain.LegacyPlatformIdentifier);
                var cfgVS = ConfigurationToVSName(config);
                projectConfigs.Append(
                    $@"    <ProjectConfiguration Include=""{cfgVS}|{cfgPlatform}"">
      <Configuration>{cfgVS}</Configuration>
      <Platform>{cfgPlatform}</Platform>
    </ProjectConfiguration>
");

                var defines = program.Defines.For(config).SeparateWith(";");

                // VS Makefile type projects can only specify include folders globally. So take the global ones for whole app,
                // and add in any coming from per-file/per-module settings.
                var globalIncludeFolders = program.IncludeDirectories.For(config);
                var toolchainIncludes = program.ValidConfigurations.First().ToolChain.Sdk.IncludePaths;
                globalIncludeFolders = globalIncludeFolders.Union(toolchainIncludes);
                var fileIncludeFolders = program.PerFileIncludeDirectories.For(config).Values.SelectMany(p => p).Distinct();
                var allIncludes = globalIncludeFolders.Union(fileIncludeFolders);

                var includes = allIncludes.Select(p => (p.IsRelative ? root.Combine(p) : p).ToString(SlashMode.Backward).InQuotes()).SeparateWith(";");

                var outputFolder = program.OutputDirectory.For(config);

                var partialDebugCommand = program.DebugCommand.For(config);
                string outputName = null;
                string debugCommand = null;
                if (!string.IsNullOrEmpty(partialDebugCommand))
                {
                    debugCommand = Paths.UnityRoot.Combine(partialDebugCommand).ToString();
                    outputName = debugCommand;
                }
                else
                    outputName = program.OutputName.For(config);


                // OutputNameFor will only return something for projects setup via C.Application jamplus rules;
                // for things done via NativeProgram must query OutputName / Name
                if (string.IsNullOrEmpty(outputName))
                    outputName = program.OutputName.For(config);
                if (string.IsNullOrEmpty(outputName) && outputFolder != null)
                    outputName = program.Name;
                string outputPath = "";
                if (!string.IsNullOrEmpty(outputName) && outputFolder != null)
                {
                    if (outputFolder.IsRelative)
                        outputFolder = Paths.UnityRoot.Combine(outputFolder);

                    outputPath = debugCommand ?? $"{outputFolder}/{outputName}";

                    var ext = program.OutputExtension.For(config);
                    if (ext != null && outputPath.Length > 0)
                        outputPath = outputPath + "." + ext.TrimStart('.');
                    if (ext == null && !(new NPath(outputName).HasExtension()))
                        outputPath = outputPath + ".exe";
                }

                var usingAssemblies = "";
                var additionalOptions = "";
                if (program.WinRTExtensions.For(config))
                {
                    // Note: WinRT assembly paths are *not* ';' seperated, they need a ';' after the last element too!
                    // How to check: open UWP solution in VS, open AppCallbacks.cpp, and ensure that "AppC::EventHandler" is syntax colored.
                    // Also open ScriptingApi_DotNet.cpp and ensure "WinRT::Bridge::GetUtils()->GCMaxGeneration" is correctly colored.
                    usingAssemblies += WindowsSDKs.WinRTWindowsMetadataPath.ToString(SlashMode.Native) + ";";
                    usingAssemblies += MsvcInstallation.GetLatestInstalled().WinRTPlatformMetadata.ToString(SlashMode.Native) + ";";
                    usingAssemblies += program.WinRTAssemblies.For(config).Select(p => p.ToString(SlashMode.Native) + ";").SeparateWith("");
                    additionalOptions = "/ZW /ZW:nostdlib";
                }

                string cmd = program.CommandToBuild.For(config);
                string NMakeBuildCmdLine = cmd;
                // remove the last quote, if it exists, and append -a.
                string NMakeReBuildCmdLine = cmd.TrimEnd('"') + " -a";
                // if the original command ended with a quote then add it back
                if (cmd.EndsWith("\""))
                {
                    NMakeReBuildCmdLine += "\"";
                }

                projectConfigSettings.Append(
                    $@"  <PropertyGroup Condition=""'$(Configuration)|$(Platform)'=='{cfgVS}|{cfgPlatform}'"" Label=""Configuration"">
    <ConfigurationType>Makefile</ConfigurationType>
    <BuildLogFile>{root}\artifacts\logs\{config.IdentifierForProjectFile}\$(MSBuildProjectName).log</BuildLogFile>
    <NMakeBuildCommandLine>{NMakeBuildCmdLine}</NMakeBuildCommandLine>
    <NMakeOutput>{outputPath}</NMakeOutput>
    <NMakeReBuildCommandLine>{NMakeReBuildCmdLine}</NMakeReBuildCommandLine>
    <NMakePreprocessorDefinitions>{defines}</NMakePreprocessorDefinitions>
    <NMakeIncludeSearchPath>{includes}</NMakeIncludeSearchPath>
    <NMakeForcedUsingAssemblies>{usingAssemblies}</NMakeForcedUsingAssemblies>
    <AdditionalOptions>{additionalOptions}</AdditionalOptions>
    <OutDir>{platformSettings.outDir}\</OutDir>
    <IntDir>{root}\artifacts\</IntDir>
  </PropertyGroup>
");
            }

            var files = new StringBuilder();
            foreach (var f in sourceFiles)
            {
                if (f.Extension == "natvis")
                    continue;
                files.Append($"    <None Include=\"{f.RelativeTo(projectFolder).ToString(SlashMode.Backward)}\" />\n");
            }
            var natvisFiles = new StringBuilder();
            foreach (var f in sourceFiles)
            {
                if (f.Extension != "natvis")
                    continue;
                natvisFiles.Append($"    <Natvis Include=\"{f.RelativeTo(projectFolder).ToString(SlashMode.Backward)}\" />\n");
            }
            if (natvisFiles.Length > 0)
            {
                natvisFiles.Insert(0, "  <ItemGroup>\n");
                natvisFiles.Append("  </ItemGroup>\n");
            }

            var contents =
                $@"<?xml version=""1.0"" encoding=""utf-8""?>
<Project DefaultTargets=""Build"" ToolsVersion=""15.0"" xmlns=""http://schemas.microsoft.com/developer/msbuild/2003"">
  <ItemGroup Label=""ProjectConfigurations"">
{projectConfigs}
  </ItemGroup>
  <PropertyGroup Label=""Globals"">
    <VCProjectVersion>15.0</VCProjectVersion>
    <ProjectGUID>{{{ProjectFile.GuidForName(program.Name)}}}</ProjectGUID>
    <Keyword>MakeFileProj</Keyword>
    <ProjectName>{program.Name}</ProjectName>
    <PlatformToolset>{platformSettings.platformToolset}</PlatformToolset>{platformSettings.extraGlobalTags}
  </PropertyGroup>
  <Import Project=""$(VCTargetsPath)\Microsoft.Cpp.Default.props"" />
{projectConfigSettings}
  <Import Project=""$(VCTargetsPath)\Microsoft.Cpp.props"" />
  <ItemGroup>
{files}
  </ItemGroup>
{natvisFiles}{platformSettings.msbuildPlatformImport}
  <Import Project=""$(VCTargetsPath)\Microsoft.Cpp.targets"" />
{platformSettings.extraTailTags}</Project>
";
            WriteContentsToFile(projectFolder.Combine($"{program.Name}.vcxproj"), contents);
            if (!string.IsNullOrEmpty(platformSettings.vcxprojUserFileContents))
                WriteContentsToFile(projectFolder.Combine($"{program.Name}.vcxproj.user"), platformSettings.vcxprojUserFileContents);
            return sourceFiles;
        }

        void GenerateFiltersFile(NativeProgram program, List<NPath> sourceFiles)
        {
            var filtersFolder = ProjectsFolderPath;

            var allFolders = new HashSet<NPath>(sourceFiles.Select(f => f.Parent));

            // Some projects are entirely deep inside some subfolder, and it's nicer to have VS project not contain the
            // full deep nested folders path to get to the files. Find common prefix that is the same for all the paths,
            // and remove that from folder hierachy in VS folders/filters.
            var commonPrefixChars = allFolders.Any() ? allFolders.First().ToString().Substring(0, allFolders.Min(s => s.ToString().Length)).TakeWhile((c, i) => allFolders.All(s => s.ToString()[i] == c)).ToArray() : Array.Empty<char>();
            var commonPrefix = new NPath(new string(commonPrefixChars));

            // VS filters file needs filter elements to exist for the whole folder hierarchy (e.g. if we have
            // "Editor\Platform\Windows", we need to have "Editor\Platform", and "Editor" too). Otherwise it will
            // silently skip grouping files into filters and display them at the root.
            var parentFolders = allFolders.SelectMany(f => f.RecursiveParents).Where(p => p.Depth > 0).ToArray();
            allFolders.UnionWith(parentFolders);

            var sortedFolders = allFolders.OrderBy(f => f); // VS seems to load projects a bit faster if file/folder lists are sorted

            var folders = new StringBuilder();
            foreach (var f in sortedFolders)
            {
                if (f.Depth <= commonPrefix.Depth)
                    continue;
                var withoutPrefix = f.RelativeTo(commonPrefix);
                folders.Append($"    <Filter Include=\"{withoutPrefix.ToString(SlashMode.Backward)}\">\n     </Filter>\n");
            }
            var files = new StringBuilder();
            foreach (var f in sourceFiles)
            {
                if (f.Extension == "natvis")
                    continue;
                var withoutPrefix = f.Parent.RelativeTo(commonPrefix);
                files.Append($"    <None Include=\"{f.RelativeTo(filtersFolder).ToString(SlashMode.Backward)}\">\n      <Filter>{withoutPrefix.ToString(SlashMode.Backward)}</Filter>\n     </None>\n");
            }
            var natvisFiles = new StringBuilder();
            foreach (var f in sourceFiles)
            {
                if (f.Extension != "natvis")
                    continue;
                var withoutPrefix = f.Parent.RelativeTo(commonPrefix);
                natvisFiles.Append($"    <Natvis Include=\"{f.RelativeTo(filtersFolder).ToString(SlashMode.Backward)}\">\n      <Filter>{withoutPrefix.ToString(SlashMode.Backward)}</Filter>\n     </Natvis>\n");
            }
            if (natvisFiles.Length > 0)
            {
                natvisFiles.Insert(0, "  <ItemGroup>\n");
                natvisFiles.Append("  </ItemGroup>\n");
            }
            var contents =
                $@"<?xml version=""1.0"" encoding=""utf-8""?>
<Project ToolsVersion=""4.0"" xmlns=""http://schemas.microsoft.com/developer/msbuild/2003"">
  <ItemGroup>
{folders}
  </ItemGroup>
  <ItemGroup>
{files}
  </ItemGroup>
{natvisFiles}</Project>
";
            WriteContentsToFile(filtersFolder.Combine($"{program.Name}.vcxproj.filters"), contents);
        }

        static string SanitizeProjectGuid(string guid)
        {
            // some GUIDs might be passed in with {} already
            if (guid.StartsWith("{") && guid.EndsWith("}"))
                return guid.Substring(1, guid.Length - 2);
            return guid;
        }

        void AddNestingInfo(StringBuilder sb, string projectName, string projectGuid, Dictionary<string, string> folderStructure)
        {
            if (!folderStructure.ContainsKey(projectName))
                return;
            projectGuid = SanitizeProjectGuid(projectGuid);
            var folderGuid = ProjectFile.GuidForName($"FolderGuid-{folderStructure[projectName]}");
            sb.Append($"\t\t{{{projectGuid}}} = {{{folderGuid}}}\n");
        }

        static NativeProgramConfiguration FindClosestConfig(string platform, NativeProgramConfiguration cfg, IEnumerable<NativeProgramConfiguration> configs)
        {
            var configsArray = configs.ToArray();

            // We get VS platform name (e.g. "x86"), and want to map to roughly appropriate jam platform name;
            // see platformToVSSolutionAndProjectPlatformName.
            var cfgPlat = platform.ToLowerInvariant();
            if (configsArray.Any() && configsArray.First().ToolChain.LegacyPlatformIdentifier.StartsWith("uap_"))
            {
                if (cfgPlat == "x86")
                    cfgPlat = "uap_x86";
                else if (cfgPlat == "x64")
                    cfgPlat = "uap_x64";
                else if (cfgPlat == "xarm")
                    cfgPlat = "uap_arm";
                else if (cfgPlat == "xarm64")
                    cfgPlat = "uap_arm64";
            }
            else if (configsArray.Any() && configsArray.First().ToolChain.LegacyPlatformIdentifier.StartsWith("android"))
            {
                if (cfgPlat == "x86")
                    cfgPlat = "android-x86";
                else if (cfgPlat == "x64") // make x64 VS solution config (which is the default) build armv7 Android bits
                    cfgPlat = "android-arm7";
                else if (cfgPlat == "xarm")
                    cfgPlat = "android-arm7";
                else if (cfgPlat == "xarm64")
                    cfgPlat = "android-arm8";
            }
            else
            {
                if (cfgPlat == "x64")
                    cfgPlat = "win64";
                else if (cfgPlat == "x86")
                    cfgPlat = "win32";
            }
            return cfg.FindBestMatching(cfgPlat, configsArray);
        }

        static NativeProgramConfiguration FindClosestConfigForManagedConfig(string managedConfigName, string vsPlatformName,
            IEnumerable<NativeProgramConfiguration> configs)
        {
            NativeProgramConfiguration bestConfig = null;
            var bestScore = -1;
            var managedIsDebug = managedConfigName == "Debug";
            foreach (var c in configs)
            {
                var score = 0;
                if (PlatformToVSSolutionPlatform(c.ToolChain.LegacyPlatformIdentifier) == vsPlatformName)
                    score++;
                var isDebug = c.CodeGen == CodeGen.Debug;
                if (isDebug == managedIsDebug)
                    score++;
                if (score > bestScore)
                {
                    bestScore = score;
                    bestConfig = c;
                }
            }
            return bestConfig;
        }

        static string GetMatchingManagedConfigName(NativeProgramConfiguration cfg)
        {
            if (cfg.CodeGen == CodeGen.Debug)
                return "Debug";
            return "Release";
        }

        // Important: some lines in the .sln file *have* to be indented with tabs (not spaces), otherwise
        // VS (at least 2015) will silently ignore them. This is true for project nesting (indented with spaces will
        // just put everything into root), and various project/solution configuration mapping lines (indented with spaces
        // will assign default mappings which are often wrong).
        //
        // Returns list of C# source files that already got included into generated .csproj projects.
        NPath[] GenerateSolutionFile(string solutionName, NativeProgram[] programs, Dictionary<string, string> folderStructure, string[] managedProjects, string[] prebuiltManagedProjects)
        {
            // VS only displays the solution name in various recent items lists, making it hard to tell
            // which solution is from which checkout (for people working with multiple checkouts).
            // Append toplevel folder name of the checkout to help that.
            var suffix = Paths.UnityRoot.FileName;
            var solutionGuid = ProjectFile.GuidForName(solutionName);
            var solutionPath = $"{SolutionFolder}/{solutionName}-{suffix}.sln";
            var includedCsFiles = new List<NPath>();

            var sbProjects = new StringBuilder();
            var sbNesting = new StringBuilder();

            // Emit references to C++ projects in this solution
            foreach (var p in programs)
            {
                var projectGuid = ProjectFile.GuidForName(p.Name);
                AddProjectInfo(sbProjects, solutionGuid, projectGuid, p.Name, $"{ProjectsFolderName}/{p.Name}.vcxproj");
                AddNestingInfo(sbNesting, p.Name, projectGuid, folderStructure);
            }

            // Emit references to pre-built UnityEngine/UnityEditor C# projects
            foreach (var mpr in prebuiltManagedProjects)
            {
                NPath csprojPath = new NPath(mpr);
                BuiltinRules.Needs(solutionPath, csprojPath);
                var projectName = csprojPath.FileNameWithoutExtension;
                var projectGuid = LegacyCSharpSupport.GuidForProject(mpr);
                AddProjectInfo(sbProjects, solutionGuid, projectGuid, projectName, csprojPath.RelativeTo(SolutionFolder).ToString());
                AddNestingInfo(sbNesting, projectName, projectGuid, folderStructure);
            }

            // Emit references to C# projects
            foreach (var mpr in managedProjects)
            {
                var csAsm = LegacyCSharpSupport.GetProgram(mpr);
                var csProj = csAsm.ProjectFile;
                includedCsFiles.AddRange(CSharpProgram.SourcesIn(csProj.Program.Sources.ForAny()));
                NPath csprojPath = csProj.Path;
                BuiltinRules.Needs(solutionPath, csProj.Path);
                var projectName = csprojPath.FileNameWithoutExtension;
                var projectGuid = csProj.Guid;
                AddProjectInfo(sbProjects, solutionGuid, projectGuid, projectName, csprojPath.RelativeTo(SolutionFolder).ToString());
                AddNestingInfo(sbNesting, projectName, projectGuid, folderStructure);
            }

            // All "folders" in the solution are represented as "fake projects" too; with a special parent GUID and
            // instead of pointing to vcxproj files they just point to their own names.
            var rootFolderGuid = "2150E333-8FDC-42A3-9474-1A3956D46DE8";
            var allProjectNames = programs.Select(p => p.Name).Concat(managedProjects).Concat(prebuiltManagedProjects.Select(mpr => new NPath(mpr).FileNameWithoutExtension));

            // Just like SQL database join: from all project names, pick values that exist in folderStructure with these names
            var allFolderNames = (from pn in allProjectNames join folder in folderStructure on pn equals folder.Key select folder.Value).Distinct();
            var createdParentFolders = new HashSet<string>();
            foreach (var name in allFolderNames)
            {
                var fn = name;
                var folderGuid = ProjectFile.GuidForName($"FolderGuid-{fn}");
                var slashPosition = fn.IndexOf('/');
                if (slashPosition >= 0)
                {
                    var parentName = fn.Substring(0, slashPosition);
                    var parentGuid = ProjectFile.GuidForName($"FolderGuid-{parentName}");
                    // create info for this parent folder, if we haven't seen it yet
                    if (!createdParentFolders.Contains(parentName))
                    {
                        sbProjects.Insert(0, $"Project(\"{{{rootFolderGuid}}}\") = \"{parentName}\", \"{parentName}\", \"{{{parentGuid}}}\"\nEndProject\n");
                        sbNesting.Insert(0, $"\t\t{{{parentGuid}}} = {{{rootFolderGuid}}}\n");
                        createdParentFolders.Add(parentName);
                    }
                    sbNesting.Append($"\t\t{{{folderGuid}}} = {{{parentGuid}}}\n");
                    fn = fn.Substring(slashPosition + 1);
                }
                sbProjects.Append($"Project(\"{{{rootFolderGuid}}}\") = \"{fn}\", \"{fn}\", \"{{{folderGuid}}}\"\nEndProject\n");
            }

            // VS solution configuration/platform popups need to have the union of all things possible in the projects
            var allConfigs = programs.SelectMany(p => p.ValidConfigurations).ToArray();
            var allUniqueNamedConfigs = allConfigs.DistinctBy(cfg => cfg.IdentifierNoPlatform).ToArray();

            var allPlatforms = allConfigs
                .Select(cfg => PlatformToVSSolutionPlatform(cfg.ToolChain.LegacyPlatformIdentifier))
                .Distinct()
                .OrderBy(s => s)
                .ToArray();
            var allConfigNames = allConfigs
                .Select(ConfigurationToVSName)
                .Distinct()
                .OrderBy(s => s);


            // Get list of C# project configurations that we don't have in other projects yet
            var managedConfigNames = Array.Empty<string>();
            if (prebuiltManagedProjects.Any() || managedProjects.Any())
            {
                managedConfigNames = ManagedProjectConfigs.Where(name => !allConfigNames.Contains(name)).ToArray();
            }

            // Generate list of solution configs/platforms
            var sbConfigPlatforms = new StringBuilder();
            foreach (var cfg in allConfigNames)
            {
                AddSolutionConfigInfo(sbConfigPlatforms, allPlatforms, cfg);
            }
            foreach (var cfg in managedConfigNames)
            {
                AddSolutionConfigInfo(sbConfigPlatforms, allPlatforms, cfg);
            }

            // Generate mapping from solution configs to per-project configs. For each project, this has to contain
            // list of all configs present in the solution, mapped to appropriate (closest) config that is present
            // in the project.
            var sbConfigMapping = new StringBuilder();
            foreach (var p in programs)
            {
                var isEnabledInBuild = p == programs.First();
                var projectGuid = ProjectFile.GuidForName(p.Name);
                var validConfigs = p.ValidConfigurations;
                foreach (var solutionCfg in allUniqueNamedConfigs)
                {
                    foreach (var slnPlatform in allPlatforms)
                    {
                        var cfg = FindClosestConfig(slnPlatform, solutionCfg, validConfigs);
                        var slnConfig = ConfigurationToVSName(solutionCfg);
                        var projPlatform = PlatformToVSProjectPlatform(cfg.ToolChain.LegacyPlatformIdentifier);
                        var projConfig = ConfigurationToVSName(cfg);
                        AddSolutionToProjectConfigMapping(sbConfigMapping, projectGuid, slnConfig, slnPlatform, projConfig, projPlatform, isEnabledInBuild);
                    }
                }
                foreach (var managedConfig in managedConfigNames)
                {
                    foreach (var slnPlatform in allPlatforms)
                    {
                        var config = FindClosestConfigForManagedConfig(managedConfig, slnPlatform, validConfigs);
                        var projPlatform = PlatformToVSProjectPlatform(config.ToolChain.LegacyPlatformIdentifier);
                        var projConfig = ConfigurationToVSName(config);
                        AddSolutionToProjectConfigMapping(sbConfigMapping, projectGuid, managedConfig, slnPlatform, projConfig, projPlatform, isEnabledInBuild);
                    }
                }
            }

            // And the same for any pre-built .csproj we might have.
            // Do include them into build if "build solution" is pressed; other .csprojs might depend on them and if
            // we have them excluded from build then the builds will fail.
            foreach (var mpr in prebuiltManagedProjects)
            {
                var projectGuid = LegacyCSharpSupport.GuidForProject(mpr);
                AddManagedProjectConfigMapping(projectGuid, sbConfigMapping, allPlatforms, allUniqueNamedConfigs, managedConfigNames, true);
            }

            // And the same for any .csproj we might have.
            // Exclude them from build by default -- they run actual build system as a post-build step, and if someone
            // does "build solution" that ends up invoking several build processes all at once. This often leads to file
            // write sharing failures.
            foreach (var mpr in managedProjects)
            {
                var csAsm = LegacyCSharpSupport.GetProgram(mpr);
                var csProj = csAsm.ProjectFile;
                var projectGuid = csProj.Guid;
                AddManagedProjectConfigMapping(projectGuid, sbConfigMapping, allPlatforms, allUniqueNamedConfigs, managedConfigNames, false);
            }

            var contents = $@"Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio 15
VisualStudioVersion = 15.0.26228.4
MinimumVisualStudioVersion = 10.0.40219.1
{sbProjects}
Global
    GlobalSection(SolutionConfigurationPlatforms) = preSolution
{sbConfigPlatforms}
    EndGlobalSection
    GlobalSection(ProjectConfigurationPlatforms) = postSolution
{sbConfigMapping}
    EndGlobalSection
    GlobalSection(SolutionProperties) = preSolution
        HideSolutionNode = FALSE
    EndGlobalSection
    GlobalSection(NestedProjects) = preSolution
{sbNesting}
    EndGlobalSection
EndGlobal

";
            using (new ConsoleColorScope(ConsoleColors.Success))
                Console.WriteLine($"Generating {solutionPath}");
            WriteContentsToFile(solutionPath, contents);

            return includedCsFiles.Distinct().ToArray();
        }

        private void AddManagedProjectConfigMapping(string projectGuid, StringBuilder sbConfigMapping, string[] allPlatforms, NativeProgramConfiguration[] allUniqueNamedConfigs, string[] managedConfigNames, bool isEnabledInBuild)
        {
            var projPlatform = "Any CPU";
            foreach (var cfg in allUniqueNamedConfigs)
            {
                foreach (var slnPlatform in allPlatforms)
                {
                    var slnConfig = ConfigurationToVSName(cfg);
                    var projConfig = GetMatchingManagedConfigName(cfg);
                    AddSolutionToProjectConfigMapping(sbConfigMapping, projectGuid, slnConfig, slnPlatform, projConfig, projPlatform, isEnabledInBuild);
                }
            }
            foreach (var cfg in managedConfigNames)
            {
                foreach (var slnPlatform in allPlatforms)
                {
                    AddSolutionToProjectConfigMapping(sbConfigMapping, projectGuid, cfg, slnPlatform, cfg, projPlatform, isEnabledInBuild);
                }
            }
        }

        private static void AddSolutionToProjectConfigMapping(StringBuilder sb, string projectGuid,
            string solutionConfig, string solutionPlatform, string projectConfig, string projectPlatform,
            bool isEnabledInBuild)
        {
            projectGuid = SanitizeProjectGuid(projectGuid);
            sb.Append($"\t\t{{{projectGuid}}}.{solutionConfig}|{solutionPlatform}.ActiveCfg = {projectConfig}|{projectPlatform}\n");
            if (isEnabledInBuild)
                sb.Append($"\t\t{{{projectGuid}}}.{solutionConfig}|{solutionPlatform}.Build.0 = {projectConfig}|{projectPlatform}\n");
        }

        private static void AddProjectInfo(StringBuilder sb, string solutionGuid, string projectGuid, string projectName, string projectFile)
        {
            projectGuid = SanitizeProjectGuid(projectGuid);
            sb.Append($"Project(\"{{{solutionGuid}}}\") = \"{projectName}\", \"{projectFile}\", \"{{{projectGuid}}}\"\nEndProject\n");
        }

        private static void AddSolutionConfigInfo(StringBuilder sb, string[] solutionPlatforms, string cfg)
        {
            foreach (var plat in solutionPlatforms)
            {
                sb.Append($"\t\t{cfg}|{plat} = {cfg}|{plat}\n");
            }
        }
    }

    public class SolutionProjects
    {
        public List<NativeProgram> native = new List<NativeProgram>();
        public List<string> managed = new List<string>();
    }
}
