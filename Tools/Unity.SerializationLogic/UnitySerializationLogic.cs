using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using Mono.Collections.Generic;
using Unity.CecilTools;
using Unity.CecilTools.Extensions;

namespace Unity.SerializationLogic
{
    internal class GenericInstanceHolder
    {
        public int Count;
        public IGenericInstance GenericInstance;
    }

    public class TypeResolver
    {
        private readonly IGenericInstance _typeDefinitionContext;
        private readonly IGenericInstance _methodDefinitionContext;
        private readonly Dictionary<string, GenericInstanceHolder> _context = new Dictionary<string, GenericInstanceHolder>();

        public TypeResolver()
        {
        }

        public TypeResolver(IGenericInstance typeDefinitionContext)
        {
            _typeDefinitionContext = typeDefinitionContext;
        }

        public TypeResolver(GenericInstanceMethod methodDefinitionContext)
        {
            _methodDefinitionContext = methodDefinitionContext;
        }

        public TypeResolver(IGenericInstance typeDefinitionContext, IGenericInstance methodDefinitionContext)
        {
            _typeDefinitionContext = typeDefinitionContext;
            _methodDefinitionContext = methodDefinitionContext;
        }

        public void Add(GenericInstanceType genericInstanceType)
        {
            Add(ElementTypeFor(genericInstanceType).FullName, genericInstanceType);
        }

        public void Remove(GenericInstanceType genericInstanceType)
        {
            Remove(genericInstanceType.ElementType.FullName, genericInstanceType);
        }

        public void Add(GenericInstanceMethod genericInstanceMethod)
        {
            Add(ElementTypeFor(genericInstanceMethod).FullName, genericInstanceMethod);
        }

        private static MemberReference ElementTypeFor(TypeSpecification genericInstanceType)
        {
            return genericInstanceType.ElementType;
        }

        private static MemberReference ElementTypeFor(MethodSpecification genericInstanceMethod)
        {
            return genericInstanceMethod.ElementMethod;
        }

        public void Remove(GenericInstanceMethod genericInstanceMethod)
        {
            Remove(genericInstanceMethod.ElementMethod.FullName, genericInstanceMethod);
        }

        public TypeReference Resolve(TypeReference typeReference)
        {
            var genericParameter = typeReference as GenericParameter;
            if (genericParameter != null)
            {
                var resolved = ResolveGenericParameter(genericParameter);
                if (genericParameter == resolved) // Resolving failed, return what we have.
                    return resolved;

                return Resolve(resolved);
            }

            var arrayType = typeReference as ArrayType;
            if (arrayType != null)
                return new ArrayType(Resolve(arrayType.ElementType), arrayType.Rank);

            var pointerType = typeReference as PointerType;
            if (pointerType != null)
                return new PointerType(Resolve(pointerType.ElementType));

            var byReferenceType = typeReference as ByReferenceType;
            if (byReferenceType != null)
                return new ByReferenceType(Resolve(byReferenceType.ElementType));

            var genericInstanceType = typeReference as GenericInstanceType;
            if (genericInstanceType != null)
            {
                var newGenericInstanceType = new GenericInstanceType(Resolve(genericInstanceType.ElementType));
                foreach (var genericArgument in genericInstanceType.GenericArguments)
                    newGenericInstanceType.GenericArguments.Add(Resolve(genericArgument));
                return newGenericInstanceType;
            }

            var pinnedType = typeReference as PinnedType;
            if (pinnedType != null)
                return new PinnedType(Resolve(pinnedType.ElementType));

            var reqModifierType = typeReference as RequiredModifierType;
            if (reqModifierType != null)
                return Resolve(reqModifierType.ElementType);

            var optModifierType = typeReference as OptionalModifierType;
            if (optModifierType != null)
                return new OptionalModifierType(Resolve(optModifierType.ModifierType), Resolve(optModifierType.ElementType));

            var sentinelType = typeReference as SentinelType;
            if (sentinelType != null)
                return new SentinelType(Resolve(sentinelType.ElementType));

            var funcPtrType = typeReference as FunctionPointerType;
            if (funcPtrType != null)
                throw new NotSupportedException("Function pointer types are not supported by the SerializationWeaver");

            if (typeReference is TypeSpecification)
                throw new NotSupportedException();

            return typeReference;
        }

        private TypeReference ResolveGenericParameter(GenericParameter genericParameter)
        {
            if (genericParameter.Owner == null)
                throw new NotSupportedException();

            var memberReference = genericParameter.Owner as MemberReference;
            if (memberReference == null)
                throw new NotSupportedException();

            var key = memberReference.FullName;
            if (!_context.ContainsKey(key))
            {
                if (genericParameter.Type == GenericParameterType.Type)
                {
                    if (_typeDefinitionContext != null)
                        return _typeDefinitionContext.GenericArguments[genericParameter.Position];

                    return genericParameter;
                }

                if (_methodDefinitionContext != null)
                    return _methodDefinitionContext.GenericArguments[genericParameter.Position];

                return genericParameter;
            }

            return GenericArgumentAt(key, genericParameter.Position);
        }

        private TypeReference GenericArgumentAt(string key, int position)
        {
            return _context[key].GenericInstance.GenericArguments[position];
        }

        private void Add(string key, IGenericInstance value)
        {
            GenericInstanceHolder oldValue;

            if (_context.TryGetValue(key, out oldValue))
            {
                var memberReference = value as MemberReference;
                if (memberReference == null)
                    throw new NotSupportedException();

                var storedValue = (MemberReference)oldValue.GenericInstance;

                if (storedValue.FullName != memberReference.FullName)
                    throw new ArgumentException("Duplicate key!", "key");

                oldValue.Count++;
                return;
            }

            _context.Add(key, new GenericInstanceHolder { Count = 1, GenericInstance = value });
        }

        private void Remove(string key, IGenericInstance value)
        {
            GenericInstanceHolder oldValue;

            if (_context.TryGetValue(key, out oldValue))
            {
                var memberReference = value as MemberReference;
                if (memberReference == null)
                    throw new NotSupportedException();

                var storedValue = (MemberReference)oldValue.GenericInstance;

                if (storedValue.FullName != memberReference.FullName)
                    throw new ArgumentException("Invalid value!", "value");

                oldValue.Count--;
                if (oldValue.Count == 0)
                    _context.Remove(key);

                return;
            }

            throw new ArgumentException("Invalid key!", "key");
        }
    }

    public static class UnitySerializationLogic
    {
        public static bool WillUnitySerialize(FieldDefinition fieldDefinition)
        {
            return WillUnitySerialize(fieldDefinition, new TypeResolver(null));
        }

        public static bool WillUnitySerialize(FieldDefinition fieldDefinition, TypeResolver typeResolver)
        {
            if (fieldDefinition == null)
                return false;

            //skip static, const and NotSerialized fields before even checking the type
            if (fieldDefinition.IsStatic || IsConst(fieldDefinition) || fieldDefinition.IsNotSerialized || fieldDefinition.IsInitOnly)
                return false;

            // Don't try to resolve types that come from Windows assembly,
            // as serialization weaver will fail to resolve that (due to it being in platform specific SDKs)
            if (ShouldNotTryToResolve(fieldDefinition.FieldType))
                return false;

            //AND, the field must have correct visibility/decoration to be serialized.
            bool hasSerializeFieldAttribute = HasSerializeFieldAttribute(fieldDefinition);
            if (!fieldDefinition.IsPublic && !hasSerializeFieldAttribute &&
                !ShouldHaveHadAllFieldsPublic(fieldDefinition))
                return false;

            if (fieldDefinition.FullName == "UnityScript.Lang.Array")
                return false;

            // Resolving types is more complex and slower than checking their names or attributes,
            // thus keep those checks below

            //the type of the field must be serializable in the first place.
            if (!IsFieldTypeSerializable(typeResolver.Resolve(fieldDefinition.FieldType), fieldDefinition))
                return false;

            if (IsDelegate(typeResolver.Resolve(fieldDefinition.FieldType)))
                return false;

            return true;
        }

        private static bool IsDelegate(TypeReference typeReference)
        {
            return typeReference.IsAssignableTo("System.Delegate");
        }

        public static bool ShouldFieldBePPtrRemapped(FieldDefinition fieldDefinition)
        {
            return ShouldFieldBePPtrRemapped(fieldDefinition, new TypeResolver(null));
        }

        public static bool ShouldFieldBePPtrRemapped(FieldDefinition fieldDefinition, TypeResolver typeResolver)
        {
            if (!WillUnitySerialize(fieldDefinition, typeResolver))
                return false;

            return CanTypeContainUnityEngineObjectReference(typeResolver.Resolve(fieldDefinition.FieldType));
        }

        private static bool CanTypeContainUnityEngineObjectReference(TypeReference typeReference)
        {
            if (IsUnityEngineObject(typeReference))
                return true;

            if (typeReference.IsEnum())
                return false;

            if (IsSerializablePrimitive(typeReference))
                return false;

            if (IsSupportedCollection(typeReference))
                return CanTypeContainUnityEngineObjectReference(CecilUtils.ElementTypeOfCollection(typeReference));

            var definition = typeReference.Resolve();
            if (definition == null)
                return false;

            return HasFieldsThatCanContainUnityEngineObjectReferences(definition, new TypeResolver(typeReference as GenericInstanceType));
        }

        private static bool HasFieldsThatCanContainUnityEngineObjectReferences(TypeDefinition definition, TypeResolver typeResolver)
        {
            return AllFieldsFor(definition, typeResolver).Where(kv => kv.Value.Resolve(kv.Key.FieldType).Resolve() != definition).Any(kv => CanFieldContainUnityEngineObjectReference(definition, kv.Key, kv.Value));
        }

        private static IEnumerable<KeyValuePair<FieldDefinition, TypeResolver>> AllFieldsFor(TypeDefinition definition, TypeResolver typeResolver)
        {
            var baseType = definition.BaseType;

            if (baseType != null)
            {
                var genericBaseInstanceType = baseType as GenericInstanceType;
                if (genericBaseInstanceType != null)
                    typeResolver.Add(genericBaseInstanceType);
                foreach (var kv in AllFieldsFor(baseType.Resolve(), typeResolver))
                    yield return kv;
                if (genericBaseInstanceType != null)
                    typeResolver.Remove(genericBaseInstanceType);
            }

            foreach (var fieldDefinition in definition.Fields)
                yield return new KeyValuePair<FieldDefinition, TypeResolver>(fieldDefinition, typeResolver);
        }

        private static bool CanFieldContainUnityEngineObjectReference(TypeReference typeReference, FieldDefinition t, TypeResolver typeResolver)
        {
            if (typeResolver.Resolve(t.FieldType) == typeReference)
                return false;

            if (!WillUnitySerialize(t, typeResolver))
                return false;

            if (UnityEngineTypePredicates.IsUnityEngineValueType(typeReference))
                return false;

            return true;
        }

        private static bool IsConst(FieldDefinition fieldDefinition)
        {
            return fieldDefinition.IsLiteral && !fieldDefinition.IsInitOnly;
        }

        public static bool HasSerializeFieldAttribute(FieldDefinition field)
        {
            //return FieldAttributes(field).Any(UnityEngineTypePredicates.IsSerializeFieldAttribute);
            foreach (var attribute in FieldAttributes(field))
                if (UnityEngineTypePredicates.IsSerializeFieldAttribute(attribute))
                    return true;
            return false;
        }

        private static IEnumerable<TypeReference> FieldAttributes(FieldDefinition field)
        {
            return field.CustomAttributes.Select(_ => _.AttributeType);
        }

        public static bool ShouldNotTryToResolve(TypeReference typeReference)
        {
            if (typeReference.Scope.Name == "Windows")
            {
                return true;
            }

            if (typeReference.Scope.Name == "mscorlib")
            {
                var resolved = typeReference.Resolve();
                return resolved == null;
            }

            try
            {   // This will throw an exception if typereference thinks it's referencing a .dll,
                // but actually there's .winmd file in the current directory. RRW will fix this
                // at a later step, so we will not try to resolve this type. This is OK, as any
                // type defined in a winmd cannot be serialized.
                typeReference.Resolve();
            }
            catch
            {
                return true;
            }

            return false;
        }

        private static bool IsFieldTypeSerializable(TypeReference typeReference, FieldDefinition fieldDefinition)
        {
            return IsTypeSerializable(typeReference) || IsSupportedCollection(typeReference) || IsFixedBuffer(fieldDefinition);
        }

        private static bool IsTypeSerializable(TypeReference typeReference)
        {
            if (typeReference.IsAssignableTo("UnityScript.Lang.Array")) return false;
            if (IsGenericDictionary(typeReference)) return false;

            return IsSerializablePrimitive(typeReference)
                || typeReference.IsEnum()
                || IsUnityEngineObject(typeReference)
                || UnityEngineTypePredicates.IsSerializableUnityStruct(typeReference)
                || ShouldImplementIDeserializable(typeReference);
        }

        private static bool IsGenericDictionary(TypeReference typeReference)
        {
            var current = typeReference;

            if (current != null)
            {
                if (CecilUtils.IsGenericDictionary(current))
                    return true;
            }

            return false;
        }

        public static bool IsFixedBuffer(FieldDefinition fieldDefinition)
        {
            return GetFixedBufferAttribute(fieldDefinition) != null;
        }

        public static CustomAttribute GetFixedBufferAttribute(FieldDefinition fieldDefinition)
        {
            if (!fieldDefinition.HasCustomAttributes)
                return null;

            return fieldDefinition.CustomAttributes.SingleOrDefault(a => a.AttributeType.FullName == "System.Runtime.CompilerServices.FixedBufferAttribute");
        }

        public static int GetFixedBufferLength(FieldDefinition fieldDefinition)
        {
            var fixedBufferAttribute = GetFixedBufferAttribute(fieldDefinition);

            if (fixedBufferAttribute == null)
                throw new ArgumentException(string.Format("Field '{0}' is not a fixed buffer field.", fieldDefinition.FullName));

            var size = (Int32)fixedBufferAttribute.ConstructorArguments[1].Value;

            return size;
        }

        public static int PrimitiveTypeSize(TypeReference type)
        {
            switch (type.MetadataType)
            {
                case MetadataType.Boolean:
                case MetadataType.Byte:
                case MetadataType.SByte:
                    return 1;

                case MetadataType.Char:
                case MetadataType.Int16:
                case MetadataType.UInt16:
                    return 2;

                case MetadataType.Int32:
                case MetadataType.UInt32:
                case MetadataType.Single:
                    return 4;

                case MetadataType.Int64:
                case MetadataType.UInt64:
                case MetadataType.Double:
                    return 8;

                default:
                    throw new ArgumentException(string.Format("Unsupported {0}", type.MetadataType));
            }
        }

        private static bool IsSerializablePrimitive(TypeReference typeReference)
        {
            switch (typeReference.MetadataType)
            {
                case MetadataType.SByte:
                case MetadataType.Byte:
                case MetadataType.Char:
                case MetadataType.Int16:
                case MetadataType.UInt16:
                case MetadataType.Int64:
                case MetadataType.UInt64:
                case MetadataType.Int32:
                case MetadataType.UInt32:
                case MetadataType.Single:
                case MetadataType.Double:
                case MetadataType.Boolean:
                case MetadataType.String:
                    return true;
            }
            return false;
        }

        public static bool IsSupportedCollection(TypeReference typeReference)
        {
            if (!(typeReference is ArrayType || CecilUtils.IsGenericList(typeReference)))
                return false;

            // We don't support arrays like byte[,] etc
            if (typeReference.IsArray && ((ArrayType)typeReference).Rank > 1)
                return false;

            return IsTypeSerializable(CecilUtils.ElementTypeOfCollection(typeReference));
        }

        private static bool ShouldHaveHadAllFieldsPublic(FieldDefinition field)
        {
            return UnityEngineTypePredicates.IsUnityEngineValueType(field.DeclaringType);
        }

        private static bool IsUnityEngineObject(TypeReference typeReference)
        {
            return UnityEngineTypePredicates.IsUnityEngineObject(typeReference);
        }

        public static bool IsNonSerialized(TypeReference typeDeclaration)
        {
            if (typeDeclaration == null)
                return true;
            if (typeDeclaration.IsEnum())
                return true;
            if (typeDeclaration.HasGenericParameters)
                return true;
            if (typeDeclaration.MetadataType == MetadataType.Object)
                return true;
            if (typeDeclaration.FullName.StartsWith("System.")) //can this be done better?
                return true;
            if (typeDeclaration.IsArray)
                return true;
            if (typeDeclaration.FullName == UnityEngineTypePredicates.MonoBehaviour)
                return true;
            if (typeDeclaration.FullName == UnityEngineTypePredicates.ScriptableObject)
                return true;
            return false;
        }

        public static bool ShouldImplementIDeserializable(TypeReference typeDeclaration)
        {
            if (typeDeclaration.FullName == "UnityEngine.ExposedReference`1")
                return true;

            if (IsNonSerialized(typeDeclaration))
                return false;

            var genericInstance = typeDeclaration as GenericInstanceType;
            if (genericInstance != null)
            {
                if (genericInstance.ElementType.FullName == "UnityEngine.ExposedReference`1")
                    return true;

                return false;
            }

            try
            {
                return UnityEngineTypePredicates.IsMonoBehaviour(typeDeclaration)
                    || UnityEngineTypePredicates.IsScriptableObject(typeDeclaration)
                    || (typeDeclaration.CheckedResolve().IsSerializable && !typeDeclaration.CheckedResolve().IsAbstract && !typeDeclaration.CheckedResolve().CustomAttributes.Any(a => a.AttributeType.FullName.Contains("System.Runtime.CompilerServices.CompilerGenerated")))
                    || UnityEngineTypePredicates.ShouldHaveHadSerializableAttribute(typeDeclaration);
            }
            catch (Exception)
            {
                return false;
            }
        }
    }
}
