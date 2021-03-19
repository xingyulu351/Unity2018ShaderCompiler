using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Mono.Cecil;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.Core
{
    public enum TypeUsage
    {
        Parameter,
        WritableParameter,
        UnmarshalledParameter,
        ReturnParameter,
        StructThisParameter,
        StructField,
        ArrayElement,
        ReturnType,
        OutParameter
    }

    public enum ScriptingObjectMarshallingType
    {
        Invalid,
        ScriptingObjectPtr,
        UnityEngineObject,
        IntPtrObject,
        Delegate,
        UnmanagedFunctionPointer,
        ObjectAsStruct
    }

    public class TypeUtils
    {
        public static bool IsMethodInternalCall(MethodDefinition methodDefinition)
        {
            if (methodDefinition == null)
                return false;

            if (!methodDefinition.IsExtern())
                return false;

            if (methodDefinition.HasPInvokeInfo)
                return false;

            return true;
        }

        public static bool IsNativePodType(TypeReference typeReference, ScriptingBackend scriptingBackend)
        {
            //These types are not blittable on UWP
            if (scriptingBackend == ScriptingBackend.DotNet &&
                (typeReference.MetadataType == MetadataType.Char ||
                 typeReference.MetadataType == MetadataType.Boolean))
                return false;

            if (typeReference.IsPrimitive)
                return true;

            if (!typeReference.IsValueType)
                return false;

            var typeDefinition = typeReference.Resolve();

            if (typeDefinition.IsEnum)
                return true;

            foreach (var fieldDefinition in typeDefinition.Fields.Where(f => !f.IsStatic))
            {
                if (!IsNativePodType(fieldDefinition.FieldType, scriptingBackend))
                    return false;
            }

            return true;
        }

        public static bool IsMarshalledAsStruct(TypeReference typeReference)
        {
            return IsStruct(typeReference) ||
                GetScriptingObjectMarshallingType(typeReference) == ScriptingObjectMarshallingType.ObjectAsStruct;
        }

        public static bool IsStruct(TypeReference typeReference)
        {
            if (IsBuiltinType(typeReference))
                return false;

            if (typeReference.IsArray)
                return false;

            if (!typeReference.GetElementType().IsValueType)
                return false;

            var typeDefinition = typeReference.Resolve();

            if (typeDefinition.IsEnum)
                return false;

            if (!typeDefinition.IsClass)
                return false;

            return true;
        }

        public static bool IsBuiltinType(TypeReference typeReference)
        {
            // NOTE: System.Void is value type and not a primitive
            // NOTE: We shouldn't use text comparison, but TypeSystem comparison breaks on .NET
            // if (typeReference == typeReference.Module.TypeSystem.Void)
            if (typeReference.FullName == "System.Void")
                return true;

            if (typeReference.IsPrimitive)
                return true;

            return false;
        }

        public static bool IsByValueStruct(TypeReference typeReference)
        {
            return TypeUtils.IsStruct(typeReference) && !typeReference.IsByReference;
        }

        public static bool MethodHasByValueStructParameter(MethodDefinition methodDefinition)
        {
            return methodDefinition.Parameters.Any(p => TypeUtils.IsByValueStruct(p.ParameterType));
        }

        internal static ScriptingObjectMarshallingType GetScriptingObjectMarshallingType(TypeReference typeReference)
        {
            if (typeReference.MetadataType == MetadataType.String)
                return ScriptingObjectMarshallingType.Invalid;

            if (typeReference.IsValueType)
                return ScriptingObjectMarshallingType.Invalid;

            if (typeReference.IsArray)
                return ScriptingObjectMarshallingType.Invalid;

            var typeDefinition = typeReference.Resolve();
            if (typeDefinition == null)
                return ScriptingObjectMarshallingType.Invalid;

            if (typeDefinition.IsDelegate())
            {
                if (typeDefinition.HasAttribute<UnmanagedFunctionPointerAttribute>())
                    return ScriptingObjectMarshallingType.UnmanagedFunctionPointer;
                else
                    return ScriptingObjectMarshallingType.Delegate;
            }

            if (TypeInheritsFromUnityEngineObject(typeDefinition))
                return ScriptingObjectMarshallingType.UnityEngineObject;

            if (TypeIsUnityEngineObjectWithIntPtr(typeDefinition))
                return ScriptingObjectMarshallingType.IntPtrObject;

            if (typeDefinition.IsPrimitive)
                return ScriptingObjectMarshallingType.Invalid;

            if (typeDefinition.IsEnum)
                return ScriptingObjectMarshallingType.Invalid;

            NativeAsStructAttribute nativeAsStruct;
            if (typeDefinition.TryGetAttributeInstance(out nativeAsStruct))
                return ScriptingObjectMarshallingType.ObjectAsStruct;

            if (typeDefinition.IsClass)
                return ScriptingObjectMarshallingType.ScriptingObjectPtr;

            if (typeDefinition.IsInterface)
                return ScriptingObjectMarshallingType.ScriptingObjectPtr;

            return ScriptingObjectMarshallingType.Invalid;
        }

        private static string ResolveTypeNameForFunctionName(TypeDefinition typeDefinition)
        {
            var typeNames = new List<string>();
            var type = typeDefinition;
            while (type != null)
            {
                typeNames.Add(type.Name);
                type = type.DeclaringType;
            }
            typeNames.Reverse();
            var fullDeclaringTypeName = String.Join("_", typeNames);
            return fullDeclaringTypeName;
        }

        public static string ResolveFunctionName(MethodDefinition methodDefinition, bool isStructByrefInjected = false)
        {
            var fullDeclaringTypeName = ResolveTypeNameForFunctionName(methodDefinition.DeclaringType);
            // Getter/setters that are inject will be regular methods, not getter/setters
            // so they should follow regular naming
            if (!isStructByrefInjected)
            {
                if (methodDefinition.IsGetter)
                    return String.Format("{0}_Get_Custom_Prop{1}", fullDeclaringTypeName,
                        methodDefinition.Name.Substring(4).Capitalize());

                if (methodDefinition.IsSetter)
                    return String.Format("{0}_Set_Custom_Prop{1}", fullDeclaringTypeName,
                        methodDefinition.Name.Substring(4).Capitalize());
            }

            if (methodDefinition.IsConstructor)
                return String.Format("{0}_CUSTOM_{0}", fullDeclaringTypeName);

            return String.Format("{0}_CUSTOM_{1}", fullDeclaringTypeName, methodDefinition.Name);
        }

        public static string NativeNameFor(IMemberDefinition memberDefinition, bool useNameProviderAttribute = true)
        {
            if (memberDefinition is FieldDefinition)
            {
                if (memberDefinition.HasAttribute<CompilerGeneratedAttribute>())
                {
                    if (memberDefinition.Name.EndsWith(">k__BackingField"))
                    {
                        var propertyName = memberDefinition.Name.Substring(1, memberDefinition.Name.Length - ">k__BackingField".Length - 1);
                        var propertyDefinition = memberDefinition.DeclaringType.Properties.First(p => p.Name == propertyName);
                        return NativeNameFor(propertyDefinition, useNameProviderAttribute);
                    }
                }
            }

            if (useNameProviderAttribute)
            {
                NativeNameAttribute nameAttribute;
                if (memberDefinition.TryGetAttributeInstance(out nameAttribute))
                    return nameAttribute.Name;

                IBindingsNameProviderAttribute nameProvider;
                if (memberDefinition.TryGetAttributeInstance(out nameProvider))
                    return nameProvider.Name ?? memberDefinition.Name;
            }
            return memberDefinition.Name;
        }

        public static string NativeNameFor(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.IsGetter && !methodDefinition.IsSetter)
                return NativeNameFor(methodDefinition, true);

            NativeNameAttribute nameAttribute;
            if (methodDefinition.TryGetAttributeInstance(out nameAttribute))
                return nameAttribute.Name;

            IBindingsNameProviderAttribute nameProvider;
            if (methodDefinition.TryGetAttributeInstance(out nameProvider) && nameProvider.Name != null)
                return nameProvider.Name;

            if (methodDefinition.IsGetter)
                return $"Get{NativeNameForProperty(methodDefinition)}";

            if (methodDefinition.IsSetter)
                return $"Set{NativeNameForProperty(methodDefinition)}";

            return methodDefinition.Name;
        }

        static string NativeNameForProperty(MethodDefinition methodDefinition)
        {
            var propertyDefinition = methodDefinition.GetMatchingPropertyDefinition();

            NativeNameAttribute nameAttribute;
            if (propertyDefinition.TryGetAttributeInstance(out nameAttribute))
                return nameAttribute.Name;

            IBindingsNameProviderAttribute nameProvider;
            if (propertyDefinition.TryGetAttributeInstance(out nameProvider))
                return nameProvider.Name ?? propertyDefinition.Name.Capitalize();

            return propertyDefinition.Name.Capitalize();
        }

        public static string RegisteredNameFor(TypeReference typeDefinition)
        {
            return Naming.BindingsTypePrefix + typeDefinition.NamespaceAndNestedName().Replace('.', '_');
        }

        public static string ResolveManagedFunctionName(MethodDefinition methodDefinition)
        {
            return methodDefinition.DeclaringType.FullName + "::" + methodDefinition.Name;
        }

        public static string ResolveMarshalledTypeName(TypeReference typeReference, TypeUsage usage, ScriptingBackend scriptingBackend)
        {
            if (typeReference.IsByReference && typeReference.GetElementType().IsPrimitive)
                return "Marshalling::PointerMarshaller".WithTemplateArgument(ResolvePrimitiveNativeTypeName(typeReference.GetElementType()));

            if (typeReference.MetadataType == MetadataType.IntPtr || typeReference.MetadataType == MetadataType.UIntPtr || typeReference.IsPointer)
            {
                switch (usage)
                {
                    case TypeUsage.Parameter: return "Marshalling::IntPtrMarshaller";
                    case TypeUsage.ReturnType: return "Marshalling::IntPtrUnmarshaller";
                }
            }

            if (typeReference.IsPrimitive)
                return ResolvePrimitiveNativeTypeName(typeReference);

            TypeDefinition typeDefinition = typeReference.Resolve();

            if (typeReference.IsArray)
            {
                var elementTypeReference = GetElementType(typeReference);

                string templateArgument = ResolveUnmarshalledTypeName(elementTypeReference, TypeUsage.ArrayElement, scriptingBackend);
                var elementTypeDefinition = elementTypeReference.Resolve();
                //enum MarshalInfo is generated on the intermediate type
                if (!elementTypeReference.IsArray)
                {
                    if (elementTypeDefinition.IsEnum)
                        templateArgument = $"{templateArgument},{IntermediateTypeName(typeDefinition)}";
                    if (elementTypeDefinition.MetadataType == MetadataType.Boolean)
                        templateArgument = $"{templateArgument},bool";
                    if (elementTypeDefinition.MetadataType == MetadataType.Char)
                        templateArgument = $"{templateArgument},Marshalling::ScriptingCharForMarshalInfo";
                }

                switch (usage)
                {
                    case TypeUsage.OutParameter: return "Marshalling::ArrayOutMarshaller".WithTemplateArgument(templateArgument);
                    case TypeUsage.Parameter: return "Marshalling::ArrayMarshaller".WithTemplateArgument(templateArgument);
                    case TypeUsage.ReturnType: return "Marshalling::ArrayUnmarshaller".WithTemplateArgument(templateArgument);
                    default: throw new ArgumentException("Unexpected type usage for arrays : " + usage);
                }
            }

            switch (typeDefinition.FullName)
            {
                case "System.Void": return Naming.VoidType;
                case "System.String":
                {
                    switch (usage)
                    {
                        case TypeUsage.OutParameter:
                        case TypeUsage.Parameter:
                            if (typeReference.IsByReference)
                                return "Marshalling::StringOutMarshaller";
                            else
                                return "Marshalling::StringMarshaller";
                        case TypeUsage.ReturnType: return "Marshalling::StringUnmarshaller";
                        case TypeUsage.UnmarshalledParameter: return "ICallType_String_Argument";
                        default:
                            throw new ArgumentException("Unexpected type usage for string : " + usage);
                    }
                }
                case "System.Array":
                {
                    if (typeDefinition.IsClass)
                    {
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "Marshalling::SystemArrayMarshaller";
                            case TypeUsage.ReturnType: return "Marshalling::SystemArrayUnmarshaller";
                            default: throw new ArgumentException("Unexpected type usage for System.Array : " + usage);
                        }
                    }
                    break;
                }
                case "System.Type":
                {
                    switch (usage)
                    {
                        case TypeUsage.Parameter: return "Marshalling::SystemTypeMarshaller";
                        case TypeUsage.ReturnType: return "Marshalling::SystemTypeUnmarshaller";
                        default: throw new ArgumentException("Unexpected type usage for System.Type : " + usage);
                    }
                }
            }

            if (typeDefinition.IsValueType)
            {
                if (typeReference.IsByReference && RequireMarshalling(typeReference, scriptingBackend) || usage == TypeUsage.StructThisParameter)
                {
                    var outMarshallerType = typeDefinition.NativeProxy() ?? IntermediateTypeName(typeDefinition);
                    return $"Marshalling::OutMarshaller<{outMarshallerType}, {RegisteredNameFor(typeDefinition)}>";
                }

                return RegisteredNameFor(typeDefinition);
            }

            if (typeDefinition.IsClass)
            {
                var nativeClassName = RegisteredNameFor(typeDefinition);
                if (usage == TypeUsage.UnmarshalledParameter)
                    return "Marshalling::ManagedObjectMarshaller";

                switch (GetScriptingObjectMarshallingType(typeDefinition))
                {
                    case ScriptingObjectMarshallingType.UnityEngineObject:
                        nativeClassName = RegisteredNameFor(MarshallingType(typeDefinition));
                        switch (usage)
                        {
                            case TypeUsage.Parameter:                 return "Marshalling::ReadOnlyUnityObjectMarshaller".WithTemplateArgument(nativeClassName);
                            case TypeUsage.WritableParameter:         return "Marshalling::UnityObjectMarshaller".WithTemplateArgument(nativeClassName);
                            case TypeUsage.ReturnType:                return "Marshalling::UnityObjectUnmarshaller".WithTemplateArgument(nativeClassName);
                            default: throw new ArgumentException("Unexpected type usage for unity object : " + usage);
                        }

                    case ScriptingObjectMarshallingType.IntPtrObject:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "Marshalling::IntPtrObjectMarshaller".WithTemplateArgument(nativeClassName);
                            case TypeUsage.ReturnType: return "Marshalling::IntPtrObjectUnmarshaller".WithTemplateArgument(nativeClassName);
                            default: throw new ArgumentException("Unexpected type usage for int ptr object : " + usage);
                        }

                    case ScriptingObjectMarshallingType.ScriptingObjectPtr:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "Marshalling::ManagedObjectMarshaller";
                            case TypeUsage.ReturnType: return "Marshalling::ManagedObjectUnmarshaller";
                            default: throw new ArgumentException("Unexpected type usage for managed object : " + usage);
                        }
                    case ScriptingObjectMarshallingType.Delegate:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "Marshalling::ManagedObjectMarshaller";
                            default: throw new ArgumentException($"Unexpected type usage for delegate {typeDefinition} : {usage}");
                        }
                    case ScriptingObjectMarshallingType.UnmanagedFunctionPointer:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "Marshalling::UntypedPtrMarshaller"; // We treat the function pointer as an untyped pointer
                            default: throw new ArgumentException($"Unexpected type usage for delegate {typeDefinition} : {usage}");
                        }
                    case ScriptingObjectMarshallingType.ObjectAsStruct:
                        switch (usage)
                        {
                            case TypeUsage.OutParameter:
                                return $"Marshalling::OutMarshaller<{ResolveUnmarshalledTypeName(typeReference, TypeUsage.OutParameter, scriptingBackend)}, {RegisteredNameFor(typeDefinition)}>";
                            default: return RegisteredNameFor(typeDefinition);
                        }
                }
            }

            if (typeDefinition.IsInterface)
            {
                switch (GetScriptingObjectMarshallingType(typeDefinition))
                {
                    case ScriptingObjectMarshallingType.ScriptingObjectPtr:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "Marshalling::ManagedObjectMarshaller";
                            case TypeUsage.ReturnType: return "Marshalling::ManagedObjectUnmarshaller";
                            default: throw new ArgumentException("Unexpected type usage for managed object : " + usage);
                        }
                }
            }

            if (typeDefinition.IsInterface)
            {
                switch (GetScriptingObjectMarshallingType(typeDefinition))
                {
                    case ScriptingObjectMarshallingType.ScriptingObjectPtr:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "Marshalling::ManagedObjectMarshaller";
                            case TypeUsage.ReturnType: return "Marshalling::ManagedObjectUnmarshaller";
                            default: throw new ArgumentException("Unexpected type usage for managed object : " + usage);
                        }
                }
            }

            throw new NotImplementedException(String.Format("Don't know how to get unmarshalled type name for '{0}'", typeReference.FullName));
        }

        public static TypeReference MarshallingType(TypeDefinition typeDefinition)
        {
            var marshalUnityObjectAsAttribute = typeDefinition.CustomAttributes
                .GetAttributeInstances(typeof(MarshalUnityObjectAs), false).FirstOrDefault();

            if (marshalUnityObjectAsAttribute != null)
            {
                return (TypeReference)marshalUnityObjectAsAttribute.ConstructorArguments[0].Value;
            }

            return typeDefinition;
        }

        public static string ResolveUnmarshalledTypeName(TypeReference typeReference, TypeUsage usage, ScriptingBackend scriptingBackend, bool allowProxyStructs = true)
        {
            if (typeReference.IsByReference && typeReference.GetElementType().IsPrimitive)
                return ResolvePrimitiveNativeTypeName(typeReference.GetElementType()).Pointer();

            if (typeReference.MetadataType == MetadataType.IntPtr || typeReference.MetadataType == MetadataType.UIntPtr || typeReference.IsPointer)
                return "ICallType_IntPtr_Argument";

            if (typeReference.IsPrimitive)
                return ResolvePrimitiveNativeTypeName(typeReference);

            if (typeReference.IsArray)
            {
                var elementTypeReference = GetElementType(typeReference);

                string fieldTemplateArguments = ResolveUnmarshalledTypeName(elementTypeReference, TypeUsage.ArrayElement, scriptingBackend);
                var elementTypeDefinition = elementTypeReference.Resolve();
                if (!elementTypeReference.IsArray && elementTypeDefinition.IsEnum)
                    fieldTemplateArguments = $"{fieldTemplateArguments},{IntermediateTypeName(elementTypeDefinition)}";

                switch (usage)
                {
                    case TypeUsage.OutParameter: return "ICallType_Array_Argument";
                    case TypeUsage.Parameter: return "ICallType_Array_Argument";
                    case TypeUsage.ReturnType: return "ICallType_Array_Return";
                    case TypeUsage.StructField: return "Marshalling::ArrayStructField".WithTemplateArgument(fieldTemplateArguments);
                    case TypeUsage.ArrayElement: return "Marshalling::ArrayArrayElement".WithTemplateArgument(fieldTemplateArguments);
                    default: throw new ArgumentException("Unexpected type usage for arrays : " + usage);
                }
            }

            var typeDefinition = typeReference.Resolve();

            if (typeDefinition.IsDelegate())
            {
                if (typeDefinition.HasAttribute<UnmanagedFunctionPointerAttribute>())
                    return "void*"; // We treat the function pointer as an untyped pointer
                else
                {
                    switch (usage)
                    {
                        case TypeUsage.ArrayElement: return "Marshalling::ManagedObjectArrayElement";
                        default: return "ICallType_Object_Argument";
                    }
                }
            }

            switch (typeDefinition.FullName)
            {
                case "System.Void": return Naming.VoidType;
                case "System.String":
                {
                    switch (usage)
                    {
                        case TypeUsage.OutParameter:
                            if (typeReference.IsByReference)
                                return "ICallType_String_Argument_Out";
                            else
                                throw new ArgumentException("[Out] is not supported on strings not passed by ref or out.");
                        case TypeUsage.Parameter:
                        case TypeUsage.UnmarshalledParameter:
                            if (typeReference.IsByReference)
                            {
                                return "ICallType_String_Argument_Ref";
                            }
                            else
                            {
                                return "ICallType_String_Argument";
                            }
                        case TypeUsage.ReturnType: return "ICallType_String_Return";
                        case TypeUsage.StructField: return "Marshalling::StringStructField";
                        case TypeUsage.ArrayElement: return "Marshalling::StringArrayElement";
                        default: throw new ArgumentException("Unexpected type usage for string : " + usage);
                    }
                }
                case "System.Array":
                {
                    if (typeDefinition.IsClass)
                    {
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "ICallType_Array_Argument";
                            case TypeUsage.ReturnType: return "ICallType_Array_Return";
                            default: throw new ArgumentException("Unexpected type usage for System.Array : " + usage);
                        }
                    }
                    break;
                }
                case "System.Type":
                {
                    switch (usage)
                    {
                        case TypeUsage.Parameter: return "ICallType_SystemTypeObject_Argument";
                        case TypeUsage.ReturnType: return "ICallType_SystemTypeObject_Return";
                        case TypeUsage.ArrayElement: return "Marshalling::SystemTypeObjectArrayElement";
                        case TypeUsage.StructField: return "Marshalling::SystemTypeObjectStructField";
                        default: throw new ArgumentException("Unexpected type usage for System.Type : " + usage);
                    }
                }
            }

            if (usage != TypeUsage.ReturnType || allowProxyStructs)
            {
                var name = typeDefinition.NativeProxy();
                if (name != null)
                {
                    if (usage == TypeUsage.ReturnParameter || usage == TypeUsage.StructThisParameter || usage == TypeUsage.Parameter || usage == TypeUsage.WritableParameter || usage == TypeUsage.OutParameter)
                    {
                        if (typeDefinition.IsValueType)
                            return name + "&";
                        if (allowProxyStructs)
                            return name;
                    }
                    else
                        return name;
                }
            }

            if (typeDefinition.IsValueType)
            {
                var typeName = RegisteredNameFor(typeDefinition);
                if (typeDefinition.IsEnum)
                {
                    if (typeReference.IsByReference)
                        return Naming.Pointer(typeName);

                    return typeName;
                }

                //force use of intermediate struct in arrays so we can have access to MarshalInfo<T>
                if (RequireMarshalling(typeReference, scriptingBackend) || usage == TypeUsage.ArrayElement)
                    typeName = IntermediateTypeName(typeDefinition);

                switch (usage)
                {
                    case TypeUsage.Parameter:
                    case TypeUsage.OutParameter:
                        if (!typeReference.IsByReference)
                            typeName = Naming.ConstReference(typeName);
                        else
                            typeName = Naming.Reference(typeName);
                        break;
                    case TypeUsage.ReturnParameter:
                        typeName = Naming.Reference(typeName);
                        break;
                    case TypeUsage.StructThisParameter:
                        typeName = Naming.Pointer(typeName);
                        break;
                    case TypeUsage.ReturnType:
                    case TypeUsage.ArrayElement:
                    case TypeUsage.StructField:
                        break;
                    default: throw new ArgumentException($"Unexpected type usage for {typeReference}: {usage}");
                }

                return typeName;
            }

            if (typeDefinition.IsClass)
            {
                var nativeClassName = RegisteredNameFor(typeDefinition);
                if (usage == TypeUsage.UnmarshalledParameter)
                    return "ICallType_Object_Argument";

                switch (GetScriptingObjectMarshallingType(typeDefinition))
                {
                    case ScriptingObjectMarshallingType.UnityEngineObject:
                    {
                        nativeClassName = RegisteredNameFor(MarshallingType(typeDefinition));
                    }
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "ICallType_ReadOnlyUnityEngineObject_Argument";
                            case TypeUsage.WritableParameter: return "ICallType_Object_Argument";
                            case TypeUsage.ReturnType: return "Marshalling::UnityObjectReturnValue";
                            case TypeUsage.StructField: return "Marshalling::UnityObjectStructField".WithTemplateArgument(nativeClassName);
                            case TypeUsage.ArrayElement: return "Marshalling::UnityObjectArrayElement".WithTemplateArgument(nativeClassName);
                            default: throw new ArgumentException("Unexpected type usage for unity object : " + usage);
                        }

                    case ScriptingObjectMarshallingType.IntPtrObject:
                        switch (usage)
                        {
                            // NOTE : cannot return an intptr object
                            case TypeUsage.Parameter: return "ICallType_Object_Argument";
                            case TypeUsage.StructField: return "Marshalling::IntPtrObjectStructField".WithTemplateArgument(nativeClassName);
                            case TypeUsage.ArrayElement: return "Marshalling::IntPtrObjectArrayElement".WithTemplateArgument(nativeClassName);
                            case TypeUsage.ReturnType: return "Marshalling::IntPtrObjectReturnValue";
                            default: throw new ArgumentException("Unexpected type usage for int ptr object : " + usage);
                        }

                    case ScriptingObjectMarshallingType.ScriptingObjectPtr:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "ICallType_Object_Argument";
                            case TypeUsage.ReturnType: return "ICallType_Object_Return";
                            case TypeUsage.StructField:
                            {
                                if (RequireMarshallingAsStruct(typeDefinition))
                                    return Naming.IntermediateTypeName(typeDefinition.Name);

                                return "Marshalling::ManagedObjectStructField";
                            }
                            case TypeUsage.ArrayElement: return "Marshalling::ManagedObjectArrayElement";
                            default: throw new ArgumentException("Unexpected type usage for managed object : " + usage);
                        }
                    case ScriptingObjectMarshallingType.ObjectAsStruct:
                    {
                        switch (usage)
                        {
                            case TypeUsage.Parameter:
                            case TypeUsage.OutParameter:
                            case TypeUsage.StructField:
                            case TypeUsage.ArrayElement:
                            case TypeUsage.ReturnType:
                                if (typeDefinition.NativeProxy() != null)
                                    return "ICallType_Object_Argument";
                                return "Marshalling::ManagedObjectForStruct".WithTemplateArgument(IntermediateTypeName(typeDefinition));
                            default: throw new ArgumentException("Unexpected type usage for managed object : " + usage);
                        }
                    }
                }
            }

            if (typeDefinition.IsInterface)
            {
                switch (GetScriptingObjectMarshallingType(typeDefinition))
                {
                    case ScriptingObjectMarshallingType.ScriptingObjectPtr:
                        switch (usage)
                        {
                            case TypeUsage.Parameter: return "ICallType_Object_Argument";
                            case TypeUsage.ReturnType: return "ICallType_Object_Return";
                            case TypeUsage.StructField:
                            {
                                if (RequireMarshallingAsStruct(typeDefinition))
                                    return IntermediateTypeName(typeDefinition);

                                return "Marshalling::ManagedObjectStructField";
                            }
                            case TypeUsage.ArrayElement: return "Marshalling::ManagedObjectArrayElement";
                            default: throw new ArgumentException("Unexpected type usage for managed object : " + usage);
                        }
                }
            }


            throw new NotImplementedException(String.Format("Don't know how to get marshalled type name for '{0}'", typeReference.FullName));
        }

        public static TypeReference GetElementType(TypeReference typeReference)
        {
            TypeReference elementType;
            var specification = typeReference as TypeSpecification;
            if (specification != null)
                elementType = specification.ElementType;
            else
                throw new ArgumentException("Unsupported TypeReference");

            return elementType;
        }

        public static string IntermediateTypeName(TypeDefinition typeDefinition)
        {
            NativeTypeAttribute provider;
            typeDefinition.TryGetAttributeInstance(out provider);

            bool hasIntermediateStructNameInAttribute =
                provider != null &&
                provider.CodegenOptions == CodegenOptions.Custom && provider.IntermediateScriptingStructName != null;

            var name = hasIntermediateStructNameInAttribute
                ? provider.IntermediateScriptingStructName
                : Naming.IntermediateTypeName(typeDefinition.Name);
            return name;
        }

        public static string ResolvePrimitiveNativeTypeName(TypeReference typeReference)
        {
            var metaType = typeReference.MetadataType;
            if (metaType == MetadataType.UIntPtr || metaType == MetadataType.IntPtr || typeReference.IsPointer)
                return "ICallType_IntPtr_Argument";

            switch (metaType)
            {
                case MetadataType.Boolean:      return "ScriptingBool";
                case MetadataType.Char:         return "SInt16";

                case MetadataType.SByte:        return "SInt8";
                case MetadataType.Int16:        return "SInt16";
                case MetadataType.Int32:        return "SInt32";
                case MetadataType.Int64:        return "SInt64";

                case MetadataType.Byte:         return "UInt8";
                case MetadataType.UInt16:       return "UInt16";
                case MetadataType.UInt32:       return "UInt32";
                case MetadataType.UInt64:       return "UInt64";

                case MetadataType.Single:       return "float";
                case MetadataType.Double:       return "double";

                default:
                    throw new ArgumentException("Unhandled primitive type");
            }
        }

        public static bool RequireMarshalling(TypeReference typeReference, ScriptingBackend scriptingBackend)
        {
            var typeDefinition = typeReference.Resolve();
            if (!IsStruct(typeReference) &&
                GetScriptingObjectMarshallingType(typeDefinition) != ScriptingObjectMarshallingType.ObjectAsStruct)
                return false;

            if (ForcesMarshalling(typeDefinition))
                return true;

            return !IsNativePodType(typeDefinition, scriptingBackend);
        }

        public static bool RequireMarshallingAsStruct(TypeReference typeReference)
        {
            return ForcesMarshalling(typeReference.Resolve());
        }

        private static bool ForcesMarshalling(ICustomAttributeProvider customAttributeProvider)
        {
            // Allow users to force the generation of the use/generation of intermediate struct

            IBindingsGenerateMarshallingTypeAttribute attr;
            if (!customAttributeProvider.TryGetAttributeInstance(out attr))
                return false;

            return attr.CodegenOptions == CodegenOptions.Force || attr.CodegenOptions == CodegenOptions.Custom;
        }

        public static bool RequiresMarshalling(TypeReference type, TypeUsage usage, ScriptingBackend scriptingBackend)
        {
            TypeDefinition typeDefinition = type.Resolve();

            if (typeDefinition.MetadataType == MetadataType.IntPtr || typeDefinition.MetadataType == MetadataType.UIntPtr || type.IsPointer)
                return true; // these require marshalling due to ABI issues on android

            if (type.IsArray)
                return true;

            if (type.IsByReference)
            {
                if (type.GetElementType().IsPrimitive)
                    return true; // for primitives at least, these are mapped to PointerMarshaller
                else
                    return RequiresMarshalling(type.GetElementType(), usage, scriptingBackend);
            }

            if (!typeDefinition.IsValueType)
                return true;

            if (!IsNativePodType(typeDefinition, scriptingBackend))
                return true;

            if (ForcesMarshalling(typeDefinition))
                return true;

            return false;
        }

        public static bool CouldBeNull(TypeReference typeReference)
        {
            if (typeReference.IsValueType)
                return false;

            if (typeReference is ArrayType)
                return true;

            if (typeReference is ByReferenceType)
                return CouldBeNull(typeReference.GetElementType());

            var typeDefinition = typeReference.Resolve();
            return !typeDefinition.IsValueType;
        }

        public static string NullCheckFor(string name, TypeUsage usage, ScriptingObjectMarshallingType type)
        {
            if (usage == TypeUsage.UnmarshalledParameter)
                return String.Format("{0} == SCRIPTING_NULL", name.Marshalled());

            if (type == ScriptingObjectMarshallingType.ObjectAsStruct)
                return String.Format("{0}.m_Data == SCRIPTING_NULL", name);

            return String.Format("{0}.IsNull()", name.Marshalled());
        }

        public static bool TypeInheritsFrom(TypeReference typeReference, string @namespace, string @class)
        {
            var typeDefinition = typeReference.Resolve();
            if (typeDefinition == null)
                return false;

            if (typeDefinition.Name == @class && typeDefinition.Namespace == @namespace)
                return true;

            if (typeDefinition.BaseType != null)
                return TypeInheritsFrom(typeDefinition.BaseType, @namespace, @class);

            return false;
        }

        public static bool TypeInheritsFromUnityEngineObject(TypeReference typeReference)
        {
            return TypeInheritsFrom(typeReference, "UnityEngine", "Object");
        }

        public static bool TypeIsUnityEngineObjectWithIntPtr(TypeReference typeReference)
        {
            var typeDefinition = typeReference.Resolve();
            if (typeDefinition == null || TypeInheritsFromUnityEngineObject(typeReference))
                return false;

            if (HasIntPtrField(typeDefinition) && (typeDefinition.BaseType == null || typeDefinition.BaseType.MetadataType == MetadataType.Object || HasNoFields(typeDefinition.BaseType)))
                return true;

            if (typeDefinition.BaseType != null)
                return TypeIsUnityEngineObjectWithIntPtr(typeDefinition.BaseType.Resolve());

            return false;
        }

        public static bool HasIntPtrField(TypeReference typeReference)
        {
            var typeDefinition = typeReference.Resolve();
            var typeDefinitionFields = typeDefinition.Fields.Where(f => !f.IsStatic).ToArray();
            return typeDefinitionFields.Length > 0 && typeDefinitionFields[0].FieldType.MetadataType == MetadataType.IntPtr;
        }

        public static bool IsExternInstanceMethodInStruct(MethodDefinition methodDefinition, ScriptingBackend scriptingBackend)
        {
            if (!methodDefinition.MayRequireBindingsGeneration())
                return false;

            if (methodDefinition.IsStatic)
                return false;

            if (!IsStruct(methodDefinition.DeclaringType))
                return false;

            return true;
        }

        private static bool HasNoFields(TypeReference typeReference)
        {
            if (typeReference == null)
                return true;

            var resolved = typeReference.Resolve();
            if (resolved.Fields.Count > 0)
                return false;

            return HasNoFields(resolved.BaseType);
        }
    }
}
