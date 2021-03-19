using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace Unity.InternalCallRegistrationWriter.Collector
{
    public class InternalCallCollector
    {
        private readonly string _assemblyPath;

        private AssemblyDefinition _assemblyDefinition;
        private SortedDictionary<string, List<MethodDefinition>> _internalCallsByType;
        private List<MethodDefinition> _allCollectedCalls;

        public AssemblyDefinition AssemblyDefinition
        {
            get { return _assemblyDefinition ?? (_assemblyDefinition = AssemblyDefinitionFor(AssemblyPath)); }
        }

        public List<MethodDefinition> AllCollectedCalls
        {
            get
            {
                if (_allCollectedCalls == null)
                    CollectAllInternalCalls();

                return _allCollectedCalls;
            }
        }

        public string AssemblyPath
        {
            get { return _assemblyPath; }
        }

        public SortedDictionary<string, List<MethodDefinition>> InternalCallsByType
        {
            get
            {
                if (_internalCallsByType == null)
                    CollectAllInternalCalls();

                return _internalCallsByType;
            }
        }

        public InternalCallCollector(string assemblyPath)
        {
            _assemblyPath = assemblyPath;
        }

        private AssemblyDefinition AssemblyDefinitionFor(string path)
        {
            return AssemblyDefinition.ReadAssembly(path);
        }

        private List<MethodDefinition> CollectAllInternalCallsIn(TypeDefinition type)
        {
            if (type.Methods == null || type.Methods.Count == 0)
                return null;

            return type.Methods.Where(m => m.IsInternalCall).ToList();
        }

        public List<MethodDefinition> CollectedCallsFor(string typeFullName)
        {
            return InternalCallsByType.Single(kvp => kvp.Key == typeFullName).Value;
        }

        private void CollectAllInternalCalls()
        {
            InitializeCollections();

            var typeDefinitions = AssemblyDefinition.MainModule.GetTypes();
            if (typeDefinitions == null)
                return;

            foreach (var type in typeDefinitions)
            {
                var icalls = CollectAllInternalCallsIn(type);
                if (icalls == null || icalls.Count == 0)
                    continue;

                icalls.Sort(InternalCallNameComparison);
                _allCollectedCalls.AddRange(icalls);
                _internalCallsByType[type.FullName] = icalls;
            }
        }

        private void InitializeCollections()
        {
            _internalCallsByType = new SortedDictionary<string, List<MethodDefinition>>();
            _allCollectedCalls = new List<MethodDefinition>();
        }

        private static int InternalCallNameComparison(MethodDefinition methodDefinition, MethodDefinition otherDefinition)
        {
            return String.Compare(methodDefinition.FullName, otherDefinition.FullName, System.StringComparison.Ordinal);
        }
    }
}
