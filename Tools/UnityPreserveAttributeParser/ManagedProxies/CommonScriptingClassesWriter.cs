using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Mono.Cecil;

namespace UnityPreserveAttributeParser.ManagedProxies
{
    public sealed class CommonScriptingClassesWriter : UnityPreserveAttributeParser.CommonScriptingClassesWriter
    {
        private List<TypeDefinition> _sortedProcessedTypes;
        private List<KeyValuePair<TypeDefinition, PreserveState>> _processedTypes;
        private Dictionary<TypeDefinition, List<MethodReference>> preservedMethods = new Dictionary<TypeDefinition, List<MethodReference>>();
        HashSet<string> headerIncludes = new HashSet<string>();
        HashSet<string> sourceIncludes = new HashSet<string>();
        AssemblyDefinition modularUnityEngine = null;

        public CommonScriptingClassesWriter(string headerPath, string cppPath)
            : base(headerPath, cppPath)
        {
            Header += "#include \"Runtime/Scripting/ScriptingManagedProxySupport.h\"\n";
            Source += "#include \"Runtime/ScriptingBackend/ScriptingApi.h\"\n";
        }

        public List<TypeDefinition> SortedProcessedTypes
        {
            get { return _sortedProcessedTypes; }
        }

        public override void ProcessModule(string module)
        {
            _sortedProcessedTypes = new List<TypeDefinition>();
            _processedTypes = new List<KeyValuePair<TypeDefinition, PreserveState>>();

            base.ProcessModule(module);
        }

        public override void ProcessType(TypeDefinition type, PreserveState state)
        {
            base.ProcessType(type, state);

            if (state == PreserveState.Unused)
                return;

            _processedTypes.Add(new KeyValuePair<TypeDefinition, PreserveState>(type, state));

            foreach (var i in type.Interfaces)
                _processedTypes.Add(new KeyValuePair<TypeDefinition, PreserveState>(i.InterfaceType.Resolve(), state));
        }

        public override void ProcessMethod(MethodDefinition method, PreserveState state)
        {
            base.ProcessMethod(method, state);
            var type = method.DeclaringType;
            _processedTypes.Add(new KeyValuePair<TypeDefinition, PreserveState>(type, state));
            if (!preservedMethods.ContainsKey(type))
                preservedMethods[type] = new List<MethodReference>();
            preservedMethods[type].Add(method);
        }

        public override void EndModule()
        {
            base.EndModule();

            GenerateProxies();
        }

        private void GenerateProxies()
        {
            SortProcessedTypesByInheritance();

            foreach (var processedType in _sortedProcessedTypes)
                GenerateIncludesFor(processedType);
            foreach (var typeMethods in preservedMethods.Values)
            {
                foreach (var method in typeMethods)
                {
                    foreach (var param in method.Parameters)
                    {
                        GenerateIncludesFor(param.ParameterType.Resolve());
                    }
                }
            }

            foreach (var include in headerIncludes)
                Header += "#include \"" + include + "\"\n";
            foreach (var include in sourceIncludes)
                Source += "#include \"" + include + "\"\n";

            foreach (var processedType in _sortedProcessedTypes)
                GenerateForwardDefinitionFor(processedType);

            foreach (var processedType in _sortedProcessedTypes)
                GenerateProxyFor(processedType);
        }

        private void SortProcessedTypesByInheritance()
        {
            var unique = new HashSet<TypeDefinition>();

            foreach (var processedType in _processedTypes)
            {
                PushTypeHierarchy(processedType.Key, unique);
            }
        }

        private void PushTypeHierarchy(TypeDefinition typeDefinition, HashSet<TypeDefinition> unique)
        {
            // if we have fields we generate proxies for, make sure that the field types are pushed first
            if (typeDefinition.RequiresManagedProxyGeneration())
            {
                foreach (var field in typeDefinition.Fields)
                {
                    var fieldType = field.FieldType;
                    if (fieldType.IsArray)
                        fieldType = fieldType.Resolve().GetElementType();
                    if (!field.IsStatic)
                    {
                        foreach (var processedType in _processedTypes)
                        {
                            if (processedType.Key == fieldType && !unique.Contains(fieldType))
                                PushTypeHierarchy(fieldType.Resolve(), unique);
                        }
                    }
                }
            }

            if (typeDefinition.BaseType == null)
                return;

            var baseType = typeDefinition.BaseType.Resolve();
            if (baseType == null)
                return;

            if (Naming.IsUnityEngineType(baseType))
            {
                if (!unique.Contains(baseType))
                    PushTypeHierarchy(baseType, unique);
            }

            if (unique.Add(typeDefinition))
                _sortedProcessedTypes.Add(typeDefinition);
        }

        bool IsTypeSupported(TypeDefinition typeDefinition)
        {
            var type = typeDefinition;
            do
            {
                type = type.BaseType?.Resolve();
            }
            while (type != null);
            return true;
        }

        private void GenerateForwardDefinitionFor(TypeDefinition typeDefinition)
        {
            if (!IsTypeSupported(typeDefinition))
                return;

            if (typeDefinition.Module.Assembly != Assembly)
                return;

            if (typeDefinition.NativeProxyType() != null)
            {
                if (typeDefinition.NativeProxyTypeDeclaration() != null)
                {
                    Header += typeDefinition.NativeProxyTypeDeclaration();
                    Header += "\n";
                }
                else
                {
                    var nativeType = typeDefinition.NativeProxyType();
                    var nameSpaces = nativeType.Split(new[] { "::" }, System.StringSplitOptions.RemoveEmptyEntries);
                    for (int i = 0; i < nameSpaces.Length - 1; i++)
                        Header += string.Format("namespace {0} {{ ", nameSpaces[i]);
                    Header += $"class {nameSpaces[nameSpaces.Length - 1]};";
                    for (int i = 0; i < nameSpaces.Length - 1; i++)
                        Header += "}";
                    Header += "\n";
                }
            }

            Header += "\n";
            Header += new HeaderGenerator(typeDefinition, ModuleName, null).GenerateForwardDefinition();
            Header += "\n";
        }

        string ResolveModuleForType(TypeDefinition typeDefinition)
        {
            var assemblyName = typeDefinition.Module.Assembly.Name.Name;
            if (assemblyName.StartsWith("UnityEngine.") && assemblyName.EndsWith("Module"))
            {
                var moduleName = assemblyName.Substring("UnityEngine.".Length, assemblyName.Length - "UnityEngine.Module".Length);
                return moduleName;
            }

            if (assemblyName == "UnityEngine")
            {
                // if we get a type reference to the monolithic UnityEngine (which we get when processing Editor module,
                // which is referencing the monolithic engine), look up the type in the modular engine instead to get the
                // module name.
                if (modularUnityEngine == null)
                {
                    var monolithicUnityEngine = typeDefinition.Module;
                    var modularEngineDir = Path.Combine(Path.GetDirectoryName(monolithicUnityEngine.FileName), "UnityEngine");
                    var modularEngineFileName = Path.Combine(modularEngineDir, "UnityEngine.dll");
                    var resolver = new DefaultAssemblyResolver();
                    resolver.AddSearchDirectory(modularEngineDir);
                    var readerParameters = new ReaderParameters
                    {
                        AssemblyResolver = resolver,
                    };

                    modularUnityEngine = AssemblyDefinition.ReadAssembly(modularEngineFileName, readerParameters);
                }

                var modularType = modularUnityEngine.MainModule.ExportedTypes.FirstOrDefault(x => x.FullName == typeDefinition.FullName);
                if (modularType != null)
                    return ResolveModuleForType(modularType.Resolve());
            }
            return null;
        }

        private void GenerateIncludesFor(TypeDefinition typeDefinition)
        {
            if (!IsTypeSupported(typeDefinition))
                return;

            if (typeDefinition.Module.Assembly != Assembly)
            {
                var module = ResolveModuleForType(typeDefinition);
                if (module != null)
                    headerIncludes.Add(string.Format("{0}ScriptingClasses.h", module));
            }

            var headers = typeDefinition.NativeProxyTypeHeaders();
            foreach (var header in headers)
                sourceIncludes.Add(header);
        }

        private void GenerateProxyFor(TypeDefinition typeDefinition)
        {
            if (!IsTypeSupported(typeDefinition))
                return;

            if (typeDefinition.Module.Assembly != Assembly)
                return;

            var methods = preservedMethods.ContainsKey(typeDefinition) ? preservedMethods[typeDefinition] : null;

            Header += "\n";
            Header += new HeaderGenerator(typeDefinition, ModuleName, methods).Generate();
            Header += "\n";

            Source += "\n";
            Source += new SourceGenerator(typeDefinition, ModuleName, methods).Generate();
            Source += "\n";
        }
    }
}
