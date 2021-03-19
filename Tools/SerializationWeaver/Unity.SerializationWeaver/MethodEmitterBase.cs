using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Rocks;
using Unity.CecilTools;
using Unity.SerializationLogic;
using MethodAttributes = Mono.Cecil.MethodAttributes;
using MethodBody = Mono.Cecil.Cil.MethodBody;
using ParameterAttributes = Mono.Cecil.ParameterAttributes;

namespace Unity.Serialization.Weaver
{
    public abstract class MethodEmitterBase
    {
        protected const string AlignMethodName = "Align";
        private const string CountPropertyName = "get_Count";
        protected const string ConstructorMethodName = ".ctor";
        private const string GetTypeFromHandleMethodName = "GetTypeFromHandle";
        private const int DepthLimit = 7; // same value as kClassSerializationDepthLimit in the editor

        private LocalVariable _itemsIndex;
        private readonly TypeDefinition _typeDef;
        protected readonly SerializationBridgeProvider _serializationBridgeProvider;
        private LocalVariablesGenerator _variablesGenerator;
        protected TypeResolver TypeResolver = new TypeResolver(null);

        protected MethodEmitterBase(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider)
        {
            _typeDef = typeDef;
            _serializationBridgeProvider = serializationBridgeProvider;
        }

        protected void CreateMethodDef(string methodName)
        {
            MethodDefinition = new MethodDefinition(methodName, MethodAttributes.Public | MethodAttributes.Virtual | MethodAttributes.ReuseSlot, VoidTypeRef());

            foreach (var param in MethodDefParameters())
                MethodDefinition.Parameters.Add(param);

            MethodDefinition.Body = new MethodBody(MethodDefinition);
            Processor = MethodDefinition.Body.GetILProcessor();

            _variablesGenerator = new LocalVariablesGenerator(MethodDefinition, Processor);

            EmitMethodBody();
            MethodDefinition.Body.OptimizeMacros();
        }

        protected abstract ParameterDefinition[] MethodDefParameters();

        protected LocalVariable ItemsIndexVar
        {
            get { return _itemsIndex ?? (_itemsIndex = DefineLocal(Import(typeof(int)))); }
        }

        protected LocalVariable DefineLocal(TypeReference type)
        {
            return _variablesGenerator.Create(type);
        }

        protected ModuleDefinition Module
        {
            get { return TypeDef.Module; }
        }

        protected TypeDefinition TypeDef
        {
            get { return _typeDef; }
        }

        protected MethodDefinition MethodDefinition { get; set; }

        protected ILProcessor Processor { get; set; }

        private void EmitMethodBody()
        {
            var baseTypes = new Stack<TypeReference>();
            var lastBaseType = TypeDef.BaseType;

            TypeResolver = new TypeResolver(null);

            InjectBeforeSerialize();

            while (!UnitySerializationLogic.IsNonSerialized(lastBaseType))
            {
                var genericInstanceType = lastBaseType as GenericInstanceType;
                if (genericInstanceType != null)
                    TypeResolver.Add(genericInstanceType);

                baseTypes.Push(lastBaseType);
                lastBaseType = lastBaseType.Resolve().BaseType;
            }

            while (baseTypes.Count > 0)
            {
                var typeReference = baseTypes.Pop();
                var typeDefinition = typeReference.Resolve();

                foreach (var fieldDefinition in typeDefinition.Fields.Where(ShouldProcess))
                {
                    EmitInstructionsFor(ResolveGenericFieldReference(fieldDefinition));

                    if (!(fieldDefinition.IsPublic || fieldDefinition.IsFamily))
                    {
                        fieldDefinition.IsPrivate = false;
                        fieldDefinition.IsFamilyOrAssembly = true;
                    }
                }

                var genericInstanceType = typeReference as GenericInstanceType;
                if (genericInstanceType != null)
                    TypeResolver.Remove(genericInstanceType);
            }

            foreach (var field in FilteredFields())
            {
                EmitInstructionsFor(field);

                if (!(field.IsPublic || field.IsFamily))
                {
                    field.IsPrivate = false;
                    field.IsFamilyOrAssembly = true;
                }
            }

            InjectAfterDeserialize();

            MethodBodySuffix();

            MethodDefinition.Body.OptimizeMacros();
        }

        protected bool IfTypeImplementsInterface(string interfaceName)
        {
            return InterfacesOf(_typeDef).FirstOrDefault(i => i.FullName == interfaceName) != null;
        }

        protected void InvokeMethodIfTypeImplementsInterface(string interfaceName, string methodName)
        {
            var interfaceRef = InterfacesOf(_typeDef).FirstOrDefault(i => i.FullName == interfaceName);
            if (interfaceRef != null)
            {
                if (_typeDef.IsValueType)
                {
                    Processor.Emit(OpCodes.Ldarg_0);
                    Call(MethodRefFor(_typeDef, methodName));
                }
                else
                {
                    Processor.Emit(OpCodes.Ldarg_0);
                    Processor.Emit(OpCodes.Castclass, Module.ImportReference(interfaceRef));
                    Callvirt(MethodRefFor(interfaceRef.Resolve(), methodName, true));
                }
            }
        }

        protected void InvokeSerializationCallbackMethod(string methodName)
        {
            if (IfTypeImplementsInterface("UnityEngine.ISerializationCallbackReceiver"))
            {
                Ldarg_2();
                var endLabel = DefineLabel();
                Brtrue_S(endLabel);
                InvokeMethodIfTypeImplementsInterface("UnityEngine.ISerializationCallbackReceiver", methodName);
                MarkLabel(endLabel);
            }
        }

        protected static IEnumerable<TypeReference> InterfacesOf(TypeDefinition typeDefinition)
        {
            var current = typeDefinition;
            while (current != null)
            {
                foreach (var interfaceImpl in current.Interfaces)
                    yield return interfaceImpl.InterfaceType;

                current = current.BaseType != null ? current.BaseType.Resolve() : null;
            }
        }

        protected abstract void InjectBeforeSerialize();

        protected abstract void InjectAfterDeserialize();

        protected bool WillUnitySerialize(FieldDefinition fieldDefinition)
        {
            try
            {
                var resolvedFieldType = TypeResolver.Resolve(fieldDefinition.FieldType);

                if (UnitySerializationLogic.ShouldNotTryToResolve(resolvedFieldType))
                    return false;

                if (!UnityEngineTypePredicates.IsUnityEngineObject(resolvedFieldType))
                {
                    // See test Tests\Unity.RuntimeTests\Serialization\WontSerializeFieldOfOwnType.cs
                    if (resolvedFieldType.FullName == fieldDefinition.DeclaringType.FullName)
                        return false;
                }

                return UnitySerializationLogic.WillUnitySerialize(fieldDefinition, TypeResolver);
            }
            catch (Exception ex)
            {
                throw new Exception(string.Format("Exception while processing {0} {1}, error {2}", fieldDefinition.FieldType.FullName, fieldDefinition.FullName, ex.Message));
            }
        }

        private IEnumerable<FieldDefinition> FilteredFields()
        {
            return TypeDef.Fields.Where(ShouldProcess).Where(f =>
                UnitySerializationLogic.IsSupportedCollection(f.FieldType) ||
                !f.FieldType.IsGenericInstance ||
                UnitySerializationLogic.ShouldImplementIDeserializable(f.FieldType.Resolve()));
        }

        protected abstract bool ShouldProcess(FieldDefinition fieldDefinition);

        protected abstract void EmitInstructionsFor(FieldReference field);

        protected void MethodBodySuffix()
        {
            Ret();
        }

        public void EmitLoadField(FieldReference fieldDef)
        {
            var typeRef = TypeOf(fieldDef);

            Ldarg_0();

            if (IsStructByRef(typeRef))
                Ldflda(fieldDef);
            else
                Ldfld(fieldDef);
        }

        protected void EmitStoreField(FieldReference fieldDef)
        {
            Processor.Emit(OpCodes.Stfld, ResolveGenericFieldReference(fieldDef));
        }

        protected void Ldarg_0()
        {
            Emit(OpCodes.Ldarg_0);
        }

        protected static bool IsStruct(TypeReference typeRef)
        {
            if (!typeRef.IsValueType)
                return false;

            if (IsEnum(typeRef))
                return false;

            return !typeRef.IsPrimitive;
        }

        protected static bool IsStructByRef(TypeReference typeRef)
        {
            if (!IsStruct(typeRef))
                return false;

            switch (typeRef.FullName)
            {
                case "UnityEngine.Color32":
                    return false;
                default:
                    return true;
            }
        }

        protected static bool IsEnum(TypeReference typeRef)
        {
            return (!typeRef.IsArray && typeRef.Resolve().IsEnum);
        }

        protected TypeReference TypeOf(FieldReference fieldDef)
        {
            return Module.ImportReference(TypeResolver.Resolve(fieldDef.FieldType));
        }

        protected void Ldfld(FieldReference fieldRef)
        {
            Processor.Emit(OpCodes.Ldfld, ResolveGenericFieldReference(fieldRef));
        }

        public void Ldflda(FieldReference fieldRef)
        {
            Processor.Emit(OpCodes.Ldflda, ResolveGenericFieldReference(fieldRef));
        }

        protected void Ldsfld(FieldReference fieldRef)
        {
            Processor.Emit(OpCodes.Ldsfld, ResolveGenericFieldReference(fieldRef));
        }

        protected void Ldsflda(FieldReference fieldRef)
        {
            Processor.Emit(OpCodes.Ldsflda, ResolveGenericFieldReference(fieldRef));
        }

        protected FieldReference ResolveGenericFieldReference(FieldReference fieldRef)
        {
            var field = new FieldReference(fieldRef.Name, fieldRef.FieldType, ResolveDeclaringType(fieldRef.DeclaringType));
            return Module.ImportReference(field);
        }

        private TypeReference ResolveDeclaringType(TypeReference declaringType)
        {
            var typeDefinition = declaringType.Resolve();
            if (typeDefinition == null || !typeDefinition.HasGenericParameters)
                return typeDefinition;

            var genericInstanceType = new GenericInstanceType(typeDefinition);
            foreach (var genericParameter in typeDefinition.GenericParameters)
                genericInstanceType.GenericArguments.Add(genericParameter);

            return TypeResolver.Resolve(genericInstanceType);
        }

        protected void EmitLengthOf(FieldReference fieldDef)
        {
            EmitLoadField(fieldDef);

            if (TypeOf(fieldDef).IsArray)
                Ldlen();
            else
                Call(CountMethodFor(TypeOf(fieldDef)));
        }

        private void Ldlen()
        {
            Processor.Emit(OpCodes.Ldlen);
        }

        private MethodReference CountMethodFor(TypeReference typeReference)
        {
            return new MethodReference(CountPropertyName, Import(typeof(int)), TypeReferenceFor(typeReference))
            {
                HasThis = true
            };
        }

        protected void Newobj(MethodReference ctor)
        {
            Processor.Emit(OpCodes.Newobj, ctor);
        }

        protected void Conv_I()
        {
            Processor.Emit(OpCodes.Conv_I);
        }

        protected void Conv_I4()
        {
            Processor.Emit(OpCodes.Conv_I4);
        }

        protected void EmitTypeOf(TypeReference typeRef)
        {
            Ldtoken(typeRef);
            Call(typeof(Type), GetTypeFromHandleMethodName);
        }

        private void Ldtoken(TypeReference typeRef)
        {
            Processor.Emit(OpCodes.Ldtoken, typeRef);
        }

        protected void Ldnull()
        {
            Processor.Emit(OpCodes.Ldnull);
        }

        protected void Ret()
        {
            Emit(OpCodes.Ret);
        }

        protected void Pop()
        {
            Emit(OpCodes.Pop);
        }

        private void Call(Type type, string name)
        {
            Processor.Emit(OpCodes.Call, MethodRefFor(type, name));
        }

        protected void Callvirt(Type type, string name)
        {
            Processor.Emit(OpCodes.Callvirt, MethodRefFor(type, name));
        }

        protected void Callvirt(TypeDefinition typeDefinition, string name)
        {
            Processor.Emit(OpCodes.Callvirt, MethodRefFor(typeDefinition, name));
        }

        protected void Ldarg_1()
        {
            Emit(OpCodes.Ldarg_1);
        }

        protected void Ldarg_2()
        {
            Emit(OpCodes.Ldarg_2);
        }

        protected void Starg(ushort value)
        {
            if (value <= byte.MaxValue)
                Processor.Emit(OpCodes.Starg_S, (byte)value);
            else
                Processor.Emit(OpCodes.Starg, value);
        }

        protected void Stind(TypeReference typeRef)
        {
            switch (typeRef.MetadataType)
            {
                case MetadataType.Boolean:
                case MetadataType.SByte:
                case MetadataType.Byte:
                    Processor.Emit(OpCodes.Stind_I1);
                    break;
                case MetadataType.Int16:
                case MetadataType.UInt16:
                case MetadataType.Char:
                    Processor.Emit(OpCodes.Stind_I2);
                    break;
                case MetadataType.Int32:
                case MetadataType.UInt32:
                    Processor.Emit(OpCodes.Stind_I4);
                    break;
                case MetadataType.Int64:
                case MetadataType.UInt64:
                    Processor.Emit(OpCodes.Stind_I8);
                    break;
                case MetadataType.Single:
                    Processor.Emit(OpCodes.Stind_R4);
                    break;
                case MetadataType.Double:
                    Processor.Emit(OpCodes.Stind_R8);
                    break;
                default:
                    throw new ArgumentException(string.Format("Unsupported type {0}", typeRef.MetadataType));
            }
        }

        protected void Ldind(TypeReference typeRef)
        {
            switch (typeRef.MetadataType)
            {
                case MetadataType.SByte:
                    Processor.Emit(OpCodes.Ldind_I1);
                    break;
                case MetadataType.Boolean:
                case MetadataType.Byte:
                    Processor.Emit(OpCodes.Ldind_U1);
                    break;
                case MetadataType.Int16:
                    Processor.Emit(OpCodes.Ldind_I2);
                    break;
                case MetadataType.Char:
                case MetadataType.UInt16:
                    Processor.Emit(OpCodes.Ldind_U2);
                    break;
                case MetadataType.Int32:
                    Processor.Emit(OpCodes.Ldind_I4);
                    break;
                case MetadataType.UInt32:
                    Processor.Emit(OpCodes.Ldind_U4);
                    break;
                case MetadataType.Int64:
                    Processor.Emit(OpCodes.Ldind_I8);
                    break;
                case MetadataType.UInt64:
                    Processor.Emit(OpCodes.Ldind_I8); // There is no ldind.u8?
                    break;
                case MetadataType.Single:
                    Processor.Emit(OpCodes.Ldind_R4);
                    break;
                case MetadataType.Double:
                    Processor.Emit(OpCodes.Ldind_R8);
                    break;
                default:
                    throw new ArgumentException(string.Format("Unsupported type {0}", typeRef.MetadataType));
            }
        }

        protected static bool RequiresAlignment(TypeReference typeRef)
        {
            switch (typeRef.MetadataType)
            {
                case MetadataType.Boolean:
                case MetadataType.Char:
                case MetadataType.SByte:
                case MetadataType.Byte:
                case MetadataType.Int16:
                case MetadataType.UInt16:
                    return true;

                default:
                    return UnitySerializationLogic.IsSupportedCollection(typeRef) && RequiresAlignment(CecilUtils.ElementTypeOfCollection(typeRef));
            }
        }

        protected static bool ShouldImplementIDeserializable(TypeReference typeRef)
        {
            return UnitySerializationLogic.ShouldImplementIDeserializable(typeRef);
        }

        protected static bool IsUnityEngineObject(TypeReference typeRef)
        {
            return UnityEngineTypePredicates.IsUnityEngineObject(typeRef);
        }

        protected static bool IsSystemByte(TypeReference typeRef)
        {
            return typeRef.FullName == "System.Byte";
        }

        private static bool IsSystemString(TypeReference typeRef)
        {
            return typeRef.FullName == "System.String";
        }

        protected static string ToPascalCase(string name)
        {
            return char.ToUpper(name[0]) + name.Substring(1);
        }

        protected void Box(TypeReference typeRef)
        {
            Processor.Emit(OpCodes.Box, typeRef);
        }

        protected static bool RequiresBoxing(TypeReference typeRef)
        {
            return IsStruct(typeRef);
        }

        private void Emit(OpCode opcode)
        {
            Processor.Emit(opcode);
        }

        protected MethodReference MethodRefFor(Type type, string name)
        {
            return Module.ImportReference(type.GetMethod(name));
        }

        protected MethodReference MethodRefFor(TypeDefinition typeDefinition, string name, bool checkInBaseTypes = false)
        {
            var methodRef = FindMethodReference(typeDefinition, name);

            if (methodRef == null && checkInBaseTypes)
            {
                var current = typeDefinition.BaseType.Resolve();
                while (current != null)
                {
                    methodRef = FindMethodReference(current, name);

                    if (methodRef != null)
                        break;

                    current = current.BaseType != null ? current.BaseType.Resolve() : null;
                }
            }

            if (methodRef == null)
                throw new ArgumentException("Cannot find method " + name + " on type " + typeDefinition.FullName, "name");

            return Module.ImportReference(methodRef);
        }

        private static MethodDefinition FindMethodReference(TypeDefinition typeDefinition, string name)
        {
            var methodRef = typeDefinition.Methods.FirstOrDefault(m => m.Name == name);
            if (methodRef == null)
            {
                // In this case, the method is probably explicitly implemented from an interface.
                // Let's look for it in the overrides than.
                methodRef = typeDefinition.Methods
                    .Where(m => m.HasOverrides)
                    .FirstOrDefault(m => m.Overrides.Any(o => o.Name == name));
            }

            return methodRef;
        }

        protected TypeReference Import(Type type)
        {
            return Module.ImportReference(type);
        }

        protected FieldReference Import(FieldInfo fieldInfo)
        {
            return Module.ImportReference(fieldInfo);
        }

        protected TypeReference VoidTypeRef()
        {
            return Module.TypeSystem.Void;
        }

        protected string MethodSuffixFor(TypeReference typeRef)
        {
            if (typeRef.IsPrimitive || IsSystemString(typeRef))
                return ToPascalCase(typeRef.Name);

            if (IsEnum(typeRef))
                return "Int32";

            if (CecilUtils.IsGenericList(typeRef))
                return "ListOf" + MethodSuffixFor(CecilUtils.ElementTypeOfCollection(typeRef));

            if (typeRef.IsArray)
                return "ArrayOf" + MethodSuffixFor(typeRef.GetElementType());

            if (IsUnityEngineObject(typeRef))
                return "UnityEngineObject";

            if (UnityEngineTypePredicates.IsSerializableUnityStruct(typeRef))
                return typeRef.Name;

            if (ShouldImplementIDeserializable(typeRef))
                return "IDeserializable";

            throw new ArgumentException("typeRef", "Cannot serialize field of type " + typeRef.FullName);
        }

        protected static bool IsIUnitySerializable(TypeReference typeRef)
        {
            if (IsUnityEngineObject(typeRef) || UnityEngineTypePredicates.IsSerializableUnityStruct(typeRef))
                return false;

            return ShouldImplementIDeserializable(typeRef);
        }

        protected void Call(MethodReference methodReference)
        {
            Processor.Emit(OpCodes.Call, methodReference);
        }

        protected void Callvirt(MethodReference methodReference)
        {
            Processor.Emit(OpCodes.Callvirt, methodReference);
        }

        protected void MarkLabel(Instruction ifNotNullLbl)
        {
            Processor.Append(ifNotNullLbl);
        }

        protected void Br_S(Instruction endLbl)
        {
            Processor.Emit(OpCodes.Br, endLbl);
        }

        protected void Brtrue_S(Instruction ifNotNullLbl)
        {
            Processor.Emit(OpCodes.Brtrue, ifNotNullLbl);
        }

        protected void Bgt(Instruction endLbl)
        {
            Processor.Emit(OpCodes.Bgt, endLbl);
        }

        protected Instruction DefineLabel()
        {
            return Processor.Create(OpCodes.Nop);
        }

        protected void Ldc_I4(int value)
        {
            switch (value)
            {
                case -1:
                    Processor.Emit(OpCodes.Ldc_I4_M1);
                    break;
                case 0:
                    Processor.Emit(OpCodes.Ldc_I4_0);
                    break;
                case 1:
                    Processor.Emit(OpCodes.Ldc_I4_1);
                    break;
                case 2:
                    Processor.Emit(OpCodes.Ldc_I4_2);
                    break;
                case 3:
                    Processor.Emit(OpCodes.Ldc_I4_3);
                    break;
                case 4:
                    Processor.Emit(OpCodes.Ldc_I4_4);
                    break;
                case 5:
                    Processor.Emit(OpCodes.Ldc_I4_5);
                    break;
                case 6:
                    Processor.Emit(OpCodes.Ldc_I4_6);
                    break;
                case 7:
                    Processor.Emit(OpCodes.Ldc_I4_7);
                    break;
                case 8:
                    Processor.Emit(OpCodes.Ldc_I4_8);
                    break;
                default:
                    if ((value >= sbyte.MinValue) && (value <= sbyte.MaxValue))
                        Processor.Emit(OpCodes.Ldc_I4_S, (sbyte)value);
                    else
                        Processor.Emit(OpCodes.Ldc_I4, value);
                    break;
            }
        }

        protected void Ceq()
        {
            Processor.Emit(OpCodes.Ceq);
        }

        protected void EmitWithNullCheckOnField(FieldReference fieldDef, Action<FieldReference> ifNull, Action<FieldReference> ifNotNull)
        {
            var ifNotNullLbl = DefineLabel();
            var endLbl = DefineLabel();

            EmitLoadField(fieldDef);
            Brtrue_S(ifNotNullLbl);

            ifNull(fieldDef);
            Br_S(endLbl);

            MarkLabel(ifNotNullLbl);
            ifNotNull(fieldDef);

            MarkLabel(endLbl);
        }

        private void EmitWithDepthCheck(Action action)
        {
            var endLabel = DefineLabel();
            Ldarg_1();
            Ldc_I4(DepthLimit);
            Bgt(endLabel);
            action();
            MarkLabel(endLabel);
        }

        protected void EmitWithDepthCheck<TArg>(Action<TArg> action, TArg arg)
        {
            EmitWithDepthCheck(() => action(arg));
        }

        protected void EmitWithDepthCheck<TArg1, TArg2>(Action<TArg1, TArg2> action, TArg1 arg1, TArg2 arg2)
        {
            EmitWithDepthCheck(() => action(arg1, arg2));
        }

        protected void EmitWithNullCheckOnField(FieldReference fieldRef, Action<FieldReference> ifNull)
        {
            var endLbl = DefineLabel();

            Ldarg_0();
            Ldfld(fieldRef);
            Brtrue_S(endLbl);
            ifNull(fieldRef);

            MarkLabel(endLbl);
        }

        protected void EmitWithNotNullCheckOnField(FieldReference fieldDef, Action<FieldReference> ifNotNull)
        {
            var endLbl = DefineLabel();

            Ldarg_0();
            Ldfld(fieldDef);
            Brfalse_S(endLbl);
            ifNotNull(fieldDef);

            MarkLabel(endLbl);
        }

        protected void Brfalse_S(Instruction endLbl)
        {
            Processor.Emit(OpCodes.Brfalse, endLbl);
        }

        protected ParameterDefinition ParamDef(string name, Type type)
        {
            return new ParameterDefinition(name, ParameterAttributes.None, Import(type));
        }

        protected void EmitTypeOfIfNeeded(TypeReference typeRef)
        {
            if (!InvocationRequiresTypeRef(typeRef))
                return;

            if (!IsStruct(typeRef))
                EmitTypeOf(typeRef);
            else
                Ldnull();
        }

        private static bool InvocationRequiresTypeRef(TypeReference typeRef)
        {
            return !IsUnityEngineObject(typeRef) &&
                !UnityEngineTypePredicates.IsSerializableUnityStruct(typeRef) &&
                ShouldImplementIDeserializable(typeRef);
        }

        protected void EmitLoopOnFieldElements(FieldReference fieldDef, Action<FieldReference, TypeReference> loopBody)
        {
            EmitLoopOnFieldElements(fieldDef, EmitLengthOf, loopBody);
        }

        protected void EmitLoopOnFieldElements(FieldReference fieldDef, Action<FieldReference> maxValueGen, Action<FieldReference, TypeReference> loopBody)
        {
            var headLbl = DefineLabel();
            var condLbl = DefineLabel();

            EmitItemsLoopInitializer();
            Br_S(condLbl);

            MarkLabel(headLbl);
            {
                loopBody(fieldDef, CecilUtils.ElementTypeOfCollection(TypeOf(fieldDef)));
                EmitIncrementItemIndex();
            }

            MarkLabel(condLbl);
            {
                EmitItemsLoopCondition(fieldDef, maxValueGen);
                Brtrue_S(headLbl);
            }
        }

        protected void EmitItemsLoopInitializer()
        {
            Ldc_I4(0);
            EmitStoreItemIndex();
        }

        protected void EmitItemsLoopCondition(FieldReference fieldDef, Action<FieldReference> maxValueGen)
        {
            EmitLoadItemIndex();
            maxValueGen(fieldDef);
            Conv_I4();
            Clt();
        }

        protected void EmitIncrementItemIndex()
        {
            EmitLoadItemIndex();
            Ldc_I4(1);
            Add();
            EmitStoreItemIndex();
        }

        protected void EmitLoadItemIndex()
        {
            ItemsIndexVar.EmitLoad();
        }

        private void EmitStoreItemIndex()
        {
            ItemsIndexVar.EmitStore();
        }

        private void Clt()
        {
            Processor.Emit(OpCodes.Clt);
        }

        protected void Add()
        {
            Processor.Emit(OpCodes.Add);
        }

        protected void Mul()
        {
            Processor.Emit(OpCodes.Mul);
        }

        protected void EmitLoadFieldArrayItemInLoop(FieldReference fieldDef)
        {
            EmitLoadField(fieldDef);
            EmitLoadItemIndex();

            var elementType = CecilUtils.ElementTypeOfCollection(TypeOf(fieldDef));

            // UnityEngine.Color32 serializer takes it by value, rather than by reference like other structs
            if (elementType.IsPrimitive || IsEnum(elementType) || UnityEngineTypePredicates.IsColor32(elementType))
                Processor.Emit(OpCodes.Ldelem_Any, elementType);
            else if (IsStruct(elementType))
                Processor.Emit(OpCodes.Ldelema, elementType);
            else
                Processor.Emit(OpCodes.Ldelem_Any, elementType);
        }

        protected void Ldelem_Ref()
        {
            Processor.Emit(OpCodes.Ldelem_Ref);
        }

        protected void Ldelema(TypeReference type)
        {
            Processor.Emit(OpCodes.Ldelema, type);
        }

        protected static bool CanInlineLoopOn(TypeReference typeReference, FieldDefinition fieldDefinition)
        {
            return (UnitySerializationLogic.IsSupportedCollection(typeReference) &&
                !IsSystemByte(CecilUtils.ElementTypeOfCollection(typeReference))) ||
                UnitySerializationLogic.IsFixedBuffer(fieldDefinition);
        }

        protected void Isinst(TypeReference typeReference)
        {
            Processor.Emit(OpCodes.Isinst, typeReference);
        }

        protected void Stelem_Ref()
        {
            Processor.Emit(OpCodes.Stelem_Ref);
        }

        private MethodReference SerializationWeaverInjectedConstructorFor(TypeReference typeReference)
        {
            var methodReference = new MethodReference(".ctor", Module.TypeSystem.Void, typeReference)
            {
                HasThis = true
            };
            methodReference.Parameters.Add(SelfUnitySerializableParam());
            return methodReference;
        }

        private MethodReference DefaultConstructorFor(TypeReference typeReference)
        {
            var typeDef = typeReference.Resolve();
            if (typeDef == null)
                return null;

            var ctorDef = typeDef.Methods.SingleOrDefault(x => x.Name == ".ctor" && !x.HasParameters);
            if (ctorDef == null)
                return null;

            return new MethodReference(".ctor", Module.TypeSystem.Void, typeReference)
            {
                HasThis = true
            };
        }

        private ParameterDefinition SelfUnitySerializableParam()
        {
            return new ParameterDefinition(
                "self",
                ParameterAttributes.None,
                Module.ImportReference(_serializationBridgeProvider.UnitySerializableInterface));
        }

        protected MethodReference GetItemMethodRefFor(TypeReference typeReference)
        {
            var genericParameter = ((GenericInstanceType)typeReference).ElementType.GenericParameters[0];
            var method = new MethodReference("get_Item", genericParameter, TypeReferenceFor(typeReference));
            method.Parameters.Add(new ParameterDefinition("index", ParameterAttributes.None, Import(typeof(int))));
            method.HasThis = true;
            return Module.ImportReference(method);
        }

        protected MethodReference SetItemMethodRefFor(TypeReference typeReference)
        {
            var genericParameter = ((GenericInstanceType)typeReference).ElementType.GenericParameters[0];
            var method = new MethodReference("set_Item", Import(typeof(void)), TypeReferenceFor(typeReference));
            method.Parameters.Add(new ParameterDefinition("index", ParameterAttributes.None, Import(typeof(int))));
            method.Parameters.Add(new ParameterDefinition("item", ParameterAttributes.None, genericParameter));
            method.HasThis = true;
            return Module.ImportReference(method);
        }

        protected static TypeReference TypeReferenceFor(TypeReference typeReference)
        {
            var typeDefinition = typeReference as TypeDefinition;
            if (typeDefinition == null)
                return typeReference;

            return new TypeReference(
                typeDefinition.Namespace,
                typeDefinition.Name,
                typeDefinition.Module,
                typeDefinition.Scope,
                typeDefinition.IsValueType)
            {
                DeclaringType = TypeReferenceFor(typeDefinition.DeclaringType)
            };
        }

        protected void LoadStateInstance(TypeDefinition stateInterface)
        {
            Processor.Emit(OpCodes.Ldsfld, new FieldReference("Instance", Module.ImportReference(stateInterface), ConcreteImplementationFor(stateInterface)));
        }

        private TypeReference ConcreteImplementationFor(TypeDefinition stateIntefrace)
        {
            return new TypeReference("UnityEngine.Serialization", stateIntefrace.Name.Substring(1), Module, UnityEngineScope());
        }

        private AssemblyNameReference UnityEngineScope()
        {
            return Module.AssemblyReferences.First(m => m.Name == "UnityEngine.CoreModule");
        }

        protected void CallMethodOn(MethodReference methodReference, TypeReference thisType)
        {
            if (IsStruct(thisType))
                Call(methodReference);
            else
                Callvirt(methodReference);
        }

        protected void ConstructSerializableObject(TypeReference typeReference)
        {
            // Note: we expect "this" to be already on top of the stack.
            var defaultCtor = DefaultConstructorFor(typeReference);

            if (defaultCtor != null)
            {
                Newobj(defaultCtor);
            }
            else
            {
                Ldnull();
                Newobj(SerializationWeaverInjectedConstructorFor(typeReference));
            }
        }

        protected void ConstructAndStoreSerializableObject(FieldReference fieldRef)
        {
            Ldarg_0();
            ConstructSerializableObject(TypeOf(fieldRef));
            EmitStoreField(fieldRef);
        }

        // Check Runtime\Mono\SerializationBackend_DirectMemoryAccess\ShouldTransferField.cpp
        protected bool NeedsDepthCheck(TypeReference type)
        {
            if (type.MetadataType == MetadataType.Class)
                return true;

            if (type is ArrayType || CecilUtils.IsGenericList(type))
                return true;

            return false;
        }
    }
}
