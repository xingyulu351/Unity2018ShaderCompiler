using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Modules
{
    public class ModuleBase
    {
        public bool WasCreatedFromLegacyModule { get; }
        readonly string _name;
        public string Name => _name ?? GetType().Name.StripEnd("Module");
        public NPath Icon { get; protected set; }
        public virtual bool ForceSourceBuild { get; } = false;

        //we do not support having different bindings for different variations
        public List<NPath> LegacyBindings = new List<NPath>();

        public CollectionWithConditions<NPath, UnityPlayerConfiguration> Cs = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> Cpp = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();
        public CollectionWithConditions<string, UnityPlayerConfiguration> Defines = new CollectionWithConditions<string, UnityPlayerConfiguration>();
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> IncludeDirectories = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();
        public CollectionWithConditions<string, UnityPlayerConfiguration> Libraries = new CollectionWithConditions<string, UnityPlayerConfiguration>();
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> DynamicLibraries = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> PrebuiltLibraries = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> ManagedPrebuiltReference = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();
        public CollectionWithConditions<string, UnityPlayerConfiguration> ManagedSystemReference = new CollectionWithConditions<string, UnityPlayerConfiguration>();
        public CollectionWithConditions<string, UnityPlayerConfiguration> Dependencies = new CollectionWithConditions<string, UnityPlayerConfiguration>();
        public CollectionWithConditions<string, UnityPlayerConfiguration> Frameworks = new CollectionWithConditions<string, UnityPlayerConfiguration>();
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> NonLumpableFiles = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();
        public CollectionWithConditions<FromSourceLibraryDependency, UnityPlayerConfiguration> StaticLibraries { get; } = new CollectionWithConditions<FromSourceLibraryDependency, UnityPlayerConfiguration>();
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> ExtraDependenciesForAllObjectFiles = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();

        //You should never use this. The only reason it exists, is that we have a combiniation of two problems that are not super easy to solve:
        //1) inability for old buildcode to apply a module's include directories on the module's bindingfile's cpp file
        //2) physics2D currently needing that feature.  It shouldn't need it, as it's very very ugly, but today it does, and requires some serious refactoring to no longer need it
        public CollectionWithConditions<NPath, UnityPlayerConfiguration> GlobalIncludeDirectories_IfYouNeedThisYouAreDoingItWrong = new CollectionWithConditions<NPath, UnityPlayerConfiguration>();

        public bool HasManagedCode => LegacyBindings.Any() || Cs.ForAny().Any();
        public virtual bool IsEditorModule => Name.EndsWith("Editor");

        // We default to true for IsStrippable, unless a module overrides it. Editor modules should not be strippable,
        // but they are ignored by the stripping system in the player build code anyways, so this field is not relevant
        // for those anyways.
        public bool IsStrippable = true;
        public bool GenerateBuiltinPackage = false;

        public ModuleBase() : this(null)
        {
        }
        public ModuleBase(string name, bool wasCreatedFromLegacyModule = false)
        {
            WasCreatedFromLegacyModule = wasCreatedFromLegacyModule;
            _name = name;
        }
    }

    public static class LegacyModule
    {
        public static ModuleBase CreateModuleFromLegacyModule(string name, string legacyTargetName, IEnumerable<UnityPlayerConfiguration> variations)
        {
            var result = new ModuleBase(name, true);
            foreach (var variation in variations)
            {
                var platformIdentifier = variation.ToolChain.LegacyPlatformIdentifier;

                result.Cs.Add(c => c == variation, InvokeRule(platformIdentifier, name, legacyTargetName, "Cs").Select(n => new NPath(n)));
                result.Cpp.Add(c => c == variation, InvokeRule(platformIdentifier, name, legacyTargetName, "Cpp").Select(n => new NPath(n)));
                result.Defines.Add(c => c == variation, InvokeRule(platformIdentifier, name, legacyTargetName, "Defines"));
                result.PrebuiltLibraries.Add(c => c == variation, InvokeRule(platformIdentifier, name, legacyTargetName, "PrebuiltLibraries").Select(n => new NPath(n)));
                result.Frameworks.Add(c => c == variation, InvokeRule(platformIdentifier, name, legacyTargetName, "Frameworks"));
                result.IncludeDirectories.Add(c => c == variation, InvokeRule(platformIdentifier, name, legacyTargetName, "Includes").Concat(InvokeRule(platformIdentifier, name, legacyTargetName, "GlobalIncludeDirectories_IfYouNeedThisYouAreDoingItWrong")).Select(n => new NPath(n)));
                IEnumerable<string> dependencies = InvokeRule(platformIdentifier, name, legacyTargetName, "Dependencies");

                result.DynamicLibraries.Add(c => c == variation, InvokeRule(platformIdentifier, name, legacyTargetName, "DynamicLibraries").Select(n => new NPath(n)));

                if (name != "SharedInternals" && name != "Core")
                    dependencies = dependencies.Append("Core");
                if (name != "SharedInternals")
                    dependencies = dependencies.Append("SharedInternals");
                result.Dependencies.Add(c => c == variation, dependencies.Distinct());

                result.IsStrippable = InvokeRule(platformIdentifier, name, legacyTargetName, "IsStrippable").FirstOrDefault() != "False";
                result.GenerateBuiltinPackage = InvokeRule(platformIdentifier, name, legacyTargetName, "GenerateBuiltinPackage").FirstOrDefault() == "True";
            }
            result.LegacyBindings.AddRange(InvokeRule(variations.First().ToolChain.LegacyPlatformIdentifier, name, legacyTargetName, "Bindings").Select(n => new NPath(n)));

            return result;
        }

        public static bool NewBuildProgramActive { get; private set; }
        public static string[] InvokeRule(string LegacyPlatformIdentifier, string moduleName, string legacyTargetName, string reportString)
        {
            var ruleName = $"{moduleName}Module_Report{reportString}";

            if (!Rules.HasRule(ruleName))
                return Array.Empty<string>();

            var backup = ConvertedJamFile.Vars.PLATFORM.Clone();

            var targetNameBackup = ConvertedJamFile.Vars.currentTarget.Clone();

            if (LegacyPlatformIdentifier != null)
                ConvertedJamFile.Vars.PLATFORM.Assign(LegacyPlatformIdentifier);

            ConvertedJamFile.Vars.currentTarget.Assign(legacyTargetName);

            NewBuildProgramActive = true;
            var result = Rules.Invoke(ruleName, Array.Empty<JamList>()).Elements;
            NewBuildProgramActive = false;
            if (LegacyPlatformIdentifier != null)
                ConvertedJamFile.Vars.PLATFORM.Assign(backup);

            ConvertedJamFile.Vars.currentTarget.Assign(targetNameBackup);
            return result;
        }
    }

    class Module : ModuleBase
    {
        public NPath Directory => $"Modules/{Name}";

        readonly NPath[] _pathsToIgnoreWhileGlobbing;
        public override bool IsEditorModule => Directory.DirectoryExists("Editor") || Name.EndsWith("Editor");


        protected virtual NPath[] PathsToIgnoreWhileGlobbing => Array.Empty<NPath>();

        public Module() : this(null)
        {
        }

        public Module(string name) : base(name)
        {
            _pathsToIgnoreWhileGlobbing = PathsToIgnoreWhileGlobbing;

            LegacyBindings.AddRange(FilesWithExtensions(Directory, "bindings"));
            NPath testsDir = Directory.Combine("Tests/");
            var csFiles = FilesWithExtensions(Directory, "cs").Where(p => !p.FileName.EndsWith(".jam.cs") && !p.IsChildOf(testsDir)).ToArray();

            Cs.Add(csFiles);

            Cpp.Add(c => FilesWithExtensions(Directory, ExtensionsFor(c)).Where(p => !p.IsChildOf(testsDir)).ToArray());

            if (Name != "Core")
                Dependencies.Add("Core");
            if (Name != "SharedInternals")
                Dependencies.Add("SharedInternals");
            if (IsEditorModule && Name != "Editor")
                Dependencies.Add("Editor");

            Cpp.Add(c =>
            {
                var platformDir = new NPath($"Platforms/{c.ToolChain.Platform.Name}/Modules/{Name}");
                return FilesWithExtensions(platformDir,  ExtensionsFor(c));
            });

            Cs.Add(c =>
            {
                var platformDir = new NPath($"Platforms/{c.ToolChain.Platform.Name}/Modules/{Name}");
                var platformTestsDir = platformDir.Combine("Tests/");
                return FilesWithExtensions(platformDir, "cs").Where(p => !p.FileName.EndsWith(".jam.cs") && !p.IsChildOf(platformTestsDir));
            });
            var iconPath = Directory.Combine("icon.png");
            if (iconPath.FileExists())
                Icon = iconPath;
        }

        private static string[] ExtensionsFor(UnityPlayerConfiguration c)
        {
            var defaultCppExtensions2 = new[] {"c", "cpp"};
            return (c.ToolChain.CppCompiler?.SupportedExtensions ?? defaultCppExtensions2).Append("h", "inc").ToArray();
        }

        static Dictionary<string, NPath[]> _globCache = new Dictionary<string, NPath[]>();

        protected NPath[] FilesWithExtensions(NPath directory, params string[] extensions)
        {
            // Module setup code is executed many times, often with identical globbing queries.
            // Save repeated globbing by having a cache keyed with folder + extensions + ignore-list.
            var key = directory.ToString();
            if (extensions.Any())
                key = key + "!" + extensions.Aggregate((cur, next) => cur + "|" + next);
            if (_pathsToIgnoreWhileGlobbing.Any())
                key = key + "!" + _pathsToIgnoreWhileGlobbing.Aggregate((cur, next) => cur + "|" + next);
            if (_globCache.ContainsKey(key))
                return _globCache[key];

            NPath[] res;
            if (!directory.DirectoryExists())
            {
                res = Array.Empty<NPath>();
            }
            else
            {
                res = directory.Files(extensions, true);
                res = res.Where(f => !_pathsToIgnoreWhileGlobbing.Any(f.IsChildOf)).ToArray();
            }
            _globCache[key] = res;
            return res;
        }
    }
}
