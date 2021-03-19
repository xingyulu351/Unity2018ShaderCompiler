using System.Collections.Generic;
using Mono.Cecil;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.Core
{
    class IncludesProvider
    {
        readonly List<string> _includes = new List<string>();
        readonly HashSet<string> _alreadyProvided = new HashSet<string>();

        public static IEnumerable<string> IncludesFor(TypeDefinition typeDefinition)
        {
            return ProviderFor(typeDefinition).GetIncludes();
        }

        public static IEnumerable<string> IncludesFor(FieldDefinition fieldDefinition)
        {
            return ProviderFor(fieldDefinition).GetIncludes();
        }

        public static IEnumerable<string> IncludesFor(MethodDefinition methodDefinition)
        {
            return ProviderFor(methodDefinition).GetIncludes();
        }

        public static IEnumerable<string> IncludesFor(ParameterDefinition parameterDefinition)
        {
            return ProviderFor(parameterDefinition).GetIncludes();
        }

        public static IEnumerable<string> IncludesFor(PropertyDefinition propertyDefinition)
        {
            return ProviderFor(propertyDefinition).GetIncludes();
        }

        internal void AddSource(ICustomAttributeProvider provider)
        {
            if (provider == null)
                return;

            _includes.AddRange(GetHeadersFor(provider));
        }

        internal void AddSource(IncludesProvider provider)
        {
            _includes.AddRange(provider.GetIncludes());
        }

        internal IEnumerable<string> GetIncludes()
        {
            return _includes;
        }

        static IncludesProvider ProviderFor(TypeDefinition typeDefinition)
        {
            if (typeDefinition == null)
                return new IncludesProvider();

            var provider = new IncludesProvider();
            provider.AddSource(ProviderFor(typeDefinition.BaseType?.Resolve()));
            provider.AddSource(typeDefinition);
            return provider;
        }

        static IncludesProvider ProviderFor(FieldDefinition fieldDefinition)
        {
            if (fieldDefinition == null)
                return new IncludesProvider();

            var provider = new IncludesProvider();
            provider.AddSource(ProviderFor(fieldDefinition.FieldType?.Resolve()));
            provider.AddSource(fieldDefinition);
            return provider;
        }

        static IncludesProvider ProviderFor(ParameterDefinition parameterDefinition)
        {
            if (parameterDefinition == null)
                return new IncludesProvider();

            var provider = new IncludesProvider();
            provider.AddSource(ProviderFor(parameterDefinition.ParameterType?.Resolve()));
            provider.AddSource(parameterDefinition);
            return provider;
        }

        static IncludesProvider ProviderFor(MethodDefinition methodDefinition)
        {
            if (methodDefinition == null)
                return new IncludesProvider();

            var provider = new IncludesProvider();

            foreach (var parameterDefinition in methodDefinition.Parameters)
                provider.AddSource(ProviderFor(parameterDefinition));

            provider.AddSource(ProviderFor(methodDefinition.ReturnType?.Resolve()));
            provider.AddSource(methodDefinition.MethodReturnType);
            provider.AddSource(methodDefinition);
            return provider;
        }

        static IncludesProvider ProviderFor(PropertyDefinition propertyDefinition)
        {
            if (propertyDefinition == null)
                return new IncludesProvider();

            var provider = new IncludesProvider();

            provider.AddSource(ProviderFor(propertyDefinition.PropertyType?.Resolve()));
            provider.AddSource(propertyDefinition.GetMethod);
            provider.AddSource(propertyDefinition.SetMethod);
            provider.AddSource(propertyDefinition);
            return provider;
        }

        IEnumerable<string> GetHeadersFor(ICustomAttributeProvider attributeProvider)
        {
            foreach (var headerProvider in attributeProvider.GetAttributeInstances<IBindingsHeaderProviderAttribute>())
            {
                if (headerProvider.Header != null && _alreadyProvided.Add(headerProvider.Header))
                    yield return headerProvider.Header;
            }
        }
    }
}
