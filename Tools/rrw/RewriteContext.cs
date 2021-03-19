using System;
using System.Collections.Generic;
using System.IO;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Mdb;
using Mono.Cecil.Pdb;
using Mono.Collections.Generic;
using Unity;

namespace Unity.ReferenceRewriter
{
    public class RewriteContext
    {
        public bool RewriteTarget { get; set; }
        public ModuleDefinition TargetModule { get; private set; }
        public ModuleDefinition SupportModule { get; private set; }
        public string SupportModulePartialNamespace { get; private set; }
        public IDictionary<string, ModuleDefinition[]> AltModules { get; private set; }
        public IDictionary<string, IList<string>> IgnoredTypes { get; private set; }
        public string[] FrameworkPaths { get; private set; }
        public IAssemblyResolver AssemblyResolver { get; private set; }
        public Collection<string> StrongNameReferences { get; private set; }
        public Collection<string> WinmdReferences { get; private set; }
        public DebugSymbolFormat DebugSymbolFormat { get; private set; }

        public static RewriteContext For(ModuleDefinition targetModule, IAssemblyResolver resolver, DebugSymbolFormat symbolFormat, ModuleDefinition supportModule, string supportModulePartialNamespace,
            string[] frameworkPaths, ICollection<string> strongNamedReferences, ICollection<string> winmdReferences, IDictionary<string, ModuleDefinition[]> altModules, IDictionary<string, IList<string>> ignore)
        {
            if (targetModule == null)
                throw new ArgumentNullException("targetModule");

            return new RewriteContext
            {
                TargetModule = targetModule,
                SupportModule = supportModule,
                SupportModulePartialNamespace = supportModulePartialNamespace,
                AltModules = altModules,
                IgnoredTypes = ignore,
                FrameworkPaths = frameworkPaths,
                AssemblyResolver = resolver,
                StrongNameReferences = new Collection<string>(strongNamedReferences),
                WinmdReferences = new Collection<string>(winmdReferences),
                DebugSymbolFormat = symbolFormat,
                RewriteTarget = !DoesSymbolFormatMatch(symbolFormat, targetModule)
            };
        }

        private static bool DoesSymbolFormatMatch(DebugSymbolFormat symbolFormat, ModuleDefinition targetModule)
        {
            switch (symbolFormat)
            {
                case DebugSymbolFormat.None:
                    return targetModule.SymbolReader == null;

                case DebugSymbolFormat.Mdb:
                    return targetModule.SymbolReader is MdbReader;

                case DebugSymbolFormat.Pdb:
                    return targetModule.SymbolReader is NativePdbReader;
            }

            return false;
        }

        public void Save(string targetModule)
        {
            var parameters = new WriterParameters();
            if (TargetModule.HasSymbols)
            {
                parameters.SymbolWriterProvider = DebugSymbolFormat == DebugSymbolFormat.Mdb
                    ? (ISymbolWriterProvider) new MdbWriterProvider()
                    : new PdbWriterProvider();
                parameters.WriteSymbols = true;
            }

            TargetModule.Write(targetModule, parameters);
        }
    }
}
