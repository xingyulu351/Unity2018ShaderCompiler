using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.DotNet;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    // Class for building managed C# programs or assemblies.
    // Most common usage: setup sources, references etc., and call SetupDefault().
    public class CSharpProgram
    {
        private readonly Dictionary<NPath, DotNetAssembly> _alreadySetup = new Dictionary<NPath, DotNetAssembly>();

        public NPath Path { get; set; }

        public CollectionWithConditions<NPath, CSharpProgramConfiguration> PrebuiltReferences { get; } = new CollectionWithConditions<NPath, CSharpProgramConfiguration>();
        public NPath ProjectFilePath { get; set; }
        public CollectionWithConditions<string, CSharpProgramConfiguration> Defines { get; } = new CollectionWithConditions<string, CSharpProgramConfiguration>();
        public CollectionWithConditions<CSharpProgram, CSharpProgramConfiguration> References { get; } = new CollectionWithConditions<CSharpProgram, CSharpProgramConfiguration>();
        public CollectionWithConditions<CSharpProjectFileReference, CSharpProgramConfiguration> CsprojReferences { get; } = new CollectionWithConditions<CSharpProjectFileReference, CSharpProgramConfiguration>();
        public CollectionWithConditions<string, CSharpProgramConfiguration> SystemReferences { get; } = new CollectionWithConditions<string, CSharpProgramConfiguration>();
        public CollectionWithConditions<NPath, CSharpProgramConfiguration> Sources { get; } = new CollectionWithConditions<NPath, CSharpProgramConfiguration>();
        public CollectionWithConditions<EmbeddedResource, CSharpProgramConfiguration> Resources { get; } = new CollectionWithConditions<EmbeddedResource, CSharpProgramConfiguration>();

        public Framework Framework { get; set; } = Framework.Framework46;
        public CollectionWithConditions<int, CSharpProgramConfiguration> IgnoredWarnings { get; } = new CollectionWithConditions<int, CSharpProgramConfiguration>();
        public bool CopyReferencesNextToTarget { get; set; } = true;
        public string LanguageVersion { get; set; } = "6";
        public bool WarningsAsErrors { get; set; } = true;
        public bool Unsafe { get; set; }
        public string CprojBuildEvents { get; set; } = "";
        public string ExplicitGuid { get; set; }

        // Generated csproj files by default ignore assembly build Path location, and
        // use artifacts/Unity.BuildSystem.CSharpSupport/{name}/{config}. Set this
        // to true if it needs to use actual output location for whatever reason.
        public bool UseOutputPathInProjectFile { get; set; }

        // Setup actual build of the C# program, with default (Debug) config. Returns output exe/dll path.
        public DotNetAssembly SetupDefault()
        {
            var config = new CSharpProgramConfiguration(CSharpCodeGen.Debug, DetermineCompiler());
            return SetupSpecificConfiguration(config);
        }

        public DotNetAssembly SetupSpecificConfiguration(CSharpProgramConfiguration config)
        {
            Validate();
            var dest = Path;

            if (_alreadySetup.TryGetValue(dest, out var result))
                return result;

            if (CsprojReferences.For(config).Any())
            {
                throw new Exception($"CSharpProgram with pre-built .csproj references should only be used when generating IDE projects (i.e. not inside SetupSpecificConfiguration). Error happened while building {Path}");
            }

            foreach (var r in References.For(config))
                r.SetupSpecificConfiguration(config);

            var sourceFiles = SourcesIn(Sources.For(config));
            var myPrebuiltReferences = PrebuiltReferences.For(config).ToList();
            var transitiveReferences = AllTransitiveReferencesFor(config, new[] {this}).ToList();

            var refToSetup = transitiveReferences.ToDictionary(k => k, k => k.SetupSpecificConfiguration(config));

            var prebuilt = myPrebuiltReferences
                .Concat(References.For(config).Select(r => refToSetup[r].ReferenceAssemblyPath ?? refToSetup[r].Path))
                .Concat(CsprojReferences.For(config).Select(r => r.Path))
                .ToArray();

            var runtimeDepsForPrebuiltLibs = myPrebuiltReferences.Select(p => new DotNetAssembly(p, Framework.Framework46));
            var dotNetAssembliesOfReferences = refToSetup.Values.Concat(runtimeDepsForPrebuiltLibs).ToArray();

            var builtAssembly = config.Compiler.SetupInvocation(
                targetFile: dest,
                sources: sourceFiles.ToArray(),
                prebuiltReferences: prebuilt,
                systemReferences: SystemReferences.For(config).ToArray(),
                @unsafe: Unsafe,
                defines: Defines.For(config).ToArray(),
                ignoredWarnings: IgnoredWarnings.For(config).ToArray(),
                framework: Framework,
                warningsAsErrors: WarningsAsErrors,
                langversion: LanguageVersion,
                resources: Resources.For(config).ToArray());

            builtAssembly = builtAssembly.WithRuntimeDependencies(dotNetAssembliesOfReferences);

            if (CopyReferencesNextToTarget)
                builtAssembly.DeployTo(builtAssembly.Path.Parent);

            _alreadySetup.Add(dest, builtAssembly);
            return builtAssembly;
        }

        public static CSharpCompiler DetermineCompiler()
        {
            var allCompilers = DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<CSharpCompiler>("Bee.*.dll").Where(c => c.CanBuild()).ToArray();

            var maxScore = allCompilers.Max(c => c.PreferredUseScore);
            var compilersWithMaxScore = allCompilers.Where(c => c.PreferredUseScore == maxScore).ToList();
            if (compilersWithMaxScore.Count > 1)
            {
                var mcs = compilersWithMaxScore.OfType<Mcs>().FirstOrDefault();
                if (mcs != null)
                    return mcs;
            }

            return compilersWithMaxScore.First();
        }

        public static IEnumerable<NPath> SourcesIn(IEnumerable<NPath> sourceFilesAndFolders)
        {
            foreach (var path in sourceFilesAndFolders)
            {
                if (path.DirectoryExists())
                {
                    foreach (var file in FilteredGlob.RecursiveGlob(path, "*.cs", p => !p.HasDirectory("obj") && !p.HasDirectory("bin")))
                        yield return file;
                    continue;
                }
                yield return path;
            }
        }

        public static NPath[] FilteredSources(IEnumerable<NPath> sources)
        {
            return sources.Where(p => !p.HasDirectory("obj") && !p.HasDirectory("bin") && !p.FileName.EndsWith(".jam.cs")).ToArray();
        }

        void Validate()
        {
            if (Path == null)
                Errors.Exit($"{nameof(Path)} is not set up for CSharpProgram");
            if (!Sources.ForAny().Any())
                Errors.Exit($"{nameof(Sources)} are not set up for CSharpProgram");

            if (!Framework.IsDotNetFramework && SystemReferences.ForAny().Any())
                Errors.Exit("When specifying a non dotnet framework: {Framework} you cannot have any SystemReferences.");

            SetDefaultProjectFilePath();
        }

        void SetDefaultProjectFilePath()
        {
            if (ProjectFilePath == null)
            {
                var firstDirectory = Sources.ForAny().FirstOrDefault(p => p.DirectoryExists());
                var projectFileDirectory = firstDirectory ?? Sources.ForAny().First().Parent;
                var projectName = Path.FileNameWithoutExtension;
                ProjectFilePath = projectFileDirectory.Combine(projectName + ".gen.csproj");
            }
        }

        private ProjectFile _projectFile;

        public ProjectFile ProjectFile
        {
            get
            {
                if (_projectFile == null)
                {
                    Validate();
                    _projectFile = new ProjectFile(this, ProjectFilePath);
                }
                return _projectFile;
            }
        }


        public ProjectFile ProjectFileWithAdditionalFiles(IEnumerable<NPath> embeddedResources, IEnumerable<NPath> contentResources = null)
        {
            Validate();
            return new ProjectFile(this, ProjectFilePath, embeddedResources, contentResources);
        }

        public IEnumerable<CSharpProgram> TransitiveReferencesFor(CSharpProgramConfiguration config) => AllTransitiveReferencesFor(config, this);

        public static IEnumerable<CSharpProgram> AllTransitiveReferencesFor(CSharpProgramConfiguration config,
            params CSharpProgram[] assemblies)
        {
            var results = new HashSet<CSharpProgram>();
            var queue = new Queue<CSharpProgram>();
            foreach (var assembly in assemblies)
            {
                results.Add(assembly);
                queue.Enqueue(assembly);
            }

            while (queue.Any())
            {
                var process = queue.Dequeue();
                foreach (var r in process.References.For(config))
                {
                    if (!results.Add(r))
                        continue;
                    queue.Enqueue(r);
                }
            }

            return results.Exclude(assemblies);
        }
    }

    public class EmbeddedResource
    {
        public NPath PathToEmbed;
        public string Identifier;
    }
}
