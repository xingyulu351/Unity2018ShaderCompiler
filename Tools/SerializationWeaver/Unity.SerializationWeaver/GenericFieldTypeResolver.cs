using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace Unity.Serialization.Weaver
{
    public class GenericFieldTypeResolver
    {
        class Context
        {
            private readonly TypeReference _typeReference;
            private readonly GenericFieldTypeResolver _fieldTypeResolver;
            private readonly Dictionary<string, TypeReference> _scope = new Dictionary<string, TypeReference>();

            public Context(TypeReference typeReference, Context parentContext, GenericFieldTypeResolver fieldTypeResolver)
            {
                _typeReference = typeReference;
                _fieldTypeResolver = fieldTypeResolver;

                BuildScope(parentContext);
            }

            private void BuildScope(Context parentContext)
            {
                if (parentContext != null)
                    foreach (var reference in parentContext._scope)
                        _scope[reference.Key] = reference.Value;

                var genericInstanceType = _typeReference as GenericInstanceType;
                if (genericInstanceType == null)
                    return;

                var elementType = genericInstanceType.ElementType;
                foreach (var genericParameter in elementType.GenericParameters)
                {
                    var typeReference = genericInstanceType.GenericArguments.ElementAt(genericParameter.Position);
                    if (_fieldTypeResolver.HasGenericParameters(typeReference))
                        _scope[genericParameter.Name] = _fieldTypeResolver.ResolveGenericParameters(typeReference);
                    else
                        _scope[genericParameter.Name] = typeReference;
                }
            }

            public TypeReference Resolve(GenericParameter genericParameter)
            {
                return genericParameter.Module.ImportReference(_scope[genericParameter.Name]);
            }

            public bool CanResolve(GenericParameter genericParameter)
            {
                return ((TypeReference)genericParameter.Owner).FullName == _typeReference.GetElementType().FullName;
            }
        }

        private readonly Stack<Context> _resolutionContext = new Stack<Context>();

        public TypeReference TypeOf(FieldReference fieldDefinition)
        {
            var fieldType = fieldDefinition.FieldType;

            if (HasGenericParameters(fieldType))
                return ResolveGenericParameters(fieldType);

            return fieldType;
        }

        public TypeReference ResolveGenericParameters(TypeReference typeReference)
        {
            if (typeReference.IsGenericParameter)
                return ResolveByName((GenericParameter)typeReference);

            if (typeReference.IsArray)
                return new ArrayType(ResolveGenericParameters(typeReference.GetElementType()));

            if (typeReference.IsGenericInstance)
            {
                var genericInstanceType = ((GenericInstanceType)typeReference);
                var newGenericInstanceType = new GenericInstanceType(genericInstanceType.ElementType);
                foreach (var genericArgument in genericInstanceType.GenericArguments)
                    newGenericInstanceType.GenericArguments.Add(ResolveGenericParameters(genericArgument));
                return newGenericInstanceType;
            }

            throw new ResolutionException(typeReference);
        }

        private TypeReference ResolveByName(GenericParameter genericParameter)
        {
            foreach (var context in _resolutionContext.Reverse().Where(context => context.CanResolve(genericParameter)))
                return context.Resolve(genericParameter);

            throw new ResolutionException(genericParameter);
        }

        public bool HasGenericParameters(TypeReference typeReference)
        {
            if (typeReference.IsGenericInstance)
            {
                var genericInstanceType = ((GenericInstanceType)typeReference);
                return HasGenericParameters(genericInstanceType.ElementType) || genericInstanceType.GenericArguments.Any(HasGenericParameters);
            }

            if (typeReference.IsArray)
                return HasGenericParameters(((ArrayType)typeReference).ElementType);

            if (typeReference.IsGenericParameter)
                return true;

            return false;
        }

        public void ResetGenericArgumentStack()
        {
            _resolutionContext.Clear();
        }

        public void AddGenericArgumentScope(TypeReference typeReference)
        {
            _resolutionContext.Push(new Context(typeReference, _resolutionContext.Count == 0 ? null : _resolutionContext.Peek(), this));
        }
    }
}
