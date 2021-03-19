using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using Bee.Core;
using Bee.NativeProgramSupport.Building;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public enum Language
    {
        AssemblerWithCpp,
        C,
        Cpp,
        ObjectiveC,
        ObjectiveCpp,
    }

    public static class FileLanguage
    {
        public static Language FromPath(NPath fileName)
        {
            return FromExtension(fileName.Extension);
        }

        public static Language FromExtension(string extension)
        {
            switch (extension.ToLowerInvariant())
            {
                case "c": return Language.C;
                case "cpp": return Language.Cpp;
                case "cc": return Language.Cpp;
                case "m": return Language.ObjectiveC;
                case "mm": return Language.ObjectiveCpp;
                case "s": return Language.AssemblerWithCpp;
                default: throw new NotSupportedException($"Can't determine language for {nameof(extension)}: {extension}");
            }
        }
    }

    public enum FromSourceLibraryMode
    {
        StaticLibrary,
        //We do not support dynamic libraries built from source transparently yet.
        //Dynamic,
        BagOfObjects
    }

    public static class FromSourceLibraryDependencyHelper
    {
        public static void Add(this CollectionWithConditions<FromSourceLibraryDependency, NativeProgramConfiguration> collection, IEnumerable<NativeProgram> programs)
        {
            collection.Add(programs.Select(p => new FromSourceLibraryDependency() { Library = p}));
        }
    }

    public class FromSourceLibraryDependency
    {
        public FromSourceLibraryMode FromSourceLibraryMode = FromSourceLibraryMode.StaticLibrary;
        public NativeProgram Library;
        public Func<NativeProgramConfiguration, NativeProgramConfiguration> ConfigurationCustomizationCallback;

        public static implicit operator FromSourceLibraryDependency(NativeProgram library)
        {
            return new FromSourceLibraryDependency() {Library = library, ConfigurationCustomizationCallback = c => c};
        }

        public NativeProgramConfiguration ConfigurationToUseFor(NativeProgramConfiguration config)
        {
            return ConfigurationCustomizationCallback?.Invoke(config) ?? config;
        }
    }

    public class NativeProgram
    {
        string _setName;

        public ValueWithConditions<string, NativeProgramConfiguration> OutputName { get; } = new ValueWithConditions<string, NativeProgramConfiguration>();
        public ValueWithConditions<NPath, NativeProgramConfiguration> OutputDirectory { get; } = new ValueWithConditions<NPath, NativeProgramConfiguration>();
        public ValueWithConditions<string, NativeProgramConfiguration> OutputExtension { get; } = new ValueWithConditions<string, NativeProgramConfiguration>();

        public ValueWithConditions<bool, NativeProgramConfiguration> RTTI { get; } = new ValueWithConditions<bool, NativeProgramConfiguration>();
        public ValueWithConditions<bool, NativeProgramConfiguration> Exceptions { get; } = new ValueWithConditions<bool, NativeProgramConfiguration>();
        public CollectionWithConditions<NPath, NativeProgramConfiguration> Sources { get; } = new CollectionWithConditions<NPath, NativeProgramConfiguration>();
        public CollectionWithConditions<NPath, NativeProgramConfiguration> NonLumpableFiles { get; } = new CollectionWithConditions<NPath, NativeProgramConfiguration>();

        public CollectionWithConditions<string, NativeProgramConfiguration> Defines { get; } = new CollectionWithConditions<string, NativeProgramConfiguration>();
        public CollectionWithConditions<NPath, NativeProgramConfiguration> IncludeDirectories { get; } = new CollectionWithConditions<NPath, NativeProgramConfiguration>();
        public CollectionWithConditions<PrecompiledLibrary, NativeProgramConfiguration> PrebuiltLibraries { get; } = new CollectionWithConditions<PrecompiledLibrary, NativeProgramConfiguration>();
        public CollectionWithConditions<FromSourceLibraryDependency, NativeProgramConfiguration> StaticLibraries { get; } = new CollectionWithConditions<FromSourceLibraryDependency, NativeProgramConfiguration>();

        public CollectionWithConditions<NPath, NativeProgramConfiguration> ExtraDependenciesForAllObjectFiles { get; } = new CollectionWithConditions<NPath, NativeProgramConfiguration>();

        public KeyedCollectionWithConditions<Language, string, NativeProgramConfiguration> PerLanguageFlags { get; } = new KeyedCollectionWithConditions<Language, string, NativeProgramConfiguration>();
        public KeyedCollectionWithConditions<NPath, string, NativeProgramConfiguration> PerFileDefines { get; } = new KeyedCollectionWithConditions<NPath, string, NativeProgramConfiguration>();
        public KeyedCollectionWithConditions<NPath, NPath, NativeProgramConfiguration> PerFileIncludeDirectories { get; } = new KeyedCollectionWithConditions<NPath, NPath, NativeProgramConfiguration>();
        public KeyedValueWithConditions<NPath, NPath, NativeProgramConfiguration> PerFilePchs { get; } =
            new KeyedValueWithConditions<NPath, NPath, NativeProgramConfiguration>();

        public ValueWithConditions<bool, NativeProgramConfiguration> WinRTExtensions { get; } = new ValueWithConditions<bool, NativeProgramConfiguration>();
        public CollectionWithConditions<NPath, NativeProgramConfiguration> WinRTAssemblies { get; } = new CollectionWithConditions<NPath, NativeProgramConfiguration>();

        public NPath DefaultPCH { get; set; }
        public CollectionWithConditions<NPath, NativeProgramConfiguration> PublicIncludeDirectories { get; } = new CollectionWithConditions<NPath, NativeProgramConfiguration>();
        public CollectionWithConditions<string, NativeProgramConfiguration> PublicDefines { get; } = new CollectionWithConditions<string, NativeProgramConfiguration>();

        private List<NativeProgramConfiguration> _setupConfigurations = new List<NativeProgramConfiguration>();
        public IEnumerable<NativeProgramConfiguration> SetupConfigurations => _setupConfigurations;

        private IEnumerable<NativeProgramConfiguration> _validConfigurations;
        public IEnumerable<NativeProgramConfiguration> ValidConfigurations
        {
            get
            {
                if (_validConfigurations != null && !_validConfigurations.Any())
                    Errors.Exit("Fetching empty valid configurations list for NativeProgram");
                return _validConfigurations;
            }
            set
            {
                _validConfigurations = value;
                if (!_validConfigurations.Any())
                    Errors.Exit("Setting up empty valid configurations list for NativeProgram");
            }
        }

        internal readonly CompilerCustomizations _compilerCustomizations = new CompilerCustomizations();
        public ICustomizationFor<CLikeCompilerSettings> CompilerSettings() => _compilerCustomizations;

        internal readonly LinkerCustomizations _linkerCustomizations = new LinkerCustomizations();
        public ICustomizationFor<ObjectFileLinker> DynamicLinkerSettings() => _linkerCustomizations;

        string m_ExtraJamArgs;

        private readonly HashSet<NPath> _alreadySetup = new HashSet<NPath>();

        static string[] MostCommonSupportedExtensions { get; } = {"cpp", "h", "mm", "m"};

        public bool EnsureAllPathsAreRelative { get; set; }

        public ValueWithConditions<string, NativeProgramConfiguration> TargetNameForIDE { get; } =
            new ValueWithConditions<string, NativeProgramConfiguration>();

        public ValueWithConditions<string, NativeProgramConfiguration> DebugCommand { get; } =
            new ValueWithConditions<string, NativeProgramConfiguration>();

        public ValueWithConditions<string, NativeProgramConfiguration> CommandToBuild { get; } =
            new ValueWithConditions<string, NativeProgramConfiguration>();

        private NPath _rootDirectory;
        public NPath RootDirectory
        {
            get { return _rootDirectory ?? Sources.ForAny().FirstOrDefault(d => d.DirectoryExists()) ?? Sources.ForAny().First().Parent; }
            set { _rootDirectory = value; }
        }

        public string ExtraJamArgs
        {
            get { return m_ExtraJamArgs ?? ""; }
            set { m_ExtraJamArgs = value; }
        }

        private NativeProgram()
        {
            Sources.ValueValidator = ValidatePathIsRelative;
            PerFileDefines.KeyValidator = ValidatePathIsRelative;
            PerFileIncludeDirectories.KeyValidator = ValidatePathIsRelative;

            //we add this to all programs as it's a requirement for lumping to work
            IncludeDirectories.Add(".");
        }

        public NativeProgram(string name)
            : this()
        {
            _setName = name;

            ConstructorCallback.Invoke(this);
        }

        public struct FilterResult<T>
        {
            public List<T> True;
            public List<T> False;
        }

        public static FilterResult<T> Filter<T>(T[] inputs, Func<T, bool> predicate)
        {
            var result = new FilterResult<T>
            {
                True = new List<T>(inputs.Length),
                False = new List<T>(inputs.Length)
            };
            foreach (var input in inputs)
            {
                var target = predicate(input) ? result.True : result.False;
                target.Add(input);
            }
            return result;
        }

        private IEnumerable<ILibrary> SetupLibraryDependencies(NativeProgramConfiguration config, ToolChain toolchain, ObjectFileProducer[] objectFileProducers, NPath artifactsPath)
        {
            var precompiledLibraries = StaticLibraries.For(config).SelectMany(library =>
            {
                var configToUse = library.ConfigurationToUseFor(config);

                NativeProgram libraryLibrary = library.Library;

                var staticLibraryFormat = toolchain.StaticLibraryFormat;
                var formatToUse = staticLibraryFormat;
                BagOfObjectsFormat bagOfObjectsFormat = null;
                if (library.FromSourceLibraryMode == FromSourceLibraryMode.BagOfObjects)
                {
                    formatToUse = bagOfObjectsFormat = new BagOfObjectsFormat(staticLibraryFormat.Linker);
                }

                var path = libraryLibrary.SetupSpecificConfiguration(configToUse, formatToUse, objectFileProducers, null, artifactsPath);

                var precompiledLibrary = bagOfObjectsFormat == null
                    ? (ILibrary) new StaticLibrary(path)
                    : new BagOfObjectFilesLibrary(bagOfObjectsFormat.AllObjectFiles);

                var dependencies = libraryLibrary.SetupLibraryDependencies(configToUse, toolchain, objectFileProducers, artifactsPath);

                return dependencies.Prepend(precompiledLibrary);
            }).ToArray();


            var setupLibraryDependencies = precompiledLibraries
                .Concat(PrebuiltLibraries.For(config)).ToList();

            var badLibrary = setupLibraryDependencies.OfType<PrecompiledLibrary>().FirstOrDefault(l => !l.System && l.Path == null);
            if (badLibrary != null)
                throw new InvalidOperationException($"Library {badLibrary} is not a system library, but does not have a Path set");

            while (true)
            {
                var pair = FindFirstEqualPair(setupLibraryDependencies);
                if (pair == null)
                    break;

                var keepLast = WhenMultiplePolicy.KeepLast;
                if (setupLibraryDependencies[pair.First] is PrecompiledLibrary precompiled)
                    keepLast = precompiled.WhenMultiplePolicy;

                setupLibraryDependencies.RemoveAt(keepLast == WhenMultiplePolicy.KeepLast ? pair.First : pair.Second);
            }

            return setupLibraryDependencies;
        }

        class EqualPair
        {
            public int First;
            public int Second;
        }

        static EqualPair FindFirstEqualPair(List<ILibrary> list)
        {
            for (int first = 0; first < list.Count - 1; first++)
                for (int second = first + 1; second != list.Count; second++)
                    if (Equals(list[first], list[second]))
                        return new EqualPair() {First = first, Second = second};
            return null;
        }

        public virtual NPath SetupSpecificConfiguration(NativeProgramConfiguration config, NativeProgramFormat format,
            ObjectFileProducer[] objectFileProducers = null, NPath targetDirectory = null,
            NPath artifactsPath = null)
        {
            if (config == null)
                throw new ArgumentNullException(nameof(config));
            if (format == null)
                throw new ArgumentNullException(nameof(format));

            // using (TinyProfiler.Section($"{nameof(SetupSpecificConfiguration)} for {Name}"))
            {
                if (ValidConfigurations != null && !ValidConfigurations.Contains(config))
                    throw new ArgumentException("config is not known as a valid config");

                _setupConfigurations.Add(config);

                if (format.Toolchain != config.ToolChain)
                    throw new ArgumentException($"The toolchain setup in the {nameof(NativeProgramConfiguration)} must be the same toolchain that provided the {nameof(NativeProgramFormat)}");

                Backend.Current.Register(format.Toolchain);

                var programArtifactsPath = artifactsPath ?? DefaultArtifactsPathFor(config);

                var outputName = OutputName.For(config) ?? Name;
                var linkPath = new NPath($"{programArtifactsPath}/{outputName}").ChangeExtension(format.Extension);
                if (_alreadySetup.Contains(linkPath))
                    return linkPath;

                var toolchain = format.Toolchain;
                objectFileProducers = objectFileProducers ?? toolchain.ObjectFileProducers;

                var libraryDependencies = SetupLibraryDependencies(config, toolchain, objectFileProducers, artifactsPath).ToArray();
                var allUnits = ProduceObjectFileProductionInstructions(linkPath, config, format, programArtifactsPath, objectFileProducers, out var additionalObjectFiles);
                var outputFiles = CombineObjectFilesAndSetupProgram(linkPath, config, format, programArtifactsPath, allUnits, additionalObjectFiles, libraryDependencies);

                var finalTarget = linkPath;
                if ((targetDirectory = targetDirectory ?? OutputDirectory.For(config)) != null)
                {
                    finalTarget = targetDirectory.Combine(outputName).ChangeExtension(format.Extension);
                    foreach (var outputFile in outputFiles)
                    {
                        var targetFile = CopyTool.Instance().Setup(targetDirectory.Combine(outputFile.FileName), outputFile);
                        if (targetFile != finalTarget)
                            // If the linker output multiple files that we're copying, make sure that the copy of the 'main'
                            // output file will force the copies of the other files to come with it
                            Backend.Current.AddDependency(finalTarget, targetFile);
                    }
                }

                _alreadySetup.Add(finalTarget);
                return finalTarget;
            }
        }

        internal NPath ObjectPathFor(NPath objectFilesRoot, ObjectFileProductionInstructions cui)
        {
            var objFolder = cui.InputFile.Parent;
            if (!cui.InputFile.IsRelative)
            {
                // some source files can be with absolute paths; e.g. Android build compiles one .c file
                // straight out of NDK sources; use hash of path as object file location then
                objFolder = "abs-" + cui.InputFile.Parent.GetHashCode().ToString("X");
            }
            else
            {
                // if source file was under artifacts root already (e.g. generated/lump file), don't repeat the same
                // artifacts root into object file location
                if (objFolder == objectFilesRoot || objFolder.IsChildOf(objectFilesRoot))
                    objFolder = objFolder.RelativeTo(objectFilesRoot);
            }

            return objectFilesRoot.Combine($"{objFolder.Combine(cui.InputFile.FileNameWithoutExtension)}.{cui.ObjectFileProducer.ObjectExtension}");
        }

        private NPath[] PublicIncludesOfLibrariesFor(NativeProgramConfiguration config)
        {
            var fromSourceLibraries = TransitiveFromSourceLibrariesFor(config).ToList();
            var prebuiltLibraries = TransitivePrebuiltLibraries(config, fromSourceLibraries);

            var one = fromSourceLibraries.SelectMany(d => d.Library.PublicIncludeDirectories.For(d.ConfigurationToUseFor(config))).ToList();
            var two = prebuiltLibraries.SelectMany(d => d.PublicIncludeDirectoriesFor(config));
            return one.Concat(two).Distinct().ToArray();
        }

        private string[] PublicDefinesOfLibrariesFor(NativeProgramConfiguration config)
        {
            var fromSourceLibraries = TransitiveFromSourceLibrariesFor(config).ToList();
            var prebuiltLibraries = TransitivePrebuiltLibraries(config, fromSourceLibraries);

            var one = fromSourceLibraries.SelectMany(d => d.Library.PublicDefines.For(d.ConfigurationToUseFor(config))).ToList();
            var two = prebuiltLibraries.SelectMany(d => d.PublicDefinesFor(config));
            return one.Concat(two).Distinct().ToArray();
        }

        private List<PrecompiledLibrary> TransitivePrebuiltLibraries(NativeProgramConfiguration config, IEnumerable<FromSourceLibraryDependency> fromSourceLibraries)
        {
            return fromSourceLibraries
                .SelectMany(d => d.Library.PrebuiltLibraries.For(d.ConfigurationToUseFor(config)))
                .Concat(PrebuiltLibraries.For(config))
                .Distinct().ToList();
        }

        public IEnumerable<FromSourceLibraryDependency> TransitiveFromSourceLibrariesFor(NativeProgramConfiguration config)
        {
            var processed = new HashSet<NativeProgram>();

            var toProcess = new Queue<FromSourceLibraryDependency>(StaticLibraries.For(config));
            while (toProcess.Any())
            {
                var pop = toProcess.Dequeue();
                if (processed.Contains(pop.Library))
                    continue;
                yield return pop;

                foreach (var d in pop.Library.StaticLibraries.For(pop.ConfigurationToUseFor(config)))
                    toProcess.Enqueue(d);
            }
        }

        public ObjectFileProductionInstructions[] ProduceObjectFileProductionInstructions(
            NPath destination,
            NativeProgramConfiguration config, NativeProgramFormat format, NPath artifactsPath,
            ObjectFileProducer[] objectFileProducers,
            out NPath[] additionalObjectFiles)
        {
            //var modifiedObjectFileProducers = originalObjectFileProducers.Select(CompilerCustomizations.For).ToArray();

            var supportedExtensions = objectFileProducers.SelectMany(o => o.SupportedExtensions).Distinct().ToArray();
            var sourceFiles = SourcesFilesIn(Sources.For(config), supportedExtensions).Distinct();
            var additionalSourceFilesForFormat = format.SetupAdditionalSourceFilesForTarget(destination, config.CodeGen);
            sourceFiles = sourceFiles.Concat(additionalSourceFilesForFormat);

            // this hack is here to make sure toolchain provided source files don't pickup any precompiled headers
            PerFilePchs.Add(null, additionalSourceFilesForFormat);

            var allUsedProviders = new HashSet<ObjectFileProducer>();
            var nonLumpedCompilationUnitInstructions = sourceFiles
                .Select(s =>
            {
                var compiler = objectFileProducers.FirstOrDefault(o => s.HasExtension(o.SupportedExtensions));
                if (compiler == null) return null;
                allUsedProviders.Add(compiler);
                var cui = compiler.ObjectProduceInstructionsFor(this, config, s, AllIncludeDirectoriesFor(config), AllDefinesFor(config), artifactsPath);
                return cui;
            }).Where(o => o != null).ToArray();

            //now we have all vanilla instructions, some of which have a InputPCH set that we're supposed to be using.
            //We're now going to give this whole set of instructions to each of the compilers used, to give them a chance to change something
            //in the instructions. (technically, it will return a new copy). This is used by the precompiled header code, to figure out
            //which files share the same InputPCH _and_ the same compiler settings.  each group like that gets its own precompiled header compiled
            //and we modify the instructions' OutputPCH setting with the filename of that generated precompiled header file.
            //to make things a bit more complicated, we also give the compiler a chance to report additional object files. The usecase for this is the
            //visual studio compiler, that can only create a pch as a side effect of making an object file, and you _have_ to give the objectfile as
            //input to the linker for the link to work. super annoying.
            var additionalObjectFilesBuilder = new List<NPath>();
            foreach (var usedProvider in allUsedProviders)
                nonLumpedCompilationUnitInstructions = usedProvider.PostProcessObjectProductionInstructions(
                    nonLumpedCompilationUnitInstructions, artifactsPath,
                    extraObjectFile => additionalObjectFilesBuilder.Add(extraObjectFile));
            additionalObjectFiles = additionalObjectFilesBuilder.ToArray();

            var lumpBlackList = new HashSet<NPath>(NonLumpableFiles.For(config));

            var filterResult = Filter(nonLumpedCompilationUnitInstructions,
                f => !lumpBlackList.Contains(f.InputFile) && config.Lump &&
                f is CLikeCompilationUnitInstruction);
            var lumpableUnits = filterResult.True.Cast<CLikeCompilationUnitInstruction>().ToList();
            var nonlumpableunits = filterResult.False;

            return Lumping.SetupLumps(artifactsPath, lumpableUnits).Concat(nonlumpableunits).ToArray();
        }

        private NPath[] CombineObjectFilesAndSetupProgram(
            NPath destination,
            NativeProgramConfiguration config, NativeProgramFormat format, NPath artifactsPath,
            ObjectFileProductionInstructions[] allUnits,
            NPath[] additionalObjectFiles,
            ILibrary[] libraries)
        {
            var realLibraries = libraries.OfType<PrecompiledLibrary>();
            var bagsOfObjects = libraries.OfType<BagOfObjectFilesLibrary>();
            additionalObjectFiles = additionalObjectFiles.Concat(bagsOfObjects.SelectMany(b => b.AllObjectFiles)).ToArray();

            foreach (var cui in allUnits)
                cui.TellJamHowToProduce(ObjectPathFor(artifactsPath, cui));

            var allObjectFiles = allUnits
                .Select(o => ObjectPathFor(artifactsPath, o))
                .Concat(additionalObjectFiles)
                .ToArray();

            var targetsForAllObjectFilesToDependOn = StaticLibraries.For(config)
                .SelectMany(s => s.Library.ExtraDependenciesForAllObjectFiles.For(config))
                .Concat(ExtraDependenciesForAllObjectFiles.For(config))
                .Distinct().ToArray();

            foreach (var objectFile in allObjectFiles)
            {
                foreach (var targetForAllObjectFilesToDependOn in targetsForAllObjectFilesToDependOn)
                    Backend.Current.AddDependency(objectFile, targetForAllObjectFilesToDependOn);
            }

            return format.CombineObjectFilesAndSetupProgram(this, config, allObjectFiles, realLibraries, destination, artifactsPath);
        }

        public string[] AllDefinesFor(NativeProgramConfiguration config)
        {
            var ourDefines = Defines.For(config).Concat(PublicDefines.For(config));
            var publicDefinesOfLibraries = PublicDefinesOfLibrariesFor(config);
            var sdkDefines = config.ToolChain.Sdk.Defines;
            var allDefines = ourDefines.Concat(publicDefinesOfLibraries).Concat(sdkDefines).Distinct().ToArray();
            return allDefines;
        }

        public NPath[] AllIncludeDirectoriesFor(NativeProgramConfiguration config)
        {
            var ourIncludes = IncludeDirectories.For(config).Concat(PublicIncludeDirectories.For(config)).ToList();
            var publicIncludesOfLibraries = PublicIncludesOfLibrariesFor(config);
            var allIncludes = ourIncludes.Concat(publicIncludesOfLibraries).Distinct().ToArray();
            return allIncludes;
        }

        public NPath DefaultArtifactsPathFor(NativeProgramConfiguration nativeProgramConfig)
        {
            return Configuration.RootArtifactsPath.Combine(Name, nativeProgramConfig.ToString());
        }

        public static IEnumerable<NPath> SourcesFilesIn(IEnumerable<NPath> sourceFilesAndFolders,
            string[] supportedExtensions = null)
        {
            supportedExtensions = supportedExtensions ?? MostCommonSupportedExtensions;

            foreach (var path in sourceFilesAndFolders)
            {
                if (path.HasExtension(supportedExtensions))
                {
                    yield return path;
                    continue;
                }

                // If the path is in one of our implicit-registering directories, we cannot check it for being a
                // directory, as the implicit-registering directories cannot be read from until the build process
                // itself. As such we'll assume that the path is a file, not a directory, and therefore we already
                // yielded it above (if it had an interesting extension).
                if (Backend.Current.AllDirectoriesProducingImplicitInputs.Any(dir =>
                    dir.MakeAbsolute() == path.MakeAbsolute() || path.IsChildOf(dir)))
                    continue;

                if (path.DirectoryExists())
                {
                    foreach (var file in FilteredGlob.RecursiveGlob(path, "*.*",
                        p =>
                        {
                            return !p.HasDirectory("obj") && !p.HasDirectory("bin") && !p.HasDirectory("artifacts");
                        }))
                        yield return file;
                }
            }
        }

        public virtual string Name
        {
            get
            {
                if (_setName != null)
                    return _setName;
                if (GetType() != typeof(NativeProgram))
                    return GetType().Name;
                throw new ArgumentException("If you do not inherit from NativeProgram, you must provide a name in the constructor");
            }
            set => _setName = value;
        }

        void ValidatePathIsRelative(NPath path)
        {
            if (EnsureAllPathsAreRelative && !path.IsRelative)
                throw new ArgumentException($"Can't use absolute path: {path}");
        }

        private class ConstructorCallback : IDisposable
        {
            // Note that we cannot initialize _constructorCallbacks here because [ThreadStatic] variables are only
            // initialized on the main thread, not on any other threads. We have to leave it null and initialize it when we
            // actually want to use it.
            [ThreadStatic]
            private static List<ConstructorCallback> _constructorCallbacks;

            private readonly Action<NativeProgram> action;

            public ConstructorCallback(Action<NativeProgram> a)
            {
                action = a;

                if (_constructorCallbacks == null)
                    _constructorCallbacks = new List<ConstructorCallback>();

                _constructorCallbacks.Add(this);
            }

            public void Dispose()
            {
                if (_constructorCallbacks == null || !_constructorCallbacks.Remove(this))
                    throw new ObjectDisposedException("ConstructorCallback");

                if (_constructorCallbacks.Count == 0)
                    _constructorCallbacks = null;
            }

            public static void Invoke(NativeProgram np)
            {
                if (_constructorCallbacks == null)
                    return;

                foreach (var callback in _constructorCallbacks)
                    callback.action(np);
            }
        }

        public static IDisposable WithConstructorCallback(Action<NativeProgram> callbackToRunDuringConstruction)
        {
            return new ConstructorCallback(callbackToRunDuringConstruction);
        }
    }
}
