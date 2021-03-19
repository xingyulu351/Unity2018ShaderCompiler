using Bee.Core;
using NiceIO;
using System;
using System.Collections.Generic;
using System.Linq;
using Bee.DotNet;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    public abstract class CSharpCompiler
    {
        protected abstract RunnableProgram CompilerProgram { get; }

        public virtual string ActionName => GetType().Name;
        public virtual Func<CSharpCompiler> StaticFunctionToCreateMe => null;
        public abstract int PreferredUseScore { get; }

        protected abstract void AddCompilerSpecificArguments(List<string> arguments, List<string> nonRspArgs);


        public DotNetAssembly SetupInvocation(NPath targetFile,
            NPath[] sources,
            Framework framework,
            NPath[] prebuiltReferences,
            string[] systemReferences,
            string[] defines,
            bool @unsafe,
            int[] ignoredWarnings,
            bool warningsAsErrors,
            string langversion,
            EmbeddedResource[] resources,
            ReferenceAssemblyProvider referenceAssemblyProvider = null)
        {
            referenceAssemblyProvider = referenceAssemblyProvider ?? ReferenceAssemblyProvider.BestAvailable;

            var args = new List<string>();

            if (targetFile.HasExtension("dll"))
                args.Add("-target:library");

            if (targetFile.HasExtension("winmd"))
                throw new NotSupportedException("CSharpCompiler does not support outputing winmd files as that requires it to pipe it through winmdexp.exe, which we currently don't ship in the repository.");

            args.Add($"-out:{targetFile}");

            NPath refOutAssembly = null;
            if (SupportsRefOutAssembly)
            {
                var hash = new KnuthHash();
                hash.Add(targetFile);
                refOutAssembly = $"artifacts/Bee.CSharpSupport/referenceassemblies/{targetFile.FileName}/{hash.Value}.dll";
                args.Add(ArgumentForRefOutAssembly(refOutAssembly));
            }

            if (@unsafe)
                args.Add("-unsafe");

            if (warningsAsErrors)
                args.Add("-warnaserror");

            foreach (var define in defines ?? Array.Empty<string>())
                args.Add($"-define:{define}");

            if (!referenceAssemblyProvider.TryFor(framework, false, out var referenceAssemblies, out var reason))
                throw new ArgumentException($"No reference assemblies found for {framework} reason: {reason}. provider: {referenceAssemblyProvider.GetType()}");

            var bclReferences = framework.IsDotNetFramework
                ? FindSystemReferencesInReferenceAssemblies(systemReferences, referenceAssemblies).Concat(referenceAssemblies.Where(r => r.HasDirectory("Facades")))
                : referenceAssemblies;

            foreach (var reference in bclReferences.Concat(prebuiltReferences ?? Array.Empty<NPath>()))
                args.Add($"-r:{reference.InQuotes()}");

            foreach (var source in sources)
                args.Add(source.InQuotes(SlashMode.Native));

            if (ignoredWarnings != null && ignoredWarnings.Any())
                args.Add($"-nowarn:{ignoredWarnings.Select(i => i.ToString()).SeparateWithComma()}");

            foreach (var resource in resources ?? Array.Empty<EmbeddedResource>())
            {
                var postfix = resource.Identifier == null ? "" : "," + resource.Identifier;
                args.Add($"-resource:{resource.PathToEmbed.InQuotes()}{postfix}");
            }

            args.Add($"-langversion:{langversion}");

            var nonRspArgs = new List<string> {"/nostdlib", "/noconfig"};

            AddCompilerSpecificArguments(args, nonRspArgs);

            var resourceFiles = resources?.Select(r => r.PathToEmbed) ?? Array.Empty<NPath>();

            var builtCSharpProgram = new DotNetAssembly(targetFile, framework, DebugSymbolFormat, PathOfGeneratedSymbolsFor(targetFile), referenceAssemblyPath: refOutAssembly);

            var targets = new List<NPath>() {builtCSharpProgram.Path, builtCSharpProgram.DebugSymbolPath};
            if (refOutAssembly != null)
                targets.Add(refOutAssembly);

            Backend.Current.AddAction(
                ActionName,
                targets.ToArray(),
                sources.Concat(prebuiltReferences).Concat(resourceFiles).Append(this.CompilerProgram.Path).ToArray(),
                CompilerProgram.InvocationString + " " + nonRspArgs.ToArray().SeparateWithSpace(),
                args.ToArray(),
                true);

            return builtCSharpProgram;
        }

        protected virtual string ArgumentForRefOutAssembly(NPath refOutAssembly)
        {
            throw new NotImplementedException();
        }

        public virtual bool SupportsRefOutAssembly => false;

        public abstract DebugFormat DebugSymbolFormat { get; }

        protected abstract NPath PathOfGeneratedSymbolsFor(NPath targetFile);

        private static IEnumerable<NPath> FindSystemReferencesInReferenceAssemblies(string[] systemReferences, NPath[] referenceAssemblies)
        {
            var defaultReferences = new[] {"mscorlib", "System", "System.Core", "System.Xml"};
            var systemReferencesToUse = systemReferences.Concat(defaultReferences).Distinct();

            foreach (var systemReference in systemReferencesToUse)
            {
                var assembly = referenceAssemblies.SingleOrDefault(a => 0 == string.Compare(a.FileNameWithoutExtension, systemReference, StringComparison.InvariantCultureIgnoreCase));
                if (assembly == null)
                    throw new ArgumentException($"Cannot find system reference {systemReference} in assembly references");
                yield return assembly;
            }
        }

        public abstract bool SupportedOnHostPlatform();
        public abstract bool CanBuild();
    }
}
