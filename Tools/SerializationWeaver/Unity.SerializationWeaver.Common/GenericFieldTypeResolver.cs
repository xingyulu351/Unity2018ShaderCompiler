using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using Mono.Collections.Generic;

namespace Unity.SerializationWeaver.Common
{
    public class GenericFieldTypeResolver : IFieldTypeResolver
    {
        private readonly Stack<Collection<TypeReference>> _genericArguments = new Stack<Collection<TypeReference>>();

        public void EnterGenericArgumentsProvider(Collection<TypeReference> genericArguments)
        {
            _genericArguments.Push(genericArguments);
        }

        public void ExitGenericArgumentsProvider()
        {
            _genericArguments.Pop();
        }

        public TypeReference TypeOf(FieldReference fieldDefinition)
        {
            return ResolveGenericParameters(fieldDefinition.FieldType);
        }

        public void ResetGenericArgumentStack()
        {
            while (_genericArguments.Count > 0)
                _genericArguments.Pop();
        }

        public IEnumerable<TypeReference> GenericArguments
        {
            get { return _genericArguments.Peek(); }
        }

        private TypeReference ResolveGenericParameters(TypeReference typeReference)
        {
            if (typeReference.IsGenericParameter)
            {
                var genericParameter = (GenericParameter)typeReference;
                return GenericArguments.ElementAt(genericParameter.Position);
            }

            if (typeReference.IsGenericInstance)
            {
                var genericInstanceType = (GenericInstanceType)typeReference;
                var newGenericInstanceType = new GenericInstanceType(ResolveGenericParameters(genericInstanceType.ElementType));
                foreach (var genericArgument in genericInstanceType.GenericArguments)
                    newGenericInstanceType.GenericArguments.Add(ResolveGenericParameters(genericArgument));
                return newGenericInstanceType;
            }

            if (typeReference.IsArray)
            {
                var arrayType = (ArrayType)typeReference;
                return new ArrayType(ResolveGenericParameters(arrayType.ElementType), arrayType.Rank);
            }

            if (typeReference.IsPointer)
                return new PointerType(ResolveGenericParameters(((PointerType)typeReference).ElementType));

            if (typeReference.IsByReference)
                return new ByReferenceType(ResolveGenericParameters(((ByReferenceType)typeReference).ElementType));

            return typeReference;
        }
    }
}
