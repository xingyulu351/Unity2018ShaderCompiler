using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using Unity.TinyProfiling;

namespace Bee.ProjectGeneration.XCode
{
    public class XCodeProjectFile
    {
        private NativeProgram[] NativePrograms { get; }
        private IEnumerable<NPath> ExtraSourceFiles { get; }
        private NPath Path { get; }
        private string ProjectName { get; }

        const string DefaultProjectName = "TheDefault";
        private static readonly string kWorkspaceSettingData = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>BuildSystemType</key>\n\t<string>Original</string>\n</dict>\n</plist>";


        public XCodeProjectFile(IEnumerable<NativeProgram> nativeProgram, NPath path, IEnumerable<NPath> extraSourceFiles = null)
        {
            NativePrograms = nativeProgram.ToArray();
            Path = path;
            ProjectName = ExtractProjectName(Path.ToString());

            ExtraSourceFiles = extraSourceFiles ?? Array.Empty<NPath>();
            using (TinyProfiler.Section("XcodeProject", path.ToString()))
            {
                RegisterWithJam();
            }
        }

        string ExtractProjectName(string projectPath)
        {
            string result = String.Empty;
            Match match = Regex.Match(projectPath, @"(\w+\.xcodeproj)", RegexOptions.IgnoreCase);
            // we have found a project in the path.
            if (match.Success)
            {
                result =  match.Groups[1].Value;
            }
            if (String.IsNullOrEmpty(result))
            {
                throw new Exception($"no valid xcode project in path {projectPath}");
            }
            return result;
        }

        void RegisterWithJam()
        {
            using (new ConsoleColorScope(ConsoleColors.Success))
                Console.WriteLine($"Generating {Path.Parent}");
            var content = ContentsFor(NativePrograms);
            TextFile.Setup(Path, content);
            Backend.Current.AddAliasDependency("NativeProjectFiles", Path);

            // we need to set this up for xcode 10+ so that we use the legecy build system
            // this file being there doesn't impact older xcode versions and they just ignore it
            var workSpaceSettingsPath = $"{Path.Parent.Parent}/{ProjectName}/project.xcworkspace/xcshareddata/WorkspaceSettings.xcsettings";
            TextFile.Setup(workSpaceSettingsPath, kWorkspaceSettingData);

            Backend.Current.AddDependency(Path, workSpaceSettingsPath);
        }

        string ContentsFor(NativeProgram[] nativePrograms)
        {
            foreach (var p in nativePrograms)
            {
                if (p.ValidConfigurations == null)
                    throw new Exception($"Native program '{p.Name}' has no valid configurations set up");
            }

            var doc = new XCodeDocument();
            var xCodeNode = doc.MakeNode("Root_Xcode_Node");
            doc.RootNode = xCodeNode;

            var configurations = nativePrograms.SelectMany(np => np.ValidConfigurations).ToList();

            var group = doc.MakeNode(Path.FileNameWithoutExtension + "_workspace");
            group.Add("isa", "PBXGroup");
            group.Add("name", Path.FileNameWithoutExtension + ".workspace");
            group.Add("children", new List<XCodeNode>());
            group.Add("sourceTree", "\"<group>\"");
            doc.Groups.Add(new NPath(""), group);

            using (TinyProfiler.Section("MakeFileNodes"))
            {
                var paths = new List<NPath>();
                using (TinyProfiler.Section("np.Sources.For"))
                {
                    foreach (var np in nativePrograms)
                    {
                        foreach (var config in configurations)
                        {
                            paths.AddRange(np.Sources.For(config));
                        }
                    }
                }

                List<NPath> distinctPaths;
                using (TinyProfiler.Section("Distinct Paths"))
                    distinctPaths = paths.Distinct().ToList();

                List<NPath> files;
                using (TinyProfiler.Section("SourcePathToFiles"))
                    files = distinctPaths.SelectMany(ProjectFile.SourcePathToFiles).ToList();

                using (TinyProfiler.Section("MakeFileNodes"))
                {
                    foreach (var file in files)
                        doc.MakeFileNode(file);
                }
            }


            using (TinyProfiler.Section("MakeFileNode ExtraSourceFileGlobs"))
            {
                foreach (var f in ExtraSourceFiles)
                {
                    doc.MakeFileNode(f);
                }
            }

            var targets = new List<XCodeNode>();
            foreach (var np in nativePrograms)
            {
                var targetNameForIDE = np.TargetNameForIDE.For(np.ValidConfigurations.First()) ?? np.Name;
                using (TinyProfiler.Section($"MakeBuildConfigs {targetNameForIDE}"))
                {
                    var buildConfigList = MakeBuildConfigurationList(np.ValidConfigurations, doc, np);

                    var buildPhases = new List<XCodeNode>();
                    var shellScript = doc.MakeNode("shellscript_" + targetNameForIDE);
                    shellScript.Add("isa", "PBXShellScriptBuildPhase");
                    shellScript.Add("buildActionMask", "2147483647");
                    shellScript.Add("files", new XCodeNode[0]);
                    shellScript.Add("inputPaths", new XCodeNode[0]);
                    shellScript.Add("outputPaths", new XCodeNode[0]);
                    shellScript.Add("runOnlyForDeploymentPostprocessing", "0");
                    shellScript.Add("shellPath", "/bin/sh");

                    shellScript.Add("shellScript", "\"${BUILDCOMMAND}\"");
                    buildPhases.Add(shellScript);

                    var sourcesPhase = doc.MakeNode("sourcesPhase_" + targetNameForIDE);
                    sourcesPhase.Add("isa", "PBXSourcesBuildPhase");
                    sourcesPhase.Add("buildActionMask", "2147483647");

                    var sourceFiles = np.Sources.ForAny().SelectMany(ProjectFile.SourcePathToFiles).Distinct();

                    var sourceFileNodes = new List<XCodeNode>();
                    foreach (var s in sourceFiles)
                    {
                        var n = doc.MakeSourceFileNode(s);
                        var includes = np.PerFileIncludeDirectories.ForAny(s).ToArray();
                        if (includes.Any())
                        {
                            var sb = new StringBuilder();
                            sb.Append("{COMPILER_FLAGS = \"");
                            foreach (var inc in includes)
                                sb.Append($"-I\\\"{(inc.IsRelative ? inc.MakeAbsolute(Paths.UnityRoot) : inc)}\\\" ");
                            sb.Append("\";}");
                            n.Add("settings", sb.ToString());
                        }
                        sourceFileNodes.Add(n);
                    }
                    sourcesPhase.Add("files", sourceFileNodes);
                    sourcesPhase.Add("runOnlyForDeploymentPostprocessing", "0");
                    buildPhases.Add(sourcesPhase);

                    var buildRules = new List<XCodeNode>();
                    var buildRule = doc.MakeNode("buildRule_" + targetNameForIDE);
                    buildRule.Add("isa", "PBXBuildRule");
                    buildRule.Add("compilerSpec", "com.apple.compilers.proxy.script");
                    buildRule.Add("filePatterns", "\"*\"");
                    buildRule.Add("fileType", "pattern.proxy");
                    buildRule.Add("isEditable", "1");
                    buildRule.Add("outputFiles", new string[0]);
                    buildRule.Add("script", "\"\"");
                    buildRules.Add(buildRule);

                    var target = doc.MakeNode("nativeTarget_" + targetNameForIDE);
                    target.Add("isa", "PBXNativeTarget");
                    target.Add("buildConfigurationList", buildConfigList);
                    target.Add("buildPhases", buildPhases.ToArray());
                    target.Add("buildRules", buildRules.ToArray());
                    target.Add("dependencies", new XCodeNode[0]);
                    target.Add("name", targetNameForIDE.InQuotes());
                    var outputName = np.OutputName.For(np.ValidConfigurations.First());
                    if (string.IsNullOrEmpty(outputName))
                        outputName = targetNameForIDE;

                    target.Add("productName", outputName.InQuotes());

                    var isiPhone = np.ValidConfigurations.First().ToolChain.LegacyPlatformIdentifier
                        .StartsWith("iphone");

                    var productReferenceExplicitFileType = isiPhone ? "archive.ar" : "compiled.mach-o.executable";
                    var productType =
                        isiPhone ? "com.apple.product-type.library.static" : "com.apple.product-type.tool";

                    target.Add("productReference",
                        doc.MakeFileNode(outputName, productReferenceExplicitFileType, "BUILT_PRODUCTS_DIR"));
                    target.Add("productType", productType.InQuotes());
                    targets.Add(target);
                }
            }

            xCodeNode.Add("isa", "PBXProject");
            xCodeNode.Add("attributes", "{\n}");

            xCodeNode.Add("buildConfigurationList", MakeBuildConfigurationListGlobal(doc));

            xCodeNode.Add("compatibilityVersion", "\"Xcode 3.2\"");
            xCodeNode.Add("developmentRegion", "English");
            xCodeNode.Add("hasScannedForEncodings", "1");
            xCodeNode.Add("knownRegions", new[] { "en"});
            xCodeNode.Add("mainGroup", group);
            xCodeNode.Add("projectDirPath", "\"\"");
            xCodeNode.Add("projectRoot", "\"\"");
            xCodeNode.Add("targets", targets.ToArray());

            return doc.ToString();
        }

        static string ProductNameFor(NativeProgram optionalNativeProgram, NativeProgramConfiguration config)
        {
            var name = optionalNativeProgram?.OutputName.For(config);
            if (string.IsNullOrEmpty(name))
                name = optionalNativeProgram?.Name;
            if (string.IsNullOrEmpty(name))
                name = "Solution";
            return name;
        }

        static XCodeNode MakeBuildConfigurationListGlobal(XCodeDocument doc)
        {
            var config = doc.MakeNode("global__build__config");
            config.Add("isa", "XCBuildConfiguration");
            config.Add("name", "\"TheDefault\"");
            config.Add("buildSettings", "{ PRODUCT_NAME = \"Solution\"; }");

            var configList = doc.MakeNode("global__build__config__list");
            configList.Add("isa", "XCConfigurationList");
            configList.Add("buildConfigurations", new[] {config});
            configList.Add("defaultConfigurationIsVisible", "1");
            configList.Add("defaultConfigurationName", DefaultProjectName.InQuotes());
            return configList;
        }

        static XCodeNode MakeBuildConfigurationList(IEnumerable<NativeProgramConfiguration> configurations, XCodeDocument doc, NativeProgram optionalNativeProgram)
        {
            var buildConfigs = new List<XCodeNode>();
            var defaultConfig = configurations.First();

            foreach (var config in configurations)
            {
                var productName = ProductNameFor(optionalNativeProgram, config);

                string postFixIfDefault = (config == defaultConfig && config.Identifier != DefaultProjectName) ? " (default)" : "";

                var configName = $"\"{config.IdentifierForProjectFile}{postFixIfDefault}\"";
                var buildConfig = SetupBuildConfig(productName, configName, doc, config, optionalNativeProgram);
                buildConfigs.Add(buildConfig);
            }

            if (configurations.All(c => c.Identifier != DefaultProjectName))
            {
                var productName = ProductNameFor(optionalNativeProgram, defaultConfig);
                var configName = DefaultProjectName.InQuotes();
                var defaultConfigNode = SetupBuildConfig(productName, configName, doc, defaultConfig, optionalNativeProgram);
                buildConfigs.Add(defaultConfigNode);
            }

            var buildConfigList = doc.MakeNode("buildConfigList-" + (optionalNativeProgram != null ? optionalNativeProgram.Name : "<null>"));
            buildConfigList.Add("isa", "XCConfigurationList");
            buildConfigList.Add("buildConfigurations", buildConfigs.ToArray());
            buildConfigList.Add("defaultConfigurationIsVisible", "1");
            buildConfigList.Add("defaultConfigurationName", DefaultProjectName.InQuotes());
            return buildConfigList;
        }

        static XCodeNode SetupBuildConfig(string productName, string configName, XCodeDocument doc, NativeProgramConfiguration config, NativeProgram nativeProgram)
        {
            NPath outputDirectoryFor = null;

            if (nativeProgram != null)
            {
                // If a debug command is set on program, use it to override product and output folder
                string debugCommand = nativeProgram.DebugCommand.For(config);

                if (debugCommand != null)
                {
                    // debug command given - override folder and product
                    NPath path = new NPath(debugCommand);
                    outputDirectoryFor = path.Parent;
                    productName = path.FileName;
                }
                else
                {
                    // no debug command, just use output directory
                    outputDirectoryFor = nativeProgram.OutputDirectory.For(config);
                }
            }

            var buildConfig = doc.MakeNode($"buildConfig-{productName}-{configName}-{config.IdentifierForProjectFile}");
            buildConfig.Add("isa", "XCBuildConfiguration");

            var sb = new StringBuilder();
            sb.Append($"PRODUCT_NAME = \"{productName}\"; ");

            if (nativeProgram != null)
            {
                sb.Append($"BUILDCOMMAND = \"{nativeProgram.CommandToBuild.For(config)}\" ; ");

                if (config != null)
                {
                    string xcodeArch = null;
                    switch (config.ToolChain.LegacyPlatformIdentifier)
                    {
                        case "macosx32":
                        case "webgl":
                        case "win32": // only used to test generation of Xcode projects on windows, while developing
                            xcodeArch = "i386";
                            break;
                        case "macosx64":
                        case "win64": // only used to test generation of Xcode projects on windows, while developing
                            xcodeArch = "x86_64";
                            break;
                        case "iphone_armv7":
                            xcodeArch = "armv7";
                            break;
                        case "iphone_aarch64":
                            xcodeArch = "arm64";
                            break;
                        case "iphone_x86":
                            xcodeArch = "i386";
                            break;
                        case "iphone_x86_64":
                            xcodeArch = "x86_64";
                            break;
                        case "iphone":
                            xcodeArch = "armv7 arm64";
                            break;
                        case "iphonesimulator":
                            xcodeArch = "i386";
                            break;
                        default:
                            throw new ArgumentException($"Unknown platform: {config.ToolChain.LegacyPlatformIdentifier}");
                    }
                    if (config.ToolChain.LegacyPlatformIdentifier.StartsWith("iphone"))
                    {
                        sb.Append("SUPPORTED_PLATFORMS = \"iphonesimulator iphoneos\";");
                        sb.Append("SDKROOT = iphoneos10.2;");
                    }
                    sb.Append($"VALID_ARCHS = \"{xcodeArch}\"; ");
                    sb.Append($"ARCHS = \"{xcodeArch}\"; ");
                }

                if (outputDirectoryFor != null)
                    sb.Append($"CONFIGURATION_BUILD_DIR = {outputDirectoryFor.MakeAbsolute(Paths.UnityRoot).InQuotes()}; ");
                sb.Append($"HEADER_SEARCH_PATHS = ( {nativeProgram.IncludeDirectories.For(config).Select(s=>(s.IsRelative ? Paths.UnityRoot.Combine(s) : s).InQuotes()).SeparateWith(", ")} ); ");
                sb.Append($"GCC_PREPROCESSOR_DEFINITIONS = ( {nativeProgram.Defines.For(config).InQuotes().SeparateWith(", ")} ); ");
            }

            buildConfig.Add("buildSettings", $"{{ {sb} }}");
            buildConfig.Add("name", configName);
            return buildConfig;
        }
    }
}
