using Mono.Cecil;
using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Unity.RegistrationGenerator
{
    struct NativeName
    {
        public static NativeName Parse(string fullname)
        {
            var toks = fullname.Split(new string[] { "::" }, StringSplitOptions.None);
            var result = new NativeName();
            result.Symbol = toks.Last();
            result.Namespaces = toks.Take(toks.Length - 1).ToArray();
            result.IsDeduced = false;
            return result;
        }

        public string QualifiedName
        {
            get
            {
                return string.Join("::", Namespaces.Concat(new[] {Symbol}));
            }
        }

        public string[] Namespaces { get; private set; }
        public string Symbol { get; private set; }
        public bool IsDeduced { get; set; }
    }

    public abstract class CodegenProcessor
    {
        public string GetName()
        {
            const string kProcessor = "Processor";
            var n = this.GetType().Name;
            return (n.EndsWith(kProcessor) ? n.Remove(n.Length - kProcessor.Length) : n) + "Spec";
        }

        internal abstract void Process(ICppCodeGeneration generator);
        internal abstract int CodegenRegistrationData(Dictionary<TypeDefinition, NativeName> types, StringBuilder output);
        internal abstract bool HasData();
        internal virtual void CodegenAdditionalCode(Dictionary<TypeDefinition, NativeName> types, StringBuilder output, string module) {}
    }

    public interface ICppCodeGeneration
    {
        AssemblyDefinition GetAssembly();
        void AddProcessor(CodegenProcessor p);
        void AddInclude(string path);
        void AddClassDeclaration(TypeDefinition type);
        void AddStructDeclaration(TypeDefinition type);
    }

    public class CppCodeGeneration : ICppCodeGeneration
    {
        const string kCppCodeTemplate = @"
// AUTO GENERATED - ANY MANUAL MODIFICATIONS WILL BE LOST
// Also see <unity repo>/Tools/RegistrationGenerator/

#include ""UnityPrefix.h""
#include ""Runtime/Modules/ModuleRegistrationTypes.h""
#include ""Runtime/BaseClasses/WeakTypeOf.h""

template<typename T> void RegisterUnityClass(const char* module);

// Includes
{0}

// Pre-declare structs and classes
// E.g. class FlareLayer
{1}

// Data
// E.g.
// static const RequireComponent g_RequireComponentFooModule[] = {{ {{WeakTypeOf<FlareLayer>(), WeakTypeOf<Camera>()}} }};
// static const size_t g_RequireComponentFooModuleLength = 1;
{2}

static ModuleSpec g_{3}ModuleSpec;

// Access point to registration for core
const ModuleSpec& Get{3}ModuleSpec()
{{
{4}
    return g_{3}ModuleSpec;
}}

{5}
";

        AssemblyDefinition m_Assembly;
        AssemblyDefinition m_CoreAssembly;
        AssemblyDefinition m_SharedInternalsAssembly;
        string m_ModulesName;
        Dictionary<TypeDefinition, NativeName> m_ManagedToNativeType;
        List<CodegenProcessor> m_Processors = new List<CodegenProcessor>();
        internal HashSet<string> Includes { get; private set; }
        internal HashSet<TypeDefinition> ClassDeclarations { get; private set; }
        internal HashSet<TypeDefinition> StructDeclarations { get; private set; }

        public CppCodeGeneration(string moduleName, AssemblyDefinition assembly, AssemblyDefinition coreAssembly = null, AssemblyDefinition sharedInternalsAssembly = null)
        {
            m_Assembly = assembly;
            m_CoreAssembly = coreAssembly;
            m_SharedInternalsAssembly = sharedInternalsAssembly;
            m_ModulesName = moduleName;
            m_ManagedToNativeType = ExtractManagedToNativeTypeMapping(assembly);
            Includes = new HashSet<string>();
            ClassDeclarations = new HashSet<TypeDefinition>();
            StructDeclarations = new HashSet<TypeDefinition>();
        }

        public void AddProcessor(CodegenProcessor p)
        {
            m_Processors.Add(p);
        }

        public void AddInclude(string path)
        {
            Includes.Add(path);
        }

        public void AddClassDeclaration(TypeDefinition type)
        {
            ClassDeclarations.Add(type);

            AddTypeToMapping(m_ManagedToNativeType, type);
        }

        public void AddStructDeclaration(TypeDefinition type)
        {
            StructDeclarations.Add(type);

            AddTypeToMapping(m_ManagedToNativeType, type);
        }

        void AddTypeToMapping(Dictionary<TypeDefinition, NativeName> mapping, TypeDefinition type)
        {
            if (type == null)
                return;
            if (mapping.ContainsKey(type))
                return;
            var attributeAssembly = m_SharedInternalsAssembly ?? m_Assembly;
            var nativeClassAttributeType = attributeAssembly.MainModule.Types.FirstOrDefault(a => a.FullName == typeof(UnityEngine.NativeClassAttribute).FullName);
            if (nativeClassAttributeType != null)
            {
                var attrs = type.CustomAttributes.Where(ca => ca.AttributeType.FullName == nativeClassAttributeType.FullName).ToArray();
                if (attrs.Length != 0)
                {
                    var attr = attrs[0];
                    var nativeClassName = (string)attr.ConstructorArguments[0].Value;
                    if (nativeClassName != null)
                        mapping.Add(type, NativeName.Parse(nativeClassName));
                    return;
                }
            }

            // Default empty string. That way the engine can decide to do last minute lookup
            // in the mono runtime it wants to.
            var nt = NativeName.Parse(type.Name);
            nt.IsDeduced = true;
            mapping.Add(type, nt);
        }

        public AssemblyDefinition CoreAssembly
        {
            get
            {
                return m_CoreAssembly;
            }
        }

        Dictionary<TypeDefinition, NativeName> ExtractManagedToNativeTypeMapping(AssemblyDefinition assembly)
        {
            var result = new Dictionary<TypeDefinition, NativeName>();

            var attributeAssembly = m_SharedInternalsAssembly ?? m_Assembly;
            var nativeClassAttributeType = attributeAssembly.MainModule.Types.FirstOrDefault(a => a.FullName == typeof(UnityEngine.NativeClassAttribute).FullName);
            if (nativeClassAttributeType != null)
            {
                // Here locate types by name because we need the exact type as defined in the assembly in concern and
                // not just a faked type with the same name since that would not work for querying the assemble on the
                // type afterwards.
                foreach (var type in assembly.GetAllTypes())
                    AddTypeToMapping(result, type);
            }
            return result;
        }

        public AssemblyDefinition GetAssembly()
        {
            return m_Assembly;
        }

        public string Generate()
        {
            foreach (var processor in m_Processors)
                processor.Process(this);

            return string.Format(kCppCodeTemplate, GenerateIncludes(), GenerateDeclarations(), GenerateData(), m_ModulesName, GenerateAccess(), GenerateAdditionalCode());
        }

        string GenerateIncludes()
        {
            var r = new StringBuilder();
            foreach (var p in Includes)
                r.AppendFormat("#include \"{0}\"\n", p);
            return r.ToString();
        }

        string GenerateDeclarations()
        {
            var r = new StringBuilder();

            foreach (var d in ClassDeclarations)
                r.Append(GenerateAggregateDeclaration(d, "class"));

            r.Append("\n");

            foreach (var d in StructDeclarations)
                r.Append(GenerateAggregateDeclaration(d, "struct"));

            return r.ToString();
        }

        string GenerateAggregateDeclaration(TypeDefinition type, string aggregateType)
        {
            // Generate lines like:
            //
            // namespace UI { class Canvas; }
            // class FlareLayer;

            var b = new StringBuilder();

            if (!m_ManagedToNativeType.ContainsKey(type))
                return "";
            var nativeName = m_ManagedToNativeType[type];

            foreach (var n in nativeName.Namespaces)
                b.AppendFormat("namespace {0} {{ ", n);

            b.AppendFormat("{0} {1};", aggregateType, nativeName.Symbol);

            foreach (var n in nativeName.Namespaces)
                b.Append(" }");

            if (nativeName.Namespaces.Length != 0)
                b.Append(";");
            b.Append("\n");
            return b.ToString();
        }

        string GenerateAdditionalCode()
        {
            var n = new StringBuilder();
            foreach (var p in m_Processors)
                p.CodegenAdditionalCode(m_ManagedToNativeType, n, m_ModulesName);

            return n.ToString();
        }

        string GenerateData()
        {
            const string kFormat = "static const {0} g_{0}{1}[] = {{\n";

            var n = new StringBuilder();
            foreach (var p in m_Processors)
            {
                if (!p.HasData())
                    continue;

                n.AppendFormat(kFormat, p.GetName(), m_ModulesName);
                int len = p.CodegenRegistrationData(m_ManagedToNativeType, n);
                if (len == 0)
                {
                    // MSVC does not like zero-length arrays. Set it to NULL instead.
                    n = new StringBuilder();
                    n.AppendFormat("static const {0}* g_{0}{1} = NULL;\n", p.GetName(), m_ModulesName);
                }
                else
                    n.Append("};\n\n");
                n.AppendFormat("static const size_t g_{0}{1}Length = {2};\n\n", p.GetName(), m_ModulesName, len);
            }

            return n.ToString();
        }

        object GenerateAccess()
        {
            const string kFormat = "    g_{2}ModuleSpec.{0} = g_{1}{2};\n";
            const string kFormat2 = "    g_{2}ModuleSpec.{0}Length = g_{1}{2}Length;\n";

            var n = new StringBuilder();
            foreach (var p in m_Processors)
            {
                if (!p.HasData())
                    continue;

                var name = p.GetName();
                var registrationName = name.Substring(0, 1).ToLower() + name.Substring(1);
                n.AppendFormat(kFormat, registrationName, name, m_ModulesName);
                n.AppendFormat(kFormat2, registrationName, name, m_ModulesName);
            }
            return n.ToString();
        }
    }
}
