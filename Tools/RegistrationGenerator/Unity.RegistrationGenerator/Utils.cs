using System;
using System.Collections.Generic;
using Mono.Cecil;

namespace Unity.RegistrationGenerator
{
    static class Utils
    {
        public static bool IsSubclassOf(TypeDefinition type, TypeDefinition parent)
        {
            while (type != null)
            {
                type = type.BaseType?.Resolve();

                if (type == parent)
                    return true;
            }

            return false;
        }

        public static IEnumerable<CustomAttribute> GetCustomAttributesIncludingParent(TypeDefinition type, TypeDefinition attributeType)
        {
            while (type != null)
            {
                foreach (var customAttribute in type.CustomAttributes)
                {
                    if (customAttribute.AttributeType.Resolve() == attributeType)
                        yield return customAttribute;
                }

                type = type.BaseType?.Resolve();
            }
        }

        public static IEnumerable<TypeDefinition> GetAllTypes(this AssemblyDefinition assembly)
        {
            foreach (var type in assembly.MainModule.Types)
            {
                yield return type;

                foreach (var nestedType in GetAllNestedTypes(type))
                    yield return nestedType;
            }
        }

        private static IEnumerable<TypeDefinition> GetAllNestedTypes(TypeDefinition type)
        {
            foreach (var nestedType in type.NestedTypes)
            {
                yield return nestedType;

                foreach (var nestedNestedType in GetAllNestedTypes(nestedType))
                    yield return nestedNestedType;
            }
        }
    }
}
