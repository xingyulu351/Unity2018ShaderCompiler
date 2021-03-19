using System;
using System.Text;
using System.Collections.Generic;
//using Mono.Collections.Generic;
using Mono.Cecil;

public interface IAssemblyApiVisitor
{
    void ModuleStart(string moduleName);
    void ModuleEnd();
    void TypeStart(bool isValueType, string nameSpace, string name);
    void TypeEnd();
    void NestedTypeStart(bool isValueType, string name);
    void NestedTypeEnd();
    void Field(string keyName, string typeDocName, string docName);
    void Property(string keyName, string typeDocName, string docName, bool hasGetter, bool hasSetter);
    void Method(string keyName, string returnTypeDocName, string docName, string docArgumentsRepresentation);
}

public class PrintAssemblyApi : IAssemblyApiVisitor
{
    public void ModuleStart(string moduleName)
    {
        Console.WriteLine("ModuleStart {0}", moduleName);
    }

    public void ModuleEnd()
    {
        Console.WriteLine("ModuleEnd");
    }

    public void TypeStart(bool isValueType, string nameSpace, string name)
    {
        Console.WriteLine("  TypeStart {0}:{1} (valueType {2})", nameSpace, name, isValueType);
    }

    public void TypeEnd()
    {
        Console.WriteLine("TypeEnd");
    }

    public void NestedTypeStart(bool isValueType, string name)
    {
        Console.WriteLine("    NestedTypeStart {0}", name);
    }

    public void NestedTypeEnd()
    {
        Console.WriteLine("    NestedTypeEnd");
    }

    public void Field(string keyName, string typeDocName, string docName)
    {
        Console.WriteLine("    Field {0}", keyName);
    }

    public void Property(string keyName, string typeDocName, string docName, bool hasGetter, bool hasSetter)
    {
        Console.WriteLine("    Property {0} (get {1}, set {2})", keyName, hasGetter, hasSetter);
    }

    public void Method(string keyName, string returnTypeDocName, string docName, string docArgumentsRepresentation)
    {
        Console.WriteLine("    Method {0}", keyName);
    }
}

static class CecilUtilities
{
    public delegate string ParameterDefinitionRepresentation(ParameterDefinition pd);

    static string DocGenericParameters(this IGenericParameterProvider self)
    {
        if (self.HasGenericParameters)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("<");
            for (int i = 0; i < self.GenericParameters.Count; i++)
            {
                if (i > 0)
                {
                    sb.Append(",");
                }
                sb.Append(self.GenericParameters[i].Name);
            }
            sb.Append(">");
            return sb.ToString();
        }
        else
        {
            return "";
        }
    }

    static string CleanupGenericSuffix(this string self)
    {
        int suffixIndex = self.IndexOf('`');
        if (suffixIndex < 0)
        {
            return self;
        }
        else
        {
            return self.Substring(0, suffixIndex);
        }
    }

    public static string DocName(this TypeReference self)
    {
        return self.Name.CleanupGenericSuffix() + self.DocGenericParameters();
    }

    public static string KeyName(this TypeReference self)
    {
        if (self.IsNested)
        {
            return self.DeclaringType.KeyName() + "/" + self.DocName();
        }
        else
        {
            return (string.IsNullOrEmpty(self.Namespace) ? self.DocName() : self.Namespace + "." + self.DocName());
        }
    }

    static string KeyParameterRepresentation(this ParameterDefinition parameter)
    {
        StringBuilder sb = new StringBuilder();
        sb.Append(parameter.ParameterType.KeyName());
        sb.Append(" ");
        sb.Append(parameter.Name);
        return sb.ToString();
    }

    static string DocParameterRepresentation(this ParameterDefinition parameter)
    {
        StringBuilder sb = new StringBuilder();
        sb.Append(parameter.ParameterType.DocName());
        sb.Append(" ");
        sb.Append(parameter.Name);
        return sb.ToString();
    }

    public static string BuildArguments(this MethodDefinition self, ParameterDefinitionRepresentation parameterProcessor)
    {
        StringBuilder sb = new StringBuilder();

        sb.Append("(");
        for (int i = 0; i < self.Parameters.Count; i++)
        {
            if (i > 0)
            {
                sb.Append(", ");
            }

            sb.Append(parameterProcessor(self.Parameters[i]));
        }
        sb.Append(")");
        return sb.ToString();
    }

    public static string KeyName(this MethodDefinition self)
    {
        StringBuilder sb = new StringBuilder();

        sb.Append(self.ReturnType.ReturnType.KeyName());

        sb.Append(" ");
        sb.Append(self.Name.CleanupGenericSuffix());
        sb.Append(self.DocGenericParameters());

        sb.Append(" ");
        sb.Append(self.BuildArguments((p) => (p.KeyParameterRepresentation())));

        return sb.ToString();
    }

    public static string DocReturnTypeName(this MethodDefinition self)
    {
        if (!self.IsConstructor)
            return self.ReturnType.ReturnType.DocName();
        else
            return null;
    }

    public static string DocName(this MethodDefinition self)
    {
        if (!self.IsConstructor)
            return self.Name.CleanupGenericSuffix() + self.DocGenericParameters();
        else
            return self.DeclaringType.DocName();
    }

    public static string DocArgumentsRepresentation(this MethodDefinition self)
    {
        return self.BuildArguments((p) => (p.DocParameterRepresentation()));
    }

    public static string KeyName(this PropertyDefinition self)
    {
        return self.PropertyType.KeyName() + " " + self.Name;
    }

    public static string DocTypeName(this PropertyDefinition self)
    {
        return self.PropertyType.DocName();
    }

    public static string DocName(this PropertyDefinition self)
    {
        return self.Name;
    }

    public static string KeyName(this FieldDefinition self)
    {
        return self.FieldType.KeyName() + " " + self.Name;
    }

    public static string DocTypeName(this FieldDefinition self)
    {
        return self.FieldType.DocName();
    }

    public static string DocName(this FieldDefinition self)
    {
        return self.Name;
    }

    static bool ContainsSecurityCritical(this CustomAttributeCollection self)
    {
        foreach (CustomAttribute attribute in self)
        {
            if (attribute.Constructor.DeclaringType.Name == "SecurityCriticalAttribute")
            {
                return true;
            }
        }
        return false;
    }

    public static bool IsPartOfApiDocumentation(this TypeDefinition self)
    {
        if (!self.IsPublic)
            return false;

        if (self.IsEnum)
            return false;

        if (self.BaseType != null)
            if ((self.BaseType.Namespace == "System") && ((self.BaseType.Name == "Delegate") || (self.BaseType.Name == "MulticastDelegate")))
                return false;

        if (!(self.IsClass || self.IsValueType))
            return false;

        //if (self.CustomAttributes.ContainsSecurityCritical ())
        //  return false;

        return true;
    }

    public static bool IsPartOfApiDocumentation(this MethodDefinition self)
    {
        if (self == null)
            return false;

        if (!self.IsPublic)
            return false;

        if (self.CustomAttributes.ContainsSecurityCritical())
            return false;

        return true;
    }

    public static bool IsPartOfApiDocumentation(this FieldDefinition self)
    {
        if (!self.IsPublic)
            return false;

        if (self.CustomAttributes.ContainsSecurityCritical())
            return false;

        return true;
    }
}

public class AssemblyApiScanner
{
    static void ScanTypeFields(TypeDefinition type, IAssemblyApiVisitor visitor)
    {
        if (type.HasFields)
            foreach (FieldDefinition field in type.Fields)
                if (field.IsPartOfApiDocumentation())
                    visitor.Field(field.KeyName(), field.DocTypeName(), field.DocName());
    }

    static void ScanTypeProperties(TypeDefinition type, IAssemblyApiVisitor visitor)
    {
        if (type.HasProperties)
        {
            foreach (PropertyDefinition property in type.Properties)
            {
                bool getterIsVisible = property.GetMethod.IsPartOfApiDocumentation();
                bool setterIsVisible = property.SetMethod.IsPartOfApiDocumentation();

                if (getterIsVisible || setterIsVisible)
                {
                    visitor.Property(property.KeyName(), property.DocTypeName(), property.DocName(), getterIsVisible, setterIsVisible);
                }
            }
        }
    }

    static void ScanTypeMethods(TypeDefinition type, IAssemblyApiVisitor visitor)
    {
        if (type.HasMethods || type.HasConstructors)
            foreach (MethodDefinition method in GetMethodsAndConstructors(type))
                if (method.IsPartOfApiDocumentation() && (!method.IsGetter) && (!method.IsSetter))
                    visitor.Method(method.KeyName(), method.DocReturnTypeName(), method.DocName(), method.DocArgumentsRepresentation());
    }

    private static IEnumerable<MethodDefinition> GetMethodsAndConstructors(TypeDefinition type)
    {
        foreach (MethodDefinition m in type.Constructors)
            yield return m;
        foreach (MethodDefinition m in type.Methods)
            yield return m;
    }

    static void ScanTypeNestedTypes(TypeDefinition type, IAssemblyApiVisitor visitor)
    {
        if (type.HasNestedTypes)
            foreach (TypeDefinition nestedType in type.NestedTypes)
                if (nestedType.IsPartOfApiDocumentation())
                {
                    visitor.NestedTypeStart(nestedType.IsValueType, nestedType.DocName());
                    ScanType(nestedType, visitor);
                    visitor.NestedTypeEnd();
                }
    }

    static void ScanType(TypeDefinition type, IAssemblyApiVisitor visitor)
    {
        ScanTypeFields(type, visitor);
        ScanTypeProperties(type, visitor);
        ScanTypeMethods(type, visitor);
        ScanTypeNestedTypes(type, visitor);
    }

    public static void Scan(AssemblyDefinition assembly, IAssemblyApiVisitor visitor)
    {
        visitor.ModuleStart(assembly.Name.Name);
        foreach (ModuleDefinition module in assembly.Modules)
        {
            foreach (TypeDefinition type in module.Types)
            {
                if (type.IsPartOfApiDocumentation())
                {
                    visitor.TypeStart(type.IsValueType, type.Namespace, type.DocName());
                    ScanType(type, visitor);
                    visitor.TypeEnd();
                }
            }
        }
        visitor.ModuleEnd();
    }
}
