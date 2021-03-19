using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using Unity.TinyProfiling;

namespace Bee.ProjectGeneration
{
    public class CLionProjectFile
    {
        private NativeProgram[] NativeProgram { get; }
        public NPath Path { get; }

        public CLionProjectFile(IEnumerable<NativeProgram> nativeProgram, NPath path)
        {
            NativeProgram = nativeProgram.ToArray();
            Path = path;
            using (TinyProfiler.Section("CLionProject", path.ToString()))
                Setup();
        }

        void Setup()
        {
            var content = ContentsFor(NativeProgram);

            Backend.Current.AddWriteTextAction(Path, content);
            Backend.Current.AddAliasDependency("NativeProjectFiles", Path);
        }

        //clion/cmake support is not really supported, but from time to time, someone tries to get it a little bit better
        //it's hard for now to support multiple configurations per target in clion, so to prefer to get something
        //working rather than nothing, we're going to support only the first configuration for any target right now,
        //which allows at least for clion to be used as a reasonable editor.
        bool onlySupportSingleConfiguration = true;

        string ContentsFor(NativeProgram[] nativePrograms)
        {
            IEnumerable<NativeProgramConfiguration> solutionConfigurations;

            if (onlySupportSingleConfiguration)
                solutionConfigurations = nativePrograms.Select(np => np.ValidConfigurations?.FirstOrDefault())
                    .Where(c => c != null);
            else
                solutionConfigurations = nativePrograms.Select(np => np.ValidConfigurations)
                    .Where(c => c != null)
                    .SelectMany(c => c);

            solutionConfigurations = solutionConfigurations
                .DistinctBy(c => c.IdentifierForProjectFile)
                .ToArray();

            var sb = new StringBuilder();
            sb.AppendLine(
                $@"
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CONFIGURATION_TYPES {solutionConfigurations.Select(c=>c.IdentifierForProjectFile).SeparateWithSpace()} CACHE TYPE INTERNAL FORCE )
");

            var transitiveNativePrograms = nativePrograms.Concat(nativePrograms.SelectMany(np => np.TransitiveFromSourceLibrariesFor(solutionConfigurations.First()).Select(d => d.Library)).Distinct().ToList());
            foreach (var nativeProgram in transitiveNativePrograms)
            {
                bool hasSources;
                using (TinyProfiler.Section("Sources", nativeProgram.Name))
                {
                    IEnumerable<NPath> sources = solutionConfigurations.SelectMany(nativeProgram.Sources.For).Distinct();
                    // today we use SourcesFilesIn to leave only "source" files that exist in Clion project
                    sources = Unity.BuildSystem.NativeProgramSupport.NativeProgram.SourcesFilesIn(sources);

                    hasSources = sources.Any();
                    sb.AppendLine(
                        $@"
project({nativeProgram.Name})
set(SOURCES");
                    foreach (var source in sources)
                        sb.AppendLine($"\t{Paths.UnityRoot.Combine(source)}");
                }
                sb.AppendLine($@")
");

                if (!hasSources)
                    continue;

                sb.AppendLine($"add_executable({nativeProgram.Name} ${{SOURCES}})");

                foreach (var solutionConfiguration in solutionConfigurations)
                {
                    var bestMatchingConfig = solutionConfiguration.FindBestMatching(null, nativeProgram.ValidConfigurations ?? solutionConfigurations);

                    if (!onlySupportSingleConfiguration)
                    {
                        sb.AppendLine(
                            $"if( ${{CMAKE_BUILD_TYPE}} STREQUAL \"{bestMatchingConfig.IdentifierForProjectFile}\" )");
                    }
                    var includeDirectoriesFor = nativeProgram.AllIncludeDirectoriesFor(bestMatchingConfig);
                    var includeDirs = includeDirectoriesFor.Select(p => (p.IsRelative ? Paths.UnityRoot.Combine(p) : p)
                        .ToString(SlashMode.Forward))
                        .SeparateWith(";")
                        .InQuotes();

                    sb.AppendLine($"set_target_properties({nativeProgram.Name} PROPERTIES INCLUDE_DIRECTORIES {includeDirs})");

                    var defines = nativeProgram.AllDefinesFor(bestMatchingConfig).SeparateWith(";").InQuotes();

                    sb.AppendLine($"set_target_properties({nativeProgram.Name} PROPERTIES COMPILE_DEFINITIONS {defines})");

                    var command = nativeProgram.CommandToBuild.For(bestMatchingConfig) ?? $"./jam {nativeProgram.TargetNameForIDE.For(bestMatchingConfig) ?? nativeProgram.Name} {bestMatchingConfig.JamArgs}";

                    sb.AppendLine(
                        $@"
add_custom_target({nativeProgram.Name}_jam)
add_custom_command(TARGET {nativeProgram.Name}_jam COMMAND {command.Replace("\\","\\\\")} WORKING_DIRECTORY {Paths.UnityRoot})");

                    if (!onlySupportSingleConfiguration)
                        sb.AppendLine("endif()");
                    if (onlySupportSingleConfiguration)
                        break;
                }
            }
            return sb.ToString();
        }
    }
}
