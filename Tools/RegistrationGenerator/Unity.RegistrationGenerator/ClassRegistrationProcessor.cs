using Mono.Cecil;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Unity.RegistrationGenerator
{
    public class ClassRegistrationProcessor : CodegenProcessor
    {
        public static List<TypeDefinition> Types = new List<TypeDefinition>();
        readonly AssemblyDefinition m_CoreAssembly;

        public ClassRegistrationProcessor(AssemblyDefinition coreAssembly)
        {
            m_CoreAssembly = coreAssembly;
        }

        internal override void Process(ICppCodeGeneration generator)
        {
            var assembly = generator.GetAssembly();
            var attributeAssembly = m_CoreAssembly ?? assembly;
            var baseObjectType = attributeAssembly.MainModule.Types.FirstOrDefault(a => a.FullName == "UnityEngine.Object");

            if (baseObjectType == null)
                return;

            var scriptableObjectTypeNames = new[]
            {
                "UnityEngine.ScriptableObject",
                "UnityEngine.MonoBehaviour",
                "UnityEditor.Experimental.AssetImporters.ScriptedImporter"
            };
            var scriptableObjectTypes = assembly.GetAllTypes().Where(a => scriptableObjectTypeNames.Contains(a.FullName));
            if (attributeAssembly != assembly)
                scriptableObjectTypes = scriptableObjectTypes.Concat(attributeAssembly.GetAllTypes().Where(a => scriptableObjectTypeNames.Contains(a.FullName)));
            var objects = assembly.GetAllTypes().Where(a => Utils.IsSubclassOf(a, baseObjectType) && !scriptableObjectTypes.Any(t => Utils.IsSubclassOf(a, t)));

            foreach (var o in objects)
            {
                generator.AddClassDeclaration(o);
                Types.Add(o);
            }
        }

        internal override int CodegenRegistrationData(Dictionary<TypeDefinition, NativeName> types, StringBuilder output)
        {
            return 0;
        }

        internal override bool HasData()
        {
            return false;
        }

        internal override void CodegenAdditionalCode(Dictionary<TypeDefinition, NativeName> types, StringBuilder n, string module)
        {
            foreach (var t in ClassRegistrationProcessor.Types)
                if (types.ContainsKey(t))
                    n.AppendFormat("template <> void RegisterUnityClass<{0}>(const char* module);\n", types[t].QualifiedName);

            n.AppendFormat("void RegisterModuleClassesGenerated_{0} ()\n", module);
            n.AppendLine("{");
            foreach (var t in ClassRegistrationProcessor.Types)
                if (types.ContainsKey(t))
                    n.AppendFormat("    RegisterUnityClass<{0}>(\"{1}\");\n", types[t].QualifiedName, module);
            n.AppendLine("}");
        }
    }
}
