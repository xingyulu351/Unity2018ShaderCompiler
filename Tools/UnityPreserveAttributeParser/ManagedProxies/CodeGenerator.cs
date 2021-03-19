using System;
using System.Collections.Generic;
using System.Text;
using Mono.Cecil;

namespace UnityPreserveAttributeParser.ManagedProxies
{
    public abstract class CodeGenerator
    {
        protected readonly string ModuleName;
        protected readonly StringBuilder Builder;
        protected readonly TypeDefinition TypeDefinition;
        protected readonly List<MethodReference> Methods;

        public CodeGenerator(TypeDefinition typeDefinition, string moduleName, List<MethodReference> methods)
        {
            Builder = new StringBuilder();
            TypeDefinition = typeDefinition;
            ModuleName = moduleName;
            Methods = methods;
        }

        protected void GenerateNamespaceHeader()
        {
            Builder.AppendFormat("// {0}\nnamespace Scripting {{ ", TypeDefinition.FullName);

            foreach (var chunk in TypeDefinition.NestedNamespace().Split('.'))
                Builder.AppendFormat("namespace {0} {{ ", chunk);

            Builder.AppendLine();
        }

        protected void GenerateNamespaceFooter()
        {
            foreach (var chunk in TypeDefinition.NestedNamespace().Split('.'))
                Builder.Append("}");

            Builder.AppendLine("}");
        }

        protected string CppNamespace()
        {
            return CppNamespaceFor(TypeDefinition.NestedNamespace());
        }

        public static string CppNamespaceFor(string ns, bool addGlobalNameSpace = true)
        {
            var chunks = new List<string> {addGlobalNameSpace ? " ::Scripting" : "Scripting"};
            chunks.AddRange(ns.Split('.'));
            return String.Join("::", chunks);
        }

        protected string BaseTypeCppFullName()
        {
            var ns = BaseTypeCppNamespace();
            var name = BaseTypeCppName();
            return ns == null ? name : String.Format("{0}::{1}", ns, name);
        }

        protected string BaseTypeCppNamespace()
        {
            if (TypeDefinition.BaseType == null || !Naming.IsUnityEngineType(TypeDefinition.BaseType))
                return null;

            return CppNamespaceFor(TypeDefinition.BaseType.Namespace);
        }

        protected string BaseTypeCppName()
        {
            if (TypeDefinition.BaseType == null)
                return "ScriptingObjectPtr";

            if (!Naming.IsUnityEngineType(TypeDefinition.BaseType))
            {
                if (TypeDefinition.IsValueType || TypeDefinition.BaseType.FullName == typeof(ValueType).FullName)
                    return "ScriptingValueTypePtr";

                return "ScriptingObjectPtr";
            }

            return TypeDefinition.BaseType.CppName();
        }

        public static string ScriptingTypeNameFor(TypeReference typeReference)
        {
            switch (typeReference.MetadataType)
            {
                case MetadataType.Boolean:
                    return "ScriptingBool";
                case MetadataType.SByte:
                    return "SInt8";
                case MetadataType.Byte:
                    return "UInt8";
                case MetadataType.Int16:
                    return "SInt16";
                case MetadataType.UInt16:
                    return "UInt16";
                case MetadataType.Int32:
                    return "SInt32";
                case MetadataType.UInt32:
                    return "UInt32";
                case MetadataType.Int64:
                    return "SInt64";
                case MetadataType.UInt64:
                    return "UInt64";
                case MetadataType.Single:
                    return "float";
                case MetadataType.Double:
                    return "double";
                case MetadataType.String:
                    return "ScriptingStringPtr";
                case MetadataType.Array:
                {
                    var typeDefinition = typeReference.Resolve();
                    if (typeDefinition != null)
                    {
                        var elementType = typeDefinition.GetElementType()?.Resolve();
                        if (elementType != null && elementType.RequiresManagedProxyGeneration())
                            return string.Format("ScriptingArrayOf<{0}::{1}>", CppNamespaceFor(elementType.Namespace), elementType.CppName());
                    }
                    return "ScriptingArrayPtr";
                }
                case MetadataType.IntPtr:
                    return "void*"; // Note: this will not work in il2cpp
                case MetadataType.Class:
                case MetadataType.Object:
                {
                    var typeDefinition = typeReference.Resolve();
                    if (typeDefinition != null)
                    {
                        if (PreserveLogic.ExtractPreserveStateFrom(typeDefinition) != PreserveState.Unused)
                            return string.Format("{0}::{1}", CppNamespaceFor(typeDefinition.Namespace), typeDefinition.CppName());
                    }
                    return "ScriptingObjectPtr";
                }
                case MetadataType.ValueType:
                {
                    var typeDefinition = typeReference.Resolve();
                    if (typeDefinition != null)
                    {
                        if (typeDefinition.IsEnum)
                            return string.Format(
                                "{0} /* {1} */", ScriptingTypeNameFor(typeDefinition.GetUnderlyingEnumType()),
                                typeDefinition.FullName);

                        // Color32 is a struct, and could use the generated struct marshalling.
                        // But it needs special handling due to alignment requirements.
                        if (typeDefinition.Name == "Color32")
                            return "ColorRGBA32";

                        if (typeDefinition.RequiresManagedProxyGeneration())
                            return string.Format("{0}::{1}", CppNamespaceFor(typeDefinition.Namespace), typeDefinition.CppName());
                    }

                    throw new NotSupportedException(
                        string.Format("Unsupported field type {0} {1}", typeReference.FullName, Enum.GetName(typeof(MetadataType), typeReference.MetadataType)));
                }
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
                    throw new NotSupportedException(
                        string.Format("Unsupported field type {0} {1}", typeReference.FullName, Enum.GetName(typeof(MetadataType), typeReference.MetadataType)));

                default:
                    throw new ArgumentOutOfRangeException(
                        String.Format("{0} -> {1}", typeReference.FullName, typeReference.MetadataType));
            }
        }
    }
}
