using System.Collections;
using System.Collections.Generic;
using Mono.Cecil;

namespace Unity.BindingsGenerator.Core.AssemblyResolver
{
    internal sealed class DefaultAssemblyResolver : BaseAssemblyResolver
    {
        readonly Dictionary<string, AssemblyDefinition> _assemblies;

        public IDictionary AssemblyCache
        {
            get { return _assemblies; }
        }

        public DefaultAssemblyResolver()
            : this(new Dictionary<string, AssemblyDefinition>())
        {
        }

        public DefaultAssemblyResolver(Dictionary<string, AssemblyDefinition> assemblyCache)
        {
            _assemblies = assemblyCache;

            // Remove search directories that Cecil adds. We will explicitly add the search directories we need.
            RemoveSearchDirectory(".");
            RemoveSearchDirectory("bin");
        }

        public override AssemblyDefinition Resolve(AssemblyNameReference name, ReaderParameters parameters)
        {
            AssemblyDefinition asm;

            if (_assemblies.TryGetValue(name.Name, out asm))
                return asm;

            asm = base.Resolve(name, parameters);
            _assemblies.Add(name.Name, asm);

            return asm;
        }
    }
}
