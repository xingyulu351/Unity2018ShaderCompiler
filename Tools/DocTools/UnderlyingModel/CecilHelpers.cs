using System;
using System.Linq;
using Mono.Cecil;

namespace UnderlyingModel
{
    public static class CecilHelpers
    {
        public static bool IsDelegate(TypeDefinition typedef)
        {
            return typedef.BaseType != null && typedef.BaseType.FullName == "System.MulticastDelegate";
        }

        private static bool ContainsObsolete(CustomAttribute m)
        {
            return m.AttributeType.FullName.Contains("Obsolete");
        }

        public static ObsoleteInfoType GetObsoleteInfo(ICustomAttributeProvider typedef)
        {
            bool isObsolete = typedef.CustomAttributes.Any(ContainsObsolete);
            if (!isObsolete)
                return new ObsoleteInfoType();
            var obsoleteAttribute = typedef.CustomAttributes.FirstOrDefault(ContainsObsolete);

            if (obsoleteAttribute == null || obsoleteAttribute.ConstructorArguments.Count == 0)
                return new ObsoleteInfoType();
            var obsoleteText = obsoleteAttribute.ConstructorArguments[0].Value.ToString();
            bool isError = obsoleteAttribute.ConstructorArguments.Count > 1 && (bool)obsoleteAttribute.ConstructorArguments[1].Value;
            return new ObsoleteInfoType(true, isError, obsoleteText);
        }

        public static bool ExcludedFromDocs(ICustomAttributeProvider typedef)
        {
            return typedef.CustomAttributes.Any(m => m.AttributeType.Name == "ExcludeFromDocsAttribute");
        }

        public static bool HasDefaultValue(ParameterDefinition paramdef)
        {
            return paramdef.CustomAttributes.Any(m => m.AttributeType.Name == "DefaultValueAttribute");
        }

        public static string GetParameterDefaultValue(ParameterDefinition paramdef)
        {
            var attr = paramdef.CustomAttributes.FirstOrDefault(m => m.AttributeType.Name == "DefaultValueAttribute");
            if (attr != null)
            {
                var arg = attr.ConstructorArguments[0];
                return arg.Value.ToString();
            }
            return null;
        }

        public static bool IsFieldPublic(FieldDefinition field)
        {
            //FieldAttributes.Public is a combination of multiple bits, all of which must be true
            return (field.Attributes & FieldAttributes.Public) == FieldAttributes.Public;
        }

        public static bool IsMethodImplop(MethodDefinition method)
        {
            return method.Name.StartsWith("op_Implicit");
        }

        public static bool IsMethodOp(MethodDefinition method)
        {
            return method.Name.StartsWith("op_");
        }

        public static bool IsMethodEvent(MethodDefinition method)
        {
            return method.Name.StartsWith("add_") || method.Name.StartsWith("remove_");
        }

        public static bool IsOverride(MethodDefinition method)
        {
            return !method.IsConstructOrDestruct() && method.IsReuseSlot && method.IsVirtual;
        }

        public static bool IsVoid(this TypeReference propertyType)
        {
            return propertyType == propertyType.Module.TypeSystem.Void;
        }

        public static bool IsBoolean(this TypeReference propertyType)
        {
            return propertyType == propertyType.Module.TypeSystem.Boolean;
        }

        public static bool IsParams(ParameterDefinition param)
        {
            return param.HasCustomAttributes && param.CustomAttributes.Any(m => m.AttributeType.FullName.Contains("System.ParamArrayAttribute"));
        }

        public static bool IsOut(ParameterDefinition param)
        {
            return (param.Attributes & ParameterAttributes.Out) != 0;
        }

        public static bool IsRef(ParameterDefinition param)
        {
            return param.ParameterType.IsByReference;
        }

        public static bool IsStatic(this PropertyDefinition propertyDefinition)
        {
            var method = propertyDefinition.GetMethod ?? propertyDefinition.SetMethod;
            return method.IsStatic;
        }

        public static bool IsStatic(this MemberReference mr)
        {
            var pd = mr as PropertyDefinition;
            if (pd != null) return pd.IsStatic();

            var md = mr as MethodDefinition;
            if (md != null) return md.IsStatic;

            return false;
        }

        public static bool IsPublic(this PropertyDefinition prop)
        {
            if (prop.SetMethod != null)
                if (prop.SetMethod.IsPublic)
                    return true;
            if (prop.GetMethod != null)
                if (prop.GetMethod.IsPublic)
                    return true;
            return false;
        }

        public static bool IsConstructOrDestruct(this MethodDefinition md)
        {
            return md.Name == "Dispose" || md.Name == "_Dispose" || md.Name == "Finalize"
                || md.IsConstructor;
        }

        public static TypeReference GetReturnType(this MemberReference memberReference)
        {
            var method = memberReference as MethodDefinition;
            if (method != null)
                return method.ReturnType;

            var property = memberReference as PropertyDefinition;
            if (property != null)
                return property.PropertyType;

            throw new ArgumentException();
        }

        public static bool IsDerived(TypeDefinition typeDefinition)
        {
            return typeDefinition.BaseType != null
                && typeDefinition.BaseType.FullName != "System.Enum"
                && typeDefinition.BaseType.FullName != "System.ValueType"
                && typeDefinition.BaseType.FullName != "System.Object"
                && typeDefinition.BaseType.FullName != "UnityEngine.Object"
                && typeDefinition.BaseType.FullName != "System.Attribute"
                && typeDefinition.BaseType.FullName != "System.MulticastDelegate";
        }

        public static bool IsNamespaceOmittable(string nameSpace)
        {
            return nameSpace.IsEmpty()
                || nameSpace == "System"
                || nameSpace == "UnityEngine"
                || nameSpace == "UnityEngineInternal"
                || nameSpace == "UnityEditor"
                || nameSpace == "UnityEditorInternal";
        }

        public static bool IsNamespaceInternal(string nameSpace)
        {
            return nameSpace.StartsWith("UnityEngineInternal")
                || nameSpace.StartsWith("UnityEditorInternal")
                || nameSpace.StartsWith("UnityEngine.Internal")
                || nameSpace.StartsWith("UnityEditor.Internal");
        }

        public static bool IsCompilerGenerated(TypeDefinition typeDefinition)
        {
            return typeDefinition.CustomAttributes.Any(a => a.AttributeType.FullName == "System.Runtime.CompilerServices.CompilerGeneratedAttribute");
        }

        //workaround for case 622141
        public static bool IsEditorSpritesDataUtility(TypeDefinition typeDefinition)
        {
            return typeDefinition.Name == "DataUtility" && typeDefinition.Namespace == "UnityEditor.Sprites";
        }

        //workaround for case 693302
        public static bool IsAssetModificationProcessor(TypeDefinition typeDefinition)
        {
            return typeDefinition.Name == "AssetModificationProcessor" && typeDefinition.Namespace == "";
        }

        public static bool IsSystemObject(TypeReference typeDefinition)
        {
            return typeDefinition.FullName == "System.Object";
        }

        public static bool IsAttribute(TypeDefinition typeDefinition)
        {
            return typeDefinition.BaseType != null && typeDefinition.BaseType.FullName == "System.Attribute";
        }

        //any human-readable string or anything used for file names should get rid of backticks
        public static string Backticks2Underscores(string name)
        {
            return name.Replace("`", "_");
        }

        public static string GetTypeName(TypeReference tr)
        {
            var ns = tr.IsNested ? tr.DeclaringType.Namespace : tr.Namespace;
            string simplifiedNamespace = SimplifyNamespace(ns);
            return GetTypeName(tr, simplifiedNamespace, Backticks2Underscores(tr.Name), tr.IsNested);
        }

        private static string GetTypeName(MemberReference td, string simplifiedNamespace, string name, bool isNested)
        {
            string itemName = name;
            var pType = td;
            while (isNested && pType.DeclaringType != null)
            {
                pType = pType.DeclaringType;
                itemName = pType.Name + "." + itemName;
            }
            itemName = simplifiedNamespace.IsEmpty()
                ? itemName
                : String.Format("{0}.{1}", simplifiedNamespace, itemName);
            return itemName;
        }

        //inner classes, fields and methods don't always have the correct namespace info, check in the declaring types (recursively)
        public static string NamespaceFromDescendants(TypeReference declaringType)
        {
            var ns = "";
            while (ns == "" && declaringType != null)
            {
                ns = declaringType.Namespace;
                declaringType = declaringType.DeclaringType;
            }
            return ns;
        }

        public static string SimplifyNamespace(string ns)
        {
            if (ns.IsEmpty())
                return ns;
            var splitNamespace = ns.Split('.').Where(n => !IsNamespaceOmittable(n));

            return String.Join(".", splitNamespace.ToArray());
        }
    }
}
