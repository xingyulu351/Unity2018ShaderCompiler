using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Mono.Cecil;

namespace Unity.CecilTools.Extensions
{
    public static class TypeDefinitionExtensions
    {
        public static bool IsSubclassOf(this TypeDefinition type, string baseTypeName)
        {
            var baseType = type.BaseType;
            if (baseType == null)
                return false;
            if (baseType.FullName == baseTypeName)
                return true;

            var baseTypeDef = baseType.Resolve();
            if (baseTypeDef == null)
                return false;

            return IsSubclassOf(baseTypeDef, baseTypeName);
        }
    }
}
