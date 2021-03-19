using System;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using Mono.Cecil;

namespace UnityPreserveAttributeParser.ManagedProxies
{
    internal static class Extensions
    {
        public static bool CanHaveNativeFieldAccessor(this FieldDefinition fieldDefinition)
        {
            if (fieldDefinition.IsStatic)
                return false;

            if (fieldDefinition.IsCompilerControlled)
                return false;

            if (fieldDefinition.CustomAttributes.Any(a => a.AttributeType.FullName == typeof(CompilerGeneratedAttribute).FullName))
                return false;

            return true;
        }

        public static bool IsGenerateManagedProxyAttribute(this CustomAttribute customAttribute)
        {
            return customAttribute.AttributeType.Name == "GenerateManagedProxyAttribute";
        }

        public static bool IsNativeClassAttribute(this CustomAttribute customAttribute)
        {
            return customAttribute.AttributeType.Name == "NativeClassAttribute";
        }

        public static bool RequiresManagedProxyGeneration(this TypeDefinition typeDefinition)
        {
            var attr = PreserveLogic.GetPreserveUnityEngineAttribute(typeDefinition);
            if (attr == null)
                return false;

            if (attr.HasFields)
            {
                foreach (var namedArgument in attr.Fields)
                {
                    if (namedArgument.Name == "GenerateProxy")
                        return (bool)namedArgument.Argument.Value;
                }
            }

            if (attr.HasProperties)
            {
                foreach (var namedArgument in attr.Properties)
                {
                    if (namedArgument.Name == "GenerateProxy")
                        return (bool)namedArgument.Argument.Value;
                }
            }

            return false;
        }

        public static string NativeFieldName(this FieldReference field)
        {
            var attribute = field.Resolve().CustomAttributes.FirstOrDefault(a => a.AttributeType.Name == "NativeNameAttribute");
            if (attribute != null)
                return attribute.ConstructorArguments[0].Value as string;

            return field.Name;
        }

        public static string FieldName(this FieldReference field)
        {
            var name = field.Name;
            if (name.StartsWith("m_"))
                name = name.Substring(2);
            return name;
        }

        public static bool NativeAsStruct(this TypeDefinition typeDefinition)
        {
            return typeDefinition.CustomAttributes.FirstOrDefault(a => a.AttributeType.Name.Contains("NativeAsStruct")) != null;
        }

        public static bool NameIsAmbiguous(this MethodDefinition method)
        {
            return method.DeclaringType.Methods.Where(m => m.Name == method.Name).Count() > 1;
        }

        public static string CppName(this TypeReference type)
        {
            return $"{type.Name.Replace('`','_')}Proxy";
        }

        public static string CppFullName(this TypeReference type)
        {
            return $"{type.FullName.Replace('`', '_').Replace(".", "::")}Proxy";
        }

        public static string NestedNamespace(this TypeReference type)
        {
            if (type.IsNested)
                return $"{type.DeclaringType.NestedNamespace()}.{type.DeclaringType.Name}";
            else
                return type.Namespace;
        }

        public static string WarningMessageForAmbiguousName(this MethodDefinition method)
        {
            return $"// WARNING: Cannot generate wrappers for {method.Name}, because {method.DeclaringType} has multiple signatures of that name, so it would be ambiguous.\n";
        }

        public static bool IsUnityEngineObject(this TypeDefinition typeDefinition)
        {
            var type = typeDefinition;
            while (type != null)
            {
                if (type.Namespace == "UnityEngine" && type.Name == "Object")
                    return true;
                if (type.Namespace == "UnityEngine" && type.Name == "MonoBehaviour")
                    break;
                if (type.Namespace == "UnityEngine" && type.Name == "ScriptableObject")
                    break;
                type = type.BaseType?.Resolve();
            }

            return false;
        }

        public static string NativeProxyType(this TypeDefinition typeDefinition)
        {
            if (typeDefinition.IsUnityEngineObject() || typeDefinition.RequiresManagedProxyGeneration())
            {
                var nativeClassAttribute = typeDefinition.CustomAttributes.FirstOrDefault(a => a.IsNativeClassAttribute());
                if (nativeClassAttribute != null)
                {
                    if (nativeClassAttribute.ConstructorArguments.Any())
                        return nativeClassAttribute.ConstructorArguments[0].Value as string;
                }
                else if (typeDefinition.IsUnityEngineObject())
                    return typeDefinition.Name;
            }

            return null;
        }

        public static string NativeProxyTypeDeclaration(this TypeDefinition typeDefinition)
        {
            if (typeDefinition.IsUnityEngineObject() || typeDefinition.RequiresManagedProxyGeneration())
            {
                var nativeClassAttribute = typeDefinition.CustomAttributes.FirstOrDefault(a => a.IsNativeClassAttribute());
                if (nativeClassAttribute != null)
                {
                    if (nativeClassAttribute.ConstructorArguments.Count() > 1)
                        return nativeClassAttribute.ConstructorArguments[1].Value as string;
                }
            }

            return null;
        }

        public static IEnumerable<string> NativeProxyTypeHeaders(this TypeDefinition typeDefinition)
        {
            if (!typeDefinition.RequiresManagedProxyGeneration())
                return new string[0];

            return typeDefinition.CustomAttributes.Where(a => a.AttributeType.Name.Contains("NativeHeader")).Select(a => a.ConstructorArguments[0].Value as string).OrderBy(x => x);
        }

        public static bool IsBlittable(this TypeReference type, NativeType? nativeType = null, bool useUnicodeCharset = false)
        {
            if (type.IsGenericInstance)
                return false;

            if (type is TypeSpecification)
                return false;

            if (type is ArrayType)
                return false;

            return IsBlittable(type.Resolve(), nativeType, useUnicodeCharset);
        }

        public static bool IsBlittable(this TypeDefinition type, NativeType? nativeType = null, bool useUnicodeCharset = false)
        {
            useUnicodeCharset |= (type.Attributes & TypeAttributes.UnicodeClass) != 0;

            if (type.HasGenericParameters || !type.IsValueType)
                return false;

            if (type.IsEnum) // Enums have AutoLayout, but they can still be blittable
                return IsPrimitiveBlittable(type.GetUnderlyingEnumType().Resolve(), nativeType, useUnicodeCharset);

            // structs and formatted classes are blittable if their fields are blittable
            return (type.IsSequentialLayout || type.IsExplicitLayout) && AreFieldsBlittable(type, nativeType, useUnicodeCharset);
        }

        public static TypeReference GetUnderlyingEnumType(this TypeReference type)
        {
            var typeDefinition = type.Resolve();
            if (typeDefinition == null)
                throw new Exception("Failed to resolve type reference");

            if (!typeDefinition.IsEnum)
                throw new ArgumentException("Attempting to retrieve underlying enum type for non-enum type.", "type");

            return typeDefinition.Fields.Single(f => !f.IsStatic && f.Name == "value__").FieldType;
        }

        private static bool AreFieldsBlittable(TypeDefinition typeDef, NativeType? nativeType, bool useUnicodeCharset)
        {
            // Primitives have themselves as fields, so not checking for that means infinite recursion
            if (typeDef.IsPrimitive)
                return IsPrimitiveBlittable(typeDef, nativeType, useUnicodeCharset);

            return
                typeDef.Fields.All(
                field => field.IsStatic || IsBlittable(field.FieldType, GetFieldNativeType(field), useUnicodeCharset));
        }

        private static NativeType? GetFieldNativeType(FieldDefinition field)
        {
            if (field.MarshalInfo == null)
                return null;

            return field.MarshalInfo.NativeType;
        }

        private static bool IsPrimitiveBlittable(TypeDefinition type, NativeType? nativeType, bool useUnicodeCharset)
        {
            if (nativeType == null)
            {
                if (type.MetadataType == MetadataType.Char)
                    return useUnicodeCharset;

                return type.MetadataType != MetadataType.Boolean;
            }

            switch (type.MetadataType)
            {
                case MetadataType.Boolean:
                case MetadataType.Byte:
                case MetadataType.SByte:
                    return nativeType == NativeType.U1 || nativeType == NativeType.I1;

                case MetadataType.UInt16:
                case MetadataType.Int16:
                case MetadataType.Char:
                    return nativeType == NativeType.U2 || nativeType == NativeType.I2;

                case MetadataType.UInt32:
                case MetadataType.Int32:
                    return nativeType == NativeType.U4 || nativeType == NativeType.I4;

                case MetadataType.UInt64:
                case MetadataType.Int64:
                    return nativeType == NativeType.U8 || nativeType == NativeType.I8;

                case MetadataType.IntPtr:
                case MetadataType.UIntPtr:
                    return nativeType == NativeType.UInt || nativeType == NativeType.Int;

                case MetadataType.Single:
                    return nativeType == NativeType.R4;

                case MetadataType.Double:
                    return nativeType == NativeType.R8;

                default:
                    throw new ArgumentException(string.Format("{0} is not a primitive!", type.FullName));
            }
        }
    }
}
