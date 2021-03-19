using System;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Unity.CecilTools;
using Unity.SerializationLogic;

namespace Unity.Serialization.Weaver
{
    abstract class SerializeMethodEmitterBase : MethodEmitterBase
    {
        protected abstract TypeDefinition SerializedStateWriterInterface { get; }
        protected abstract string SerializeMethodName { get; }

        protected SerializeMethodEmitterBase(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider) :
            base(typeDef, serializationBridgeProvider)
        {
        }

        protected override ParameterDefinition[] MethodDefParameters()
        {
            return new[] { ParamDef("depth", typeof(int)), ParamDef("cloningObject", typeof(bool)) };
        }

        protected abstract void WriteSequenceLength(string fieldName, Action emitSequenceLength);
        protected abstract void FinishWritingSequence();

        protected override bool ShouldProcess(FieldDefinition fieldDefinition)
        {
            return WillUnitySerialize(fieldDefinition);
        }

        protected override void EmitInstructionsFor(FieldReference fieldDef)
        {
            CallSerializationMethod(fieldDef);

            var fieldType = TypeOf(fieldDef);

            if (IsStruct(TypeDef) && UnityEngineTypePredicates.IsUnityEngineValueType(TypeDef))
                return;

            if (IsStruct(fieldType) || RequiresAlignment(fieldType))
                EmitAlign();
        }

        private void CallSerializationMethod(FieldReference fieldDef)
        {
            if (IsIUnitySerializable(TypeOf(fieldDef)))
            {
                EmitWithDepthCheck(InlinedSerializeInvocationFor, fieldDef);
                return;
            }

            EmitSerializationFor(fieldDef);
        }

        private void EmitSerializationFor(FieldReference fieldDef)
        {
            var typeRef = TypeOf(fieldDef);
            var field = fieldDef.Resolve();

            if (!CanInlineLoopOn(typeRef, field))
            {
                if (NeedsDepthCheck(typeRef))
                {
                    EmitWithDepthCheck(EmitInlineSerializationFor, fieldDef);
                }
                else
                {
                    EmitInlineSerializationFor(fieldDef);
                }
            }
            else
            {
                if (UnitySerializationLogic.IsFixedBuffer(field))
                    EmitWithDepthCheck(EmitInlineSerializationForFixedBuffer, field);
                else
                    EmitWithDepthCheck(EmitInlineSerializationLoop, fieldDef);
            }
        }

        private void EmitInlineSerializationFor(FieldReference fieldRef)
        {
            LoadStateInstance(SerializedStateWriterInterface);
            EmitLoadField(fieldRef);

            EmitTypeOfIfNeeded(ElementTypeFor(TypeOf(fieldRef)));
            CallWriterMethod(WriteMethodNameFor(fieldRef), fieldRef.Name);
        }

        private void EmitInlineSerializationLoop(FieldReference fieldRef)
        {
            EmitWithNullCheckOnField(fieldRef, EmitWriteEmptyCollection, EmitInlineSerializationLoopUnchecked);
        }

        private void EmitInlineSerializationLoopUnchecked(FieldReference fieldDef)
        {
            WriteSequenceLength(fieldDef.Name, () => EmitLengthOf(fieldDef));
            EmitLoopOnFieldElements(fieldDef, EmitSerializeItem);
            FinishWritingSequence();
        }

        private void EmitInlineSerializationForFixedBuffer(FieldDefinition fieldDef)
        {
            var fixedBufferLength = UnitySerializationLogic.GetFixedBufferLength(fieldDef);

            WriteSequenceLength(fieldDef.Name, () => Ldc_I4(fixedBufferLength));

            var fieldType = fieldDef.FieldType.Resolve();

            if (fieldType.Fields.Count != 1)
                throw new ArgumentException(string.Format("Fixed buffer type has {0} fields, expected 1", fieldType.FullName));

            var fixedElementField = fieldType.Fields[0].Resolve();

            if (fixedElementField.Name != "FixedElementField")
                throw new ArgumentException(string.Format("Fixed buffer type field is named '{0}', expected 'FixedElementField'", fixedElementField.FullName));

            var elementFieldType = fixedElementField.FieldType;
            var managedPointerType = new PinnedType(new ByReferenceType(elementFieldType));
            var localManagedPointer = DefineLocal(managedPointerType);
            var temp = DefineLocal(elementFieldType);

            var headLbl = DefineLabel();
            var condLbl = DefineLabel();

            EmitLoadField(fieldDef);
            Ldflda(fixedElementField);
            localManagedPointer.EmitStore();

            EmitItemsLoopInitializer();
            Br_S(condLbl);

            MarkLabel(headLbl);
            {
                localManagedPointer.EmitLoad();
                EmitLoadItemIndex();

                Conv_I();
                Ldc_I4(UnitySerializationLogic.PrimitiveTypeSize(elementFieldType));
                Mul();
                Add();

                Ldind(elementFieldType);

                temp.EmitStore();

                LoadStateInstance(SerializedStateWriterInterface);
                temp.EmitLoad();
                CallWriterMethod(WriteMethodNameFor(elementFieldType), null);

                EmitIncrementItemIndex();
            }

            MarkLabel(condLbl);
            {
                EmitItemsLoopCondition(fieldDef, (_) => Ldc_I4(fixedBufferLength));
                Brtrue_S(headLbl);
            }

            FinishWritingSequence();
        }

        private void EmitWriteEmptyCollection(FieldReference fieldDef)
        {
            WriteSequenceLength(fieldDef.Name, () => Ldc_I4(0));
            FinishWritingSequence();
        }

        private void EmitSerializeItem(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (!IsIUnitySerializable(elementTypeRef))
            {
                var fieldType = TypeOf(fieldDef);
                if (fieldType.IsArray)
                {
                    LoadStateInstance(SerializedStateWriterInterface);
                    EmitLoadFieldArrayItemInLoop(fieldDef);
                    CallWriterMethod(WriteMethodNameFor(elementTypeRef), null);
                }
                else
                {
                    LoadStateInstance(SerializedStateWriterInterface);
                    EmitLoadField(fieldDef);
                    EmitLoadItemIndex();

                    Call(GetItemMethodRefFor(fieldType));

                    // Non-color structs have to be loaded by ref
                    var elementType = CecilUtils.ElementTypeOfCollection(fieldType);
                    if (!elementType.IsPrimitive && !IsEnum(elementType) && !UnityEngineTypePredicates.IsColor32(elementType) && IsStruct(elementType))
                    {
                        var itemTempStorage = DefineLocal(elementType);
                        itemTempStorage.EmitStore();
                        itemTempStorage.EmitLoadAddress();
                    }

                    CallWriterMethod(WriteMethodNameFor(elementTypeRef), null);
                }
            }
            else
                EmitSerializeItemForIDeserializable(fieldDef, elementTypeRef);
        }

        private void EmitSerializeItemForIDeserializable(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (IsStruct(elementTypeRef))
                EmitSerializeItemForStruct(fieldDef, elementTypeRef);
            else
                EmitSerializeItemForIUnitySerializable(fieldDef, elementTypeRef);
        }

        private void EmitSerializeItemForStruct(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (TypeOf(fieldDef).IsArray)
                EmitSerializeArrayItemForStruct(fieldDef, elementTypeRef);
            else
                EmitSerializeListItemForStruct(fieldDef, elementTypeRef);
        }

        private void EmitSerializeArrayItemForStruct(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            EmitLoadFieldArrayItemInLoop(fieldDef);
            CallSerializeMethodFor(null, elementTypeRef);
        }

        private void EmitSerializeListItemForStruct(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            EmitLoadField(fieldDef);
            EmitLoadItemIndex();
            Call(GetItemMethodRefFor(TypeOf(fieldDef)));

            if (IsStruct(elementTypeRef))
            {
                var tmp = DefineLocal(elementTypeRef);
                tmp.EmitStore();
                Processor.Emit(OpCodes.Ldloca, tmp.Index);
            }

            CallSerializeMethodFor(null, elementTypeRef);
        }

        private void EmitSerializeItemForIUnitySerializable(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (TypeOf(fieldDef).IsArray)
                EmitSerializeArrayItemForIUnitySerializable(fieldDef, elementTypeRef);
            else
                EmitSerializeListItemForIUnitySerializable(fieldDef, elementTypeRef);
        }

        private void EmitSerializeArrayItemForIUnitySerializable(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            var ifNotNullLbl = DefineLabel();
            var endLbl = DefineLabel();

            EmitLoadFieldArrayItemInLoop(fieldDef);
            Brtrue_S(ifNotNullLbl);

            {
                ConstructSerializableObject(elementTypeRef);
                Br_S(endLbl);
            }

            MarkLabel(ifNotNullLbl);
            {
                EmitLoadFieldArrayItemInLoop(fieldDef);
            }

            MarkLabel(endLbl);
            CallSerializeMethodFor(null, elementTypeRef);
        }

        private void EmitSerializeListItemForIUnitySerializable(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            var ifNotNullLbl = DefineLabel();
            var endLbl = DefineLabel();

            EmitLoadField(fieldDef);
            EmitLoadItemIndex();
            Call(GetItemMethodRefFor(TypeOf(fieldDef)));

            Brtrue_S(ifNotNullLbl);

            {
                ConstructSerializableObject(elementTypeRef);
                Br_S(endLbl);
            }

            MarkLabel(ifNotNullLbl);
            {
                EmitLoadField(fieldDef);
                EmitLoadItemIndex();
                Call(GetItemMethodRefFor(TypeOf(fieldDef)));
            }

            MarkLabel(endLbl);
            CallSerializeMethodFor(null, elementTypeRef);
        }

        private string WriteMethodNameFor(FieldReference fieldDef)
        {
            return WriteMethodNameFor(TypeOf(fieldDef));
        }

        protected string WriteMethodNameFor(TypeReference typeRef)
        {
            return "Write" + MethodSuffixFor(typeRef);
        }

        private void InlinedSerializeInvocationFor(FieldReference fieldDef)
        {
            if (IsStruct(TypeOf(fieldDef)))
                InlinedSerializeInvocationForStruct(fieldDef);
            else
                InlinedSerializeInvocationForClass(fieldDef);
        }

        private void InlinedSerializeInvocationForStruct(FieldReference fieldDef)
        {
            EmitLoadField(fieldDef);
            CallSerializeMethodFor(fieldDef.Name, TypeOf(fieldDef));
        }

        private void InlinedSerializeInvocationForClass(FieldReference fieldDef)
        {
            EmitWithNullCheckOnField(fieldDef, ConstructAndStoreSerializableObject);
            EmitSerializeInvocation(fieldDef);
        }

        private void EmitSerializeInvocation(FieldReference fieldDef)
        {
            EmitLoadField(fieldDef);
            CallSerializeMethodFor(fieldDef.Name, TypeOf(fieldDef));
        }

        private MethodReference SerializeMethodRefFor(TypeReference typeReference)
        {
            var method = new MethodReference(SerializeMethodName, Import(typeof(void)), TypeReferenceFor(typeReference))
            {
                HasThis = true
            };
            method.Parameters.Add(new ParameterDefinition("depth", ParameterAttributes.None, Import(typeof(int))));
            method.Parameters.Add(new ParameterDefinition("cloningObject", ParameterAttributes.None, Import(typeof(bool))));

            return method;
        }

        private static TypeReference ElementTypeFor(TypeReference typeRef)
        {
            return UnitySerializationLogic.IsSupportedCollection(typeRef) ? CecilUtils.ElementTypeOfCollection(typeRef) : typeRef;
        }

        protected void EmitAlign()
        {
            LoadStateInstance(SerializedStateWriterInterface);
            Callvirt(SerializedStateWriterInterface, AlignMethodName);
        }

        protected override void InjectBeforeSerialize()
        {
            InvokeSerializationCallbackMethod("OnBeforeSerialize");
        }

        protected override void InjectAfterDeserialize()
        {
        }

        protected virtual void CallSerializeMethodFor(string fieldName, TypeReference typeReference)
        {
            Ldarg_1(); // depth
            Ldc_I4(1);
            Add();
            Ldarg_2(); // cloningObject
            CallMethodOn(SerializeMethodRefFor(typeReference), typeReference);
        }

        protected abstract void CallWriterMethod(string methodName, string fieldName);
    }
}
