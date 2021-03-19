using System.Linq;
using Mono.Cecil;

public static class PreserveLogic
{
    internal const string UsedByNativeCodeAttributeName = "UsedByNativeCodeAttribute";
    internal const string RequiredByNativeCodeAttributeName = "RequiredByNativeCodeAttribute";

    internal static PreserveState GetMostPreservingState(PreserveState first, PreserveState second)
    {
        if (first == PreserveState.Required || second == PreserveState.Required)
            return PreserveState.Required;
        if (first == PreserveState.Used || second == PreserveState.Used)
            return PreserveState.Used;
        return PreserveState.Unused;
    }

    internal static CustomAttribute GetPreserveUnityEngineAttribute(ICustomAttributeProvider attributeProvider)
    {
        return attributeProvider.CustomAttributes.SingleOrDefault(a => IsPreserveAttribute(a.AttributeType));
    }

    internal static bool IsPreserveAttribute(TypeReference typeReference)
    {
        if (typeReference == null)
            return false;

        if (typeReference.Name == UsedByNativeCodeAttributeName || typeReference.Name == RequiredByNativeCodeAttributeName)
            return true;

        var typeDefinition = typeReference.Resolve();
        if (typeDefinition == null)
            return false;

        return IsPreserveAttribute(typeDefinition.BaseType);
    }

    internal static PreserveState ExtractPreserveStateFrom(ICustomAttributeProvider provider)
    {
        var attr = GetPreserveUnityEngineAttribute(provider);
        if (attr == null)
            return PreserveState.Unused;

        if (attr.AttributeType.Name == UsedByNativeCodeAttributeName)
            return PreserveState.Used;

        return IsMarkedAsOptional(provider) ? PreserveState.Used : PreserveState.Required;
    }

    internal static bool IsMarkedAsOptional(ICustomAttributeProvider attributeProvider)
    {
        var attr = GetPreserveUnityEngineAttribute(attributeProvider);
        if (attr == null)
            return false;

        if (attr.HasConstructorArguments)
        {
            var possibleOptional = attr.ConstructorArguments[0];
            if (possibleOptional.Type.MetadataType == MetadataType.Boolean)
                return (bool)possibleOptional.Value;

            if (attr.ConstructorArguments.Count > 1)
            {
                possibleOptional = attr.ConstructorArguments[1];
                if (possibleOptional.Type.MetadataType == MetadataType.Boolean)
                    return (bool)possibleOptional.Value;
            }
        }

        if (attr.HasFields)
        {
            foreach (var namedArgument in attr.Fields)
            {
                if (namedArgument.Name == "Optional")
                    return (bool)namedArgument.Argument.Value;
            }
        }

        if (attr.HasProperties)
        {
            foreach (var namedArgument in attr.Properties)
            {
                if (namedArgument.Name == "Optional")
                    return (bool)namedArgument.Argument.Value;
            }
        }

        return false;
    }
}
