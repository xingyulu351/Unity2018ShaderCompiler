using Mono.Cecil;
using Mono.Cecil.Mdb;
using Mono.Cecil.Pdb;
using System;
using System.Collections.Generic;
using System.IO;

namespace Unity.ReferenceRewriter
{
    public static class RewriteResolver
    {
        public static SearchPathAssemblyResolver CreateAssemblyResolver(IEnumerable<string> targetModules, string[] frameworkPaths, string projectLockFile, string[] additionalReferences, string platformPath)
        {
            if (string.IsNullOrEmpty(projectLockFile))
                CheckFrameworkPaths(frameworkPaths);

            var resolver = string.IsNullOrEmpty(projectLockFile) ? new SearchPathAssemblyResolver() : new NuGetAssemblyResolver(projectLockFile);

            foreach (var targetModule in targetModules)
            {
                var targetDirectory = Path.GetDirectoryName(targetModule);
                resolver.AddSearchDirectory(targetDirectory);
            }

            foreach (var frameworkPath in frameworkPaths)
            {
                var fullFrameworkPath = Path.GetFullPath(frameworkPath);
                resolver.AddSearchDirectory(fullFrameworkPath);
            }

            foreach (var referenceDirectory in additionalReferences)
            {
                resolver.AddSearchDirectory(Path.GetFullPath(referenceDirectory));
            }

            if (!string.IsNullOrEmpty(platformPath))
                ReadModule(platformPath, resolver);

            return resolver;
        }

        public static ModuleDefinition ReadModule(string path, SearchPathAssemblyResolver resolver)
        {
            ModuleDefinition module;

            try
            {
                module = ModuleDefinition.ReadModule(path, TargetModuleParameters(path, DetectDebugSymbolFormat(path), resolver));
            }
            catch (InvalidOperationException)
            {
                module = ModuleDefinition.ReadModule(path, TargetModuleParameters(path, DebugSymbolFormat.None, resolver));
            }

            resolver.AddAssembly(module.Assembly);
            return module;
        }

        private static DebugSymbolFormat DetectDebugSymbolFormat(string targetModule)
        {
            if (File.Exists(Path.ChangeExtension(targetModule, ".pdb")))
                return DebugSymbolFormat.Pdb;
            if (File.Exists(targetModule + ".mdb"))
                return DebugSymbolFormat.Mdb;
            return DebugSymbolFormat.None;
        }

        public static Dictionary<string, ModuleDefinition[]> ResolveAltModules(IDictionary<string, IList<string>> alt, SearchPathAssemblyResolver resolver)
        {
            var altModules = new Dictionary<string, ModuleDefinition[]>();

            foreach (var pair in alt)
            {
                var modules = new ModuleDefinition[pair.Value.Count];

                for (var i = 0; i < modules.Length; ++i)
                {
                    var name = pair.Value[i];
                    var winmd = name.EndsWith(".winmd", StringComparison.InvariantCultureIgnoreCase);
                    if (winmd || name.EndsWith(".dll", StringComparison.InvariantCultureIgnoreCase))
                        name = Path.GetFileNameWithoutExtension(name);
                    var assemblyName = AssemblyNameReference.Parse(name);
                    if (winmd)
                    {
                        assemblyName.IsWindowsRuntime = true;
                        assemblyName.Version = new Version(255, 255, 255, 255);
                    }
                    modules[i] = resolver.Resolve(assemblyName, new ReaderParameters { AssemblyResolver = resolver }).MainModule;
                }

                altModules.Add(pair.Key, modules);
            }

            return altModules;
        }

        private static void CheckFrameworkPath(string frameworkPath)
        {
            if (frameworkPath == null)
                throw new ArgumentNullException("frameworkPath");
            if (string.IsNullOrEmpty(frameworkPath))
                throw new ArgumentException("Empty framework path", "frameworkPath");
            if (!Directory.Exists(frameworkPath))
                throw new ArgumentException("Reference path doesn't exist.", "frameworkPath");
            if (!File.Exists(Path.Combine(frameworkPath, "mscorlib.dll")))
                throw new ArgumentException("No mscorlib.dll in the framework path.", "frameworkPath");
        }

        private static void CheckFrameworkPaths(string[] frameworkPaths)
        {
            int timesFoundMscorlib = 0;
            foreach (var path in frameworkPaths)
            {
                try
                {
                    CheckFrameworkPath(path);
                    timesFoundMscorlib++;
                }
                catch (ArgumentException e)
                {
                    if (!e.Message.Contains(@"No mscorlib.dll in the framework path."))
                    {
                        throw;
                    }
                }
            }

            if (timesFoundMscorlib == 0)
            {
                throw new ArgumentException("No mscorlib.dll in the framework path.", "frameworkPaths");
            }
        }

        private static ReaderParameters TargetModuleParameters(string targetModule, DebugSymbolFormat symbolFormat, IAssemblyResolver resolver)
        {
            var targetParameters = new ReaderParameters { AssemblyResolver = resolver };

            if (File.Exists(Path.ChangeExtension(targetModule, ".pdb")) && symbolFormat == DebugSymbolFormat.Pdb)
            {
                targetParameters.SymbolReaderProvider = new PdbReaderProvider();
                targetParameters.ReadSymbols = true;
            }

            if (File.Exists(targetModule + ".mdb") && symbolFormat == DebugSymbolFormat.Mdb)
            {
                targetParameters.SymbolReaderProvider = new MdbReaderProvider();
                targetParameters.ReadSymbols = true;
            }

            return targetParameters;
        }
    }
}
