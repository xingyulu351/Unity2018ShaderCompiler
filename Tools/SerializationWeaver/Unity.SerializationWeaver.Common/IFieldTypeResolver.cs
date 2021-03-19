using Mono.Cecil;

namespace Unity.SerializationWeaver.Common
{
    public interface IFieldTypeResolver
    {
        TypeReference TypeOf(FieldReference fieldDefinition);
    }
}
