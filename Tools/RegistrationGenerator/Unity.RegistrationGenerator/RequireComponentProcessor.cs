using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Unity.RegistrationGenerator
{
    public class RequireComponentProcessor : CodegenProcessor
    {
        // The RequireComponent attribute lets automatically add required component as a dependency.
        public class ExtractedRequireComponent
        {
            public TypeDefinition m_Type0;
            public TypeDefinition m_Type1;
            public TypeDefinition m_Type2;

            public ExtractedRequireComponent(CustomAttribute attributeFromAssembly)
            {
                m_Type0 = ((TypeReference)attributeFromAssembly.ConstructorArguments[0].Value).Resolve();

                if (attributeFromAssembly.ConstructorArguments.Count > 1)
                {
                    m_Type1 = ((TypeReference)attributeFromAssembly.ConstructorArguments[1].Value).Resolve();

                    if (attributeFromAssembly.ConstructorArguments.Count > 2)
                        m_Type2 = ((TypeReference)attributeFromAssembly.ConstructorArguments[2].Value).Resolve();
                }
            }
        }

        IEnumerable<Tuple<TypeDefinition, IEnumerable<ExtractedRequireComponent>>> m_ComponentRequirements;
        AssemblyDefinition m_CoreAssembly;

        public RequireComponentProcessor(AssemblyDefinition coreAssembly)
        {
            m_CoreAssembly = coreAssembly;
        }

        internal override void Process(ICppCodeGeneration generator)
        {
            m_ComponentRequirements = GetComponentRequirements(generator.GetAssembly(), m_CoreAssembly);

            foreach (var i in m_ComponentRequirements)
            {
                generator.AddClassDeclaration(i.Item1);
                foreach (var componentRequire in i.Item2)
                {
                    generator.AddClassDeclaration(componentRequire.m_Type0);
                    if (componentRequire.m_Type1 != null)
                        generator.AddClassDeclaration(componentRequire.m_Type1);
                    if (componentRequire.m_Type2 != null)
                        generator.AddClassDeclaration(componentRequire.m_Type2);
                }
            }
        }

        internal override bool HasData()
        {
            return true;
        }

        internal override int CodegenRegistrationData(Dictionary<TypeDefinition, NativeName> types, StringBuilder output)
        {
            const string kFormat = "    {{WeakTypeOf<{0}>(), WeakTypeOf<{1}>()}},\n";
            int count = 0;
            foreach (var i in m_ComponentRequirements)
            {
                string componentName = types[i.Item1].QualifiedName;
                foreach (var componentRequire in i.Item2)
                {
                    count++;
                    output.AppendFormat(kFormat, componentName, types[componentRequire.m_Type0].QualifiedName);
                    if (componentRequire.m_Type1 != null)
                    {
                        count++;
                        output.AppendFormat(kFormat, componentName, types[componentRequire.m_Type1].QualifiedName);
                    }
                    if (componentRequire.m_Type2 != null)
                    {
                        count++;
                        output.AppendFormat(kFormat, componentName, types[componentRequire.m_Type2].QualifiedName);
                    }
                }
            }
            return count;
        }

        internal static IEnumerable<Tuple<TypeDefinition, IEnumerable<ExtractedRequireComponent>>> GetComponentRequirements(AssemblyDefinition assembly, AssemblyDefinition coreAssembly = null)
        {
            var result = new List<Tuple<TypeDefinition, IEnumerable<ExtractedRequireComponent>>>();
            var attributeAssembly = coreAssembly ?? assembly;
            var requireComponentAttributeType = attributeAssembly.MainModule.Types.FirstOrDefault(a => a.FullName == "UnityEngine.RequireComponent");
            var componentType = attributeAssembly.MainModule.Types.FirstOrDefault(a => a.FullName == "UnityEngine.Component");

            if (componentType != null && requireComponentAttributeType != null)
            {
                foreach (var component in assembly.GetAllTypes().Where(t => Utils.IsSubclassOf(t, componentType)))
                {
                    var requiredAttributes = Utils.GetCustomAttributesIncludingParent(component, requireComponentAttributeType).ToArray();
                    if (requiredAttributes.Length > 0)
                        result.Add(Tuple.Create(component, requiredAttributes.Select(b => new ExtractedRequireComponent(b))));
                }
            }

            return result;
        }
    }
}
