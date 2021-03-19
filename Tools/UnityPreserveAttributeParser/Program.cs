using System;
using System.IO;
using System.Collections.Generic;
using System.Globalization;
using System.Text.RegularExpressions;
using System.Threading;
using Mono.Cecil;
using System.Linq;
using System.Reflection;
using Mono.Cecil.Cil;
using Mono.Cecil.Mdb;
using Mono.Options;
using UnityPreserveAttributeParser;
using UnityPreserveAttributeParser.ManagedProxies;

public class Program
{
    public const string GeneratedMessageSummary = "GENERATED FILE. DO NOT HAND EDIT. Use \"./jam GenerateNativeDependenciesForManagedCode\" to rebuild!";
    public const string GeneratedMessageArtifacts = "THIS FILE IS GENERATED DURING BUILD. DO NOT HAND EDIT. See https://confluence.hq.unity3d.com/display/DEV/How+to+Use+Scripting+Classes+and+Methods+from+Native+Code for more info.";

    public static void WriteAllTextToFile(string path, string contents)
    {
        var dir = Path.GetDirectoryName(path);
        Directory.CreateDirectory(dir);
        File.WriteAllText(path, contents);
    }

    public class AssemblyProcessor
    {
        public SortedDictionary<string, SortedDictionary<string, HashSet<MethodDefinition>>> CollectedTypes;
        public List<IStrippingInformationOutputHandler> Handlers = new List<IStrippingInformationOutputHandler>();
        public ReaderParameters ReaderParameters;

        private string IsTypeInFilter(TypeDefinition type)
        {
            var asmName = type.Module.Assembly.Name.Name;
            var matches = Regex.Match(asmName, @"UnityEngine\.(\w*)Module");
            if (matches.Success)
                return matches.Groups[1].Value;

            return "Editor";
        }

        private void CollectType(TypeDefinition type)
        {
            var methods = type.Methods.Where(t => PreserveLogic.ExtractPreserveStateFrom(t) != PreserveState.Unused);
            var preserveType = PreserveLogic.ExtractPreserveStateFrom(type);

            if (methods.Any() || preserveType != PreserveState.Unused)
            {
                var module = IsTypeInFilter(type);
                if (module == null)
                    return;

                if (!CollectedTypes.ContainsKey(module))
                    CollectedTypes[module] = new SortedDictionary<string, HashSet<MethodDefinition>>();

                var typeString = type.ToString();
                if (!CollectedTypes[module].ContainsKey(typeString))
                    CollectedTypes[module][typeString] = new HashSet<MethodDefinition>();

                foreach (var m in methods)
                    CollectedTypes[module][typeString].Add(m);
            }

            if (type.HasNestedTypes)
            {
                foreach (TypeDefinition nestedType in type.NestedTypes)
                    CollectType(nestedType);
            }
        }

        public void Process(IEnumerable<AssemblyDefinition> assemblies)
        {
            var forwardedAssemblies = new List<string>();
            // Find all assemblies referenced by type forwarders, and include those in the list of assemblies to process
            foreach (var assembly in assemblies)
            {
                ModuleDefinition asmModule = assembly.MainModule;
                foreach (var type in asmModule.ExportedTypes)
                {
                    var tr = type.Resolve();
                    if (!assemblies.Select(x => x.MainModule).Contains(tr.Module) && !forwardedAssemblies.Contains(tr.Module.FileName))
                        forwardedAssemblies.Add(tr.Module.FileName);
                }
            }

            foreach (var assembly in forwardedAssemblies)
                assemblies = assemblies.Concat(new[] {AssemblyDefinition.ReadAssembly(assembly, ReaderParameters)});

            foreach (var assembly in assemblies.OrderBy(s => s.ToString()))
            {
                CollectedTypes = new SortedDictionary<string, SortedDictionary<string, HashSet<MethodDefinition>>>();
                Handlers.ForEach(h => h.ProcessAssembly(assembly));
                ModuleDefinition asmModule = assembly.MainModule;
                foreach (TypeDefinition type in asmModule.Types)
                    CollectType(type);

                foreach (var module in CollectedTypes)
                {
                    Handlers.ForEach(h => h.ProcessModule(module.Key));
                    foreach (var t in module.Value)
                    {
                        var type = asmModule.GetType(t.Key);
                        var preserveType = PreserveLogic.ExtractPreserveStateFrom(type);

                        // BaseType validation only happens if the type is explicitly marked
                        // as required/used.
                        if (preserveType != PreserveState.Unused)
                            ValidateBaseTypeOf(type);

                        ValidateGenerateManagedProxiesAttribute(type);

                        Handlers.ForEach(h => h.ProcessType(type, preserveType));
                        foreach (var m in t.Value.OrderBy(s => s.ToString()))
                            Handlers.ForEach(h => h.ProcessMethod(m, PreserveLogic.ExtractPreserveStateFrom(m)));

                        Handlers.ForEach(h => h.EndType());
                    }
                    Handlers.ForEach(h => h.EndModule());
                }
                Handlers.ForEach(h => h.EndAssembly());
            }
            Handlers.ForEach(h => h.End());
        }

        private static void ValidateGenerateManagedProxiesAttribute(TypeDefinition typeDefinition)
        {
            if (!typeDefinition.RequiresManagedProxyGeneration())
                return;

            if (typeDefinition.HasGenericParameters)
                throw new NotSupportedException(
                    string.Format("GenerateManagedProxies attribute is not supported on nested types and generic types yet. Please remove it from {0}.",
                        typeDefinition.FullName));

            if (!typeDefinition.IsSequentialLayout)
                throw new NotSupportedException(
                    string.Format("GenerateManagedProxies can only be applied to types with Sequential LayoutKind. Please fix {0}.",
                        typeDefinition.FullName));

            var invalidFields = typeDefinition.Fields.Where(fieldDefinition => !fieldDefinition.IsStatic && !CanGenerateFieldAccessorFor(fieldDefinition)).ToArray();

            if (invalidFields.Length <= 0)
                return;

            foreach (var invalidField in invalidFields)
            {
                Console.WriteLine(
                    "[ERROR] {0}.{1} is of unsupported type {2} ({3}).",
                    typeDefinition.FullName,
                    invalidField.Name,
                    invalidField.FieldType.FullName,
                    Enum.GetName(typeof(MetadataType), invalidField.FieldType.MetadataType));
            }

            throw new NotSupportedException(
                string.Format(
                    "A managed proxy for {0} cannot be generated. Check the console for details regarding what's blocking the generation.",
                    typeDefinition.FullName));
        }

        private static bool CanGenerateFieldAccessorFor(FieldDefinition fieldDefinition)
        {
            switch (fieldDefinition.FieldType.MetadataType)
            {
                case MetadataType.Void:
                case MetadataType.Char:
                case MetadataType.Pointer:
                case MetadataType.ByReference:
                case MetadataType.Var:
                case MetadataType.GenericInstance:
                case MetadataType.TypedByReference:
                case MetadataType.UIntPtr:
                case MetadataType.FunctionPointer:
                case MetadataType.MVar:
                case MetadataType.RequiredModifier:
                case MetadataType.OptionalModifier:
                case MetadataType.Sentinel:
                case MetadataType.Pinned:
                    return false;

                case MetadataType.ValueType:
                {
                    var fieldTypeDefinition = fieldDefinition.FieldType.Resolve();
                    // Color32 is a struct, and could use the generated struct marshalling.
                    // But it needs special handling due to alignment requirements.
                    return (fieldTypeDefinition != null && (fieldTypeDefinition.IsEnum || fieldTypeDefinition.RequiresManagedProxyGeneration() || fieldTypeDefinition.Name == "Color32"));
                }

                case MetadataType.Boolean:
                case MetadataType.SByte:
                case MetadataType.Byte:
                case MetadataType.Int16:
                case MetadataType.UInt16:
                case MetadataType.Int32:
                case MetadataType.UInt32:
                case MetadataType.Int64:
                case MetadataType.UInt64:
                case MetadataType.Single:
                case MetadataType.Double:
                case MetadataType.String:
                case MetadataType.Class:
                case MetadataType.Array:
                case MetadataType.IntPtr:
                case MetadataType.Object:
                    return true;

                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        private static void ValidateBaseTypeOf(TypeDefinition type)
        {
            if (type.BaseType == null)
                return;

            var baseType = type.BaseType.Resolve();
            if (baseType == null)
                return;

            while (baseType.IsNested)
                baseType = baseType.DeclaringType;

            if (!Naming.IsUnityEngineType(baseType))
                return;

            if (PreserveLogic.ExtractPreserveStateFrom(baseType) == PreserveState.Unused)
                throw new NotSupportedException(
                    String.Format("{0} must be explicitly marked with Preserve/UsedByNativeCode, as long as its parent type {1} is.",
                        baseType.FullName, type.FullName));

            ValidateBaseTypeOf(baseType);
        }
    }

    public static int Main(string[] args)
    {
        Thread.CurrentThread.CurrentCulture = new CultureInfo("en-us");

        var searchPath = new HashSet<string>();
        string xmlPath = null;
        string headerPath = null;
        string cppPath = null;
        string summaryPath = null;
        string checksummary = null;

        var generateManagedProxies = true;
        var appPath = Assembly.GetExecutingAssembly().Location;
        Directory.SetCurrentDirectory(Path.Combine(Path.GetDirectoryName(appPath), "../../.."));

        var processor = new AssemblyProcessor();
        var p = new OptionSet() {new ResponseFileSource()}
            .Add("xmloutput=", "XML output path.", s => xmlPath = s)
            .Add("headeroutput=", "Header output path.", s => headerPath = s)
            .Add("cppoutput=", "Cpp output path.", s => cppPath = s)
            .Add("summaryoutput=", "Summary output path.", s => summaryPath = s)
            .Add("checksummary=", "Verify summary.", s => checksummary = s)
            .Add("searchpath=", "Paths for finding dependent dlls", s => searchPath.Add(s))
            .Add("noproxies", "Disables managed proxies generation.", n => generateManagedProxies = (n == null));

        var remaining = p.Parse(args);

        if (remaining.Count < 1)
        {
            Console.WriteLine("usage: UnityPreserveAttributeParser.exe [assembly]");
            return 1;
        }

        var resolver = new DefaultAssemblyResolver();

        foreach (var s in searchPath)
            resolver.AddSearchDirectory(s);

        processor.ReaderParameters = new ReaderParameters
        {
            AssemblyResolver = resolver,
            ReadSymbols = true,
            SymbolReaderProvider = new MdbReaderProvider()
        };
        var assemblies = remaining.Select(fileName => AssemblyDefinition.ReadAssembly(fileName, processor.ReaderParameters));

        if (generateManagedProxies)
        {
            if (summaryPath != null)
                processor.Handlers.Add(new UnityPreserveAttributeParser.ManagedProxies.SummaryWriter(summaryPath, checksummary));
            if (xmlPath != null)
                processor.Handlers.Add(new UnityPreserveAttributeParser.XmlOutputWriter(xmlPath));
            if (cppPath != null || headerPath != null)
                processor.Handlers.Add(new UnityPreserveAttributeParser.ManagedProxies.CommonScriptingClassesWriter(headerPath, cppPath));
        }
        else
        {
            if (summaryPath != null)
                processor.Handlers.Add(new UnityPreserveAttributeParser.SummaryWriter(summaryPath, checksummary));
            if (xmlPath != null)
                processor.Handlers.Add(new UnityPreserveAttributeParser.XmlOutputWriter(xmlPath));
            if (cppPath != null || headerPath != null)
                processor.Handlers.Add(new UnityPreserveAttributeParser.CommonScriptingClassesWriter(headerPath, cppPath));
        }

        processor.Process(assemblies);

        return 0;
    }
}
