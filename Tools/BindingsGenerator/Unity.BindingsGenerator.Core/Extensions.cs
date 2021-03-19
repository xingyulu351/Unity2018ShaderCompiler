using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Mono.Cecil;
using UnityEngine.Bindings;
using UnityEngine.Scripting;
using ICustomAttributeProvider = Mono.Cecil.ICustomAttributeProvider;

namespace Unity.BindingsGenerator.Core
{
    public static class Extensions
    {
        internal static IEnumerable<T> GetAttributeInstances<T>(this ICustomAttributeProvider customAttributeProvider, bool inherit = true)
        {
            return customAttributeProvider.CustomAttributes.GetAttributeInstances(typeof(T), inherit).Select(InstantiateBindingsAttribute<T>);
        }

        internal static T GetAttributeInstance<T>(this ICustomAttributeProvider customAttributeProvider, bool inherit = true)
        {
            return customAttributeProvider.GetAttributeInstances<T>(inherit).FirstOrDefault();
        }

        internal static bool HasAttribute<T>(this ICustomAttributeProvider customAttributeProvider, bool inherit = true)
        {
            if (!inherit)
                return customAttributeProvider.CustomAttributes.Any(a => a.AttributeType.FullName == typeof(T).FullName);

            return customAttributeProvider.GetAttributeInstances<T>(inherit).Any();
        }

        internal static bool TryGetAttributeInstance<T>(this ICustomAttributeProvider customAttributeProvider, out T attribute, bool inherit = true) where T : IBindingsAttribute
        {
            attribute = customAttributeProvider.GetAttributeInstance<T>(inherit);
            return attribute != null;
        }

        internal static IEnumerable<CustomAttribute> GetAttributeInstances(this IEnumerable<CustomAttribute> attributes, Type type, bool inherit)
        {
            return inherit
                ? GetBindingsAttributesInheritingFrom(attributes, type)
                : attributes.Where(a => a.AttributeType.FullName == type.FullName);
        }

        static IEnumerable<CustomAttribute> GetBindingsAttributesInheritingFrom(IEnumerable<CustomAttribute> attributes, Type checkType)
        {
            foreach (var customAttribute in attributes)
            {
                var type = Type.GetType(customAttribute.AttributeType.FullName);
                if (type != null && checkType.IsAssignableFrom(type))
                    yield return customAttribute;
            }
        }

        static T InstantiateBindingsAttribute<T>(CustomAttribute a)
        {
            var type = Type.GetType(a.AttributeType.FullName);
            var arguments = new object[a.ConstructorArguments.Count];
            var argumentTypes = new Type[a.ConstructorArguments.Count];

            for (var i = 0; i < a.ConstructorArguments.Count; i++)
            {
                arguments[i] = ConvertAttributeArgument(a.ConstructorArguments[i].Value);
                argumentTypes[i] = Type.GetType(a.ConstructorArguments[i].Type.FullName);
            }

            var ctor = type.GetConstructor(BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.Public, null, argumentTypes, null);
            var attribute = (T)ctor.Invoke(arguments);

            foreach (var fieldArgument in a.Fields)
            {
                var fieldInfo = type.GetField(fieldArgument.Name);
                fieldInfo.SetValue(attribute, fieldArgument.Argument.Value);
            }

            foreach (var propertyArgument in a.Properties)
            {
                var propertyInfo = type.GetProperty(propertyArgument.Name);
                propertyInfo.SetValue(attribute, propertyArgument.Argument.Value, null);
            }

            return attribute;
        }

        private static object ConvertAttributeArgument(object value)
        {
            var typeReference = value as TypeReference;
            if (typeReference != null)
            {
                return Type.GetType(typeReference.FullName);
            }
            return value;
        }

        public static bool IsBindingsAttribute(this CustomAttribute attribute)
        {
            var type = Type.GetType(attribute.AttributeType.FullName);
            return type != null && typeof(IBindingsAttribute).IsAssignableFrom(type);
        }

        public static bool IsStatic(this TypeDefinition type)
        {
            // C# compiler emits static types as: Abstract + Sealed
            return type.IsAbstract && type.IsSealed;
        }

        public static string NativeProxy(this TypeDefinition typeDefinition)
        {
            if (GlobalContext.HasDefine("USE_NEW_SCRIPTINGCLASSES_WRAPPERS")
                && typeDefinition.CustomAttributes.Any(x => x.AttributeType.FullName.Contains("ByNativeCode"))
                && !typeDefinition.IsNested)
                return $"Scripting::{typeDefinition.SafeNamespace().Replace(".", "::")}::{typeDefinition.Name}Proxy";
            else
                return null;
        }

        /// <summary>
        /// A type may require bindings generation if it is not a delegate, not an interface and not abstract.
        /// This method does not check if the type contains any methods or attributes that would require generation, for that see RequiresBindingsGeneration.
        /// </summary>
        public static bool MayRequireBindingsGeneration(this TypeDefinition typeDefinition)
        {
            if (typeDefinition.IsDelegate())
                return false;

            if (typeDefinition.IsInterface)
                return false;

            return true;
        }

        /// <summary>
        /// Returns true if the type may require bindings generation and it has at least one method that requires bindings generation.
        /// </summary>
        public static bool RequiresBindingsGeneration(this TypeDefinition typeDefinition)
        {
            if (!typeDefinition.MayRequireBindingsGeneration())
                return false;

            return typeDefinition.HasAttribute<NativeTypeAttribute>()
                || typeDefinition.Methods.Any(m => m.MayRequireBindingsGeneration());
        }

        /// <summary>
        /// Returns true if the method requires bindings generation if it declared in a type that allows it
        /// </summary>
        public static bool MayRequireBindingsGeneration(this MethodDefinition methodDefinition)
        {
            return TypeUtils.IsMethodInternalCall(methodDefinition) && !methodDefinition.IsMarkedAsGeneratedByOldBindingsParser();
        }

        /// <summary>
        /// Returns true if the declaring type allow bindings generation and the method requires it
        /// </summary>
        public static bool RequiresBindingsGeneration(this MethodDefinition methodDefinition)
        {
            return methodDefinition.MayRequireBindingsGeneration() && methodDefinition.DeclaringType.MayRequireBindingsGeneration();
        }

        public static bool MayRequireBindingsGeneration(this PropertyDefinition propertyDefinition)
        {
            return (propertyDefinition.GetMethod != null && MayRequireBindingsGeneration(propertyDefinition.GetMethod))
                || (propertyDefinition.SetMethod != null && MayRequireBindingsGeneration(propertyDefinition.SetMethod));
        }

        public static bool RequiresBindingsGeneration(this PropertyDefinition propertyDefinition)
        {
            return (propertyDefinition.GetMethod != null && RequiresBindingsGeneration(propertyDefinition.GetMethod))
                || (propertyDefinition.SetMethod != null && RequiresBindingsGeneration(propertyDefinition.SetMethod));
        }

        public static bool IsMarkedAsGeneratedByOldBindingsParser(this MethodDefinition methodDefinition)
        {
            return methodDefinition.HasAttribute<GeneratedByOldBindingsGeneratorAttribute>();
        }

        public static bool IsExtern(this MethodDefinition methodDefinition)
        {
            if (methodDefinition.DeclaringType.IsInterface)
                return false;

            return methodDefinition.RVA == 0 && !methodDefinition.IsAbstract;
        }

        public static PropertyDefinition GetMatchingPropertyDefinition(this MethodDefinition methodDefinition)
        {
            if (!methodDefinition.IsGetter && !methodDefinition.IsSetter)
                return null;

            var propertyName = methodDefinition.Name.Substring(4);
            var propertyDefinition = methodDefinition.DeclaringType.Properties.SingleOrDefault(p => p.Name == propertyName);

            return propertyDefinition;
        }

        public static bool IsDelegate(this TypeDefinition type)
        {
            if (type.BaseType == null)
                return false;

            return type.BaseType.FullName == "System.MulticastDelegate";
        }

        public static string Capitalize(this string name)
        {
            return string.Format("{0}{1}", name[0].ToString().ToUpper(), name.Substring(1));
        }

        public static string SimpleTypeName(this TypeReference typeReference)
        {
            switch (typeReference.MetadataType)
            {
                case MetadataType.Boolean:      return "bool";
                case MetadataType.Char:         return "char";

                case MetadataType.SByte:        return "sbyte";
                case MetadataType.Int16:        return "short";
                case MetadataType.Int32:        return "int";
                case MetadataType.Int64:        return "long";

                case MetadataType.Byte:         return "byte";
                case MetadataType.UInt16:       return "ushort";
                case MetadataType.UInt32:       return "uint";
                case MetadataType.UInt64:       return "ulong";

                case MetadataType.Single:       return "float";
                case MetadataType.Double:       return "double";

                case MetadataType.String:       return "string";

                default:
                    return typeReference.Name;
            }
        }

        public static bool ExposesManagedTypeNameFor(this string str, TypeReference type)
        {
            return str.Contains(TypeUtils.RegisteredNameFor(type.GetElementType()));
        }

        // Nested types do not have namespace listed
        public static string SafeNamespace(this TypeReference type)
        {
            if (type.IsNested)
                return SafeNamespace(type.DeclaringType);

            return type.Namespace;
        }

        public static string NestedName(this TypeReference type, string nestedClassSeparator = ".")
        {
            if (type.IsNested)
                return NestedName(type.DeclaringType) + nestedClassSeparator + type.Name;

            return type.Name;
        }

        public static string NamespaceAndNestedName(this TypeReference type, string nestedClassSeparator = ".")
        {
            if (type.IsNested)
                return NamespaceAndNestedName(type.DeclaringType) + nestedClassSeparator + type.Name;

            return type.Namespace + "." + type.Name;
        }
    }
}
