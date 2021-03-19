using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Mono.Cecil;
using Mono.Options;

namespace Unity.ReferenceRewriter
{
    static class Program
    {
        static int Main(string[] args)
        {
            var help = false;
            var targetModules = new List<string>();
            var supportModule = "";
            var supportModulePartialNamespace = "";
            var frameworkPath = "";
            var projectLockFile = "";
            var additionalReference = "";
            var platformPath = "";
            var systemNamespace = "";
            var strongNamedReferences = "";
            var winmdReferences = "";
            var symbolFormat = DebugSymbolFormat.Pdb;
            var alt = new Dictionary<string, IList<string>>();
            var ignore = new Dictionary<string, IList<string>>();

            args = ProcessResponseFiles(args);

            var set = new OptionSet
            {
                { "target=", "The target module to rewrite.", t => targetModules.Add(t) },
                { "support=", "The support module containing the replacement API.", s => supportModule = s },
                { "supportpartialns=", "Namespace in the support module that implements partial types.", s => supportModulePartialNamespace = s },
                { "framework=", "A comma separated list of the directories of the target framework. Reference rewriter will assume that it will not process files in those directories",
                  f => frameworkPath = f },
                { "lock=", "Path to project.lock.json file.", l => projectLockFile = l },
                { "additionalreferences=", "A comma separated list of the directories to reference additionally. Reference rewriter will assume that it will process files in those directories",
                  ar => additionalReference = ar },
                { "platform=", "Path to platform assembly.", p => platformPath = p },
                { "system=", "The support namespace for System.", s => systemNamespace = s },
                { "snrefs=", "A comma separated list of assembly names that retain their strong names.", s => strongNamedReferences = s },
                { "winmdrefs=", "A comma separated list of assembly names that should be redirected to winmd references.", s => winmdReferences = s },
                { "dbg=", "File format of the debug symbols. Either none, mdb or pdb.", d => symbolFormat = SymbolFormat(d) },
                { "alt=", "A semicolon separated list of alternative namespace and assembly mappings.", a => AltFormat(alt, a) },
                { "ignore=", "A semicolon separated list of assembly qualified type names that should not be resolved.", i => IgnoreFormat(ignore, i) },

                { "?|h|help", h => help = true },
            };

            try
            {
                set.Parse(args);
            }
            catch (OptionException)
            {
                Usage(set);
                return 3;
            }

            if (help || targetModules.Count == 0 || new[] {frameworkPath, projectLockFile}.All(string.IsNullOrWhiteSpace))
            {
                Usage(set);
                return 2;
            }

            try
            {
                var operation = RewriteOperation.Create(
                    ns => ns.StartsWith("System")
                    ? systemNamespace + ns.Substring("System".Length)
                    : ns);

                var frameworkPaths = frameworkPath.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                var additionalReferences = additionalReference.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                var strongNamedReferencesArray = strongNamedReferences.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                var winmdReferencesArray = winmdReferences.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

                var resolver = RewriteResolver.CreateAssemblyResolver(targetModules, frameworkPaths, projectLockFile, additionalReferences, platformPath);

                ModuleDefinition support = null;
                if (!string.IsNullOrWhiteSpace(supportModule))
                    support = RewriteResolver.ReadModule(supportModule, resolver);

                var targets = new List<ModuleDefinition>();
                var rewriteContexts = new List<RewriteContext>();

                // First read target modules. It could be that one of our target modules is an alt module!
                foreach (var targetModule in targetModules)
                    targets.Add(RewriteResolver.ReadModule(targetModule, resolver));

                // Now, resolve alt modules
                var altModules = RewriteResolver.ResolveAltModules(alt, resolver);

                // Then, create rewrite contexts for each target
                foreach (var target in targets)
                {
                    var context = RewriteContext.For(target, resolver, symbolFormat, support, supportModulePartialNamespace, frameworkPaths, strongNamedReferencesArray, winmdReferencesArray, altModules, ignore);
                    rewriteContexts.Add(context);
                }

                // Finally, rewrite our targets
                foreach (var context in rewriteContexts)
                {
                    operation.Execute(context);

                    if (context.RewriteTarget)
                        context.Save(context.TargetModule.FileName);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Catastrophic failure while running rrw: {0}", e);
                return 1;
            }

            return 0;
        }

        private static string[] ProcessResponseFiles(string[] args)
        {
            var processedArgs = new List<string>();

            foreach (var arg in args)
            {
                if (arg.StartsWith("@"))
                {
                    processedArgs.AddRange(File.ReadAllLines(arg.Substring(1)).Where(line => !string.IsNullOrEmpty(line)));
                }
                else
                {
                    processedArgs.Add(arg);
                }
            }

            return processedArgs.ToArray();
        }

        private static DebugSymbolFormat SymbolFormat(string d)
        {
            return string.Equals(d, "mdb", StringComparison.OrdinalIgnoreCase)
                ? DebugSymbolFormat.Mdb
                : DebugSymbolFormat.Pdb;
        }

        private static void AltFormat(IDictionary<string, IList<string>> alt, string a)
        {
            foreach (var pair in a.Split(';'))
            {
                var parts = pair.Split(new char[] { ',' }, 2);

                var @namespace = parts[0];
                var assemblyName = @namespace;

                if (parts.Length > 1)
                    assemblyName = parts[1];

                IList<string> assemblyNames;

                if (!alt.TryGetValue(@namespace, out assemblyNames))
                {
                    assemblyNames = new List<string>();
                    alt.Add(@namespace, assemblyNames);
                }

                assemblyNames.Add(assemblyName);
            }
        }

        private static void IgnoreFormat(IDictionary<string, IList<string>> ignore, string i)
        {
            foreach (var pair in i.Split(';'))
            {
                var parts = pair.Split(new char[] { ',' }, 2);

                if (parts.Length != 2)
                    throw new OptionException("Type name is not assembly qualified.", "ignore");

                var typeName = parts[0];
                var assemblyName = parts[1];

                IList<string> typeNames;

                if (!ignore.TryGetValue(assemblyName, out typeNames))
                {
                    typeNames = new List<string>();
                    ignore.Add(assemblyName, typeNames);
                }

                typeNames.Add(typeName);
            }
        }

        private static void Usage(OptionSet set)
        {
            Console.WriteLine("rrw reference rewriter");
            set.WriteOptionDescriptions(Console.Out);
        }

        public static string ConvertToWindowsPath(this string path)
        {
            return path.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
        }
    }
}
