using System;
using Mono.Cecil;

public static class Naming
{
    public static string CppNamefor(TypeDefinition typeDefinition)
    {
        var altName = AlternativeNameFor(typeDefinition);
        if (altName != null)
            return altName;

        return CleanName(ExtractNameFrom(typeDefinition));
    }

    private static string ExtractNameFrom(TypeDefinition typeDefinition)
    {
        if (!typeDefinition.IsNested)
            return typeDefinition.Name;

        return ExtractNameFrom(typeDefinition.DeclaringType) + typeDefinition.Name;
    }

    public static string CppNamefor(MethodDefinition methodDefinition)
    {
        var altName = AlternativeNameFor(methodDefinition);
        if (altName != null)
            return altName;

        return CleanName(methodDefinition.Name);
    }

    internal static string CleanName(string name)
    {
        var cppName = name;
        for (var i = 0; i < name.Length && Char.IsUpper(name[i]) && (i == name.Length - 1 || Char.IsUpper(name[i + 1]) || i == 0); i++)
            cppName = cppName.Substring(0, i) + Char.ToLower(cppName[i]) + cppName.Substring(i + 1);
        return cppName;
    }

    internal static string AlternativeNameFor(ICustomAttributeProvider attributeProvider)
    {
        var attr = PreserveLogic.GetPreserveUnityEngineAttribute(attributeProvider);
        if (attr == null)
            return null;
        if (attr.HasConstructorArguments)
        {
            var possibleName = attr.ConstructorArguments[0];
            if (possibleName.Type.MetadataType == MetadataType.String)
                return (string)possibleName.Value;
        }

        if (attr.HasFields)
        {
            foreach (var namedArgument in attr.Fields)
            {
                if (namedArgument.Name == "Name")
                    return (string)namedArgument.Argument.Value;
            }
        }

        if (attr.HasProperties)
        {
            foreach (var namedArgument in attr.Properties)
            {
                if (namedArgument.Name == "Name")
                    return (string)namedArgument.Argument.Value;
            }
        }

        return null;
    }

    public static bool IsUnityEngineType(TypeReference typeReference)
    {
        while (typeReference.IsNested)
            typeReference = typeReference.DeclaringType;
        return typeReference.Namespace.StartsWith("Unity");
    }
}
