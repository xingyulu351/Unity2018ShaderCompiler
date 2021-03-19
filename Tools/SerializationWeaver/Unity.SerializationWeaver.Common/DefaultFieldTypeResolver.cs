using Mono.Cecil;

namespace Unity.SerializationWeaver.Common
{
    public class DefaultFieldTypeResolver : IFieldTypeResolver
    {
        public TypeReference TypeOf(FieldReference fieldDefinition)
        {
            return fieldDefinition.FieldType;
        }
    }
}
