using Mono.Cecil;
using Mono.Cecil.Cil;
using System;
using Unity.CecilTools;
using Unity.SerializationLogic;

namespace Unity.Serialization.Weaver
{
    abstract class DeserializeMethodEmitterBase : MethodEmitterBase
    {
        protected abstract string DeserializeMethodName { get; }
        private LocalVariable _countVar;

        private LocalVariable CountVar
        {
            get { return _countVar ?? (_countVar = DefineLocal(Import(typeof(int)))); }
        }

        protected DeserializeMethodEmitterBase(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider) :
            base(typeDef, serializationBridgeProvider)
        {
        }

        protected override ParameterDefinition[] MethodDefParameters()
        {
            return new[] { ParamDef("depth", typeof(int)), ParamDef("cloningObject", typeof(bool)) };
        }

        protected override void InjectBeforeSerialize()
        {
        }

        protected override void InjectAfterDeserialize()
        {
            InvokeSerializationCallbackMethod("OnAfterDeserialize");
        }

        protected abstract void CallReaderMethod(string methodName, string fieldName);
        protected abstract void ReadSequenceLength(string fieldName);
        protected abstract void FinishReadingSequence();

        protected override bool ShouldProcess(FieldDefinition fieldDefinition)
        {
            return WillUnitySerialize(fieldDefinition);
        }

        protected override void EmitInstructionsFor(FieldReference fieldDef)
        {
            CallDeserializationMethod(fieldDef);

            var fieldType = TypeOf(fieldDef);

            if (IsStruct(TypeDef) && UnityEngineTypePredicates.IsUnityEngineValueType(TypeDef))
                return;

            if (IsStruct(fieldType) || RequiresAlignment(fieldType))
                EmitAlign();
        }

        private void CallDeserializationMethod(FieldReference fieldDef)
        {
            if (IsIUnitySerializable(TypeOf(fieldDef)))
            {
                EmitWithDepthCheck(InlinedDeserializeInvocationFor, fieldDef);
                return;
            }

            EmitDeserializationFor(fieldDef);
        }

        private void InlinedDeserializeInvocationFor(FieldReference fieldDef)
        {
            var fieldType = TypeOf(fieldDef);

            if (!IsStruct(fieldType))
            {
                EmitWithNullCheckOnField(fieldDef, ConstructAndStoreSerializableObject);

                EmitLoadField(fieldDef);
                CallDeserializeMethodFor(fieldDef.Name, fieldType);
            }
            else
            {
                EmitLoadField(fieldDef);
                CallDeserializeMethodFor(fieldDef.Name, fieldType);
            }
        }

        private MethodReference DeserializeMethodRefFor(TypeReference typeReference)
        {
            var method = new MethodReference(DeserializeMethodName, Import(typeof(void)), TypeReferenceFor(typeReference))
            {
                HasThis = true,
            };
            method.Parameters.Add(new ParameterDefinition("depth", ParameterAttributes.None, Import(typeof(int))));
            method.Parameters.Add(new ParameterDefinition("cloningObject", ParameterAttributes.None, Import(typeof(bool))));
            return method;
        }

        private void EmitDeserializationFor(FieldReference fieldDef)
        {
            var typeRef = TypeOf(fieldDef);
            var field = fieldDef.Resolve();

            if (!CanInlineLoopOn(typeRef, field))
            {
                if (NeedsDepthCheck(typeRef))
                {
                    EmitWithDepthCheck(EmitInlineDeserializationFor, fieldDef, typeRef);
                }
                else
                {
                    EmitInlineDeserializationFor(fieldDef, typeRef);
                }
            }
            else
            {
                if (UnitySerializationLogic.IsFixedBuffer(field))
                    EmitWithDepthCheck(EmitInlineDeserializationForFixedBuffer, field);
                else
                    EmitWithDepthCheck(EmitDeserializationLoopFor, fieldDef, typeRef);
            }
        }

        private void EmitInlineDeserializationFor(FieldReference fieldRef, TypeReference typeRef)
        {
            Ldarg_0();
            EmitDeserializeInvocationFor(fieldRef);
            if (!typeRef.IsValueType)
                Isinst(typeRef);

            EmitStoreField(fieldRef);
        }

        private void EmitDeserializationLoopFor(FieldReference fieldDef, TypeReference typeRef)
        {
            Ldarg_0();
            ReadSequenceLength(fieldDef.Name);

            if (CecilUtils.IsGenericList(TypeOf(fieldDef)))
            {
                CountVar.EmitStore();
                CountVar.EmitLoad();
                EmitNewCollectionOf(typeRef);
                EmitStoreField(fieldDef);

                EmitLoopOnFieldElements(fieldDef, _ => CountVar.EmitLoad(), EmitDeserializeOfItem);
            }
            else
            {
                EmitNewCollectionOf(typeRef);
                EmitStoreField(fieldDef);

                EmitLoopOnFieldElements(fieldDef, EmitDeserializeOfItem);
            }

            FinishReadingSequence();
        }

        private void EmitInlineDeserializationForFixedBuffer(FieldDefinition fieldDef)
        {
            var fixedBufferLength = UnitySerializationLogic.GetFixedBufferLength(fieldDef);

            ReadSequenceLength(fieldDef.Name);
            Pop();

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
                LoadStateInstance(SerializedStateReaderInterface);
                CallReaderMethod(ReadMethodNameFor(elementFieldType), null);

                temp.EmitStore();

                localManagedPointer.EmitLoad();
                EmitLoadItemIndex();

                Conv_I();
                Ldc_I4(UnitySerializationLogic.PrimitiveTypeSize(elementFieldType));
                Mul();
                Add();

                temp.EmitLoad();

                Stind(elementFieldType);

                EmitIncrementItemIndex();
            }

            MarkLabel(condLbl);
            {
                EmitItemsLoopCondition(fieldDef, (_) => Ldc_I4(fixedBufferLength));
                Brtrue_S(headLbl);
            }

            FinishReadingSequence();
        }

        protected abstract TypeDefinition SerializedStateReaderInterface { get; }

        private void EmitNewCollectionOf(TypeReference typeRef)
        {
            var elementType = CecilUtils.ElementTypeOfCollection(typeRef);
            if (typeRef.IsArray)
                Newarr(elementType);
            else
                Newobj(ListConstructorRefFor(typeRef));
        }

        private MethodReference ListConstructorRefFor(TypeReference typeReference)
        {
            return new MethodReference(ConstructorMethodName, Import(typeof(void)), TypeReferenceFor(typeReference))
            {
                HasThis = true,
                Parameters =
                {
                    ParamDef("count", typeof(int))
                }
            };
        }

        private void Newarr(TypeReference elementType)
        {
            Processor.Emit(OpCodes.Newarr, elementType);
        }

        private void EmitDeserializeOfItem(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (!IsIUnitySerializable(elementTypeRef))
            {
                var typeRef = TypeOf(fieldDef);
                if (CecilUtils.IsGenericList(typeRef))
                {
                    EmitLoadField(fieldDef);

                    LoadStateInstance(SerializedStateReaderInterface);
                    CallReaderMethod(ReadMethodNameFor(elementTypeRef), null);

                    if (!elementTypeRef.IsValueType)
                    {
                        Isinst(elementTypeRef);
                        Callvirt(AddMethodRefFor(typeRef));
                    }
                    else
                        Callvirt(AddMethodRefFor(typeRef));
                }
                else
                {
                    EmitLoadField(fieldDef);
                    EmitLoadItemIndex();

                    LoadStateInstance(SerializedStateReaderInterface);
                    CallReaderMethod(ReadMethodNameFor(elementTypeRef), null);

                    if (!elementTypeRef.IsValueType)
                    {
                        Isinst(elementTypeRef);
                        Stelem_Ref();
                    }
                    else
                        Stelem_Any(elementTypeRef);
                }
            }
            else
                EmitDeserializeOfIUnitySerializableItem(fieldDef, elementTypeRef);
        }

        private void Stelem_Any(TypeReference elementTypeRef)
        {
            Processor.Emit(OpCodes.Stelem_Any, elementTypeRef);
        }

        private void EmitDeserializeOfIUnitySerializableItem(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (IsStruct(elementTypeRef))
                EmitDeserializeOfStructItem(fieldDef, elementTypeRef);
            else
                EmitDeserializeOfIUnitySerializableClassItem(fieldDef, elementTypeRef);
        }

        protected void EmitDeserializeOfIUnitySerializableClassItem(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (CecilUtils.IsGenericList(TypeOf(fieldDef)))
            {
                var tmp = DefineLocal(elementTypeRef);

                ConstructSerializableObject(elementTypeRef);
                tmp.EmitStore();

                tmp.EmitLoad();
                CallDeserializeMethodFor(null, elementTypeRef);

                EmitLoadField(fieldDef);
                tmp.EmitLoad();
                Callvirt(AddMethodRefFor(TypeOf(fieldDef)));
            }
            else
            {
                EmitLoadField(fieldDef);
                EmitLoadItemIndex();

                ConstructSerializableObject(elementTypeRef);
                Stelem_Ref();

                EmitLoadFieldArrayItemInLoop(fieldDef);
                CallDeserializeMethodFor(null, elementTypeRef);
            }
        }

        private void EmitDeserializeOfStructItem(FieldReference fieldDef, TypeReference elementTypeRef)
        {
            if (CecilUtils.IsGenericList(TypeOf(fieldDef)))
            {
                var tmp = DefineLocal(elementTypeRef);

                Processor.Emit(OpCodes.Ldloca, tmp.Index);
                Processor.Emit(OpCodes.Initobj, elementTypeRef);

                Processor.Emit(OpCodes.Ldloca, tmp.Index);
                CallDeserializeMethodFor(null, elementTypeRef);

                EmitLoadField(fieldDef);
                tmp.EmitLoad();
                Callvirt(AddMethodRefFor(TypeOf(fieldDef)));
            }
            else
            {
                var tmp = DefineLocal(elementTypeRef);

                Processor.Emit(OpCodes.Ldloca, tmp.Index);
                Processor.Emit(OpCodes.Initobj, elementTypeRef);

                Processor.Emit(OpCodes.Ldloca, tmp.Index);
                CallDeserializeMethodFor(null, elementTypeRef);

                EmitLoadField(fieldDef);
                EmitLoadItemIndex();
                tmp.EmitLoad();
                Stelem_Any(elementTypeRef);
            }
        }

        private MethodReference AddMethodRefFor(TypeReference typeReference)
        {
            var genericParameter = ((GenericInstanceType)typeReference).ElementType.GenericParameters[0];
            var method = new MethodReference("Add", Import(typeof(void)), TypeReferenceFor(typeReference));
            method.Parameters.Add(new ParameterDefinition("item", ParameterAttributes.None, genericParameter));
            method.HasThis = true;
            return Module.ImportReference(method);
        }

        private void EmitDeserializeInvocationFor(FieldReference fieldDef)
        {
            LoadStateInstance(SerializedStateReaderInterface);
            CallReaderMethod(ReadMethodNameFor(fieldDef), fieldDef.Name);
        }

        private string ReadMethodNameFor(FieldReference fieldDef)
        {
            return ReadMethodNameFor(TypeOf(fieldDef));
        }

        protected string ReadMethodNameFor(TypeReference typeRef)
        {
            return "Read" + MethodSuffixFor(typeRef);
        }

        private void EmitAlign()
        {
            LoadStateInstance(SerializedStateReaderInterface);
            Callvirt(SerializedStateReaderInterface, AlignMethodName);
        }

        protected virtual void CallDeserializeMethodFor(string fieldName, TypeReference typeReference)
        {
            Ldarg_1(); // depth
            Ldc_I4(1);
            Add();
            Ldarg_2(); // cloningObject
            CallMethodOn(DeserializeMethodRefFor(typeReference), typeReference);
        }
    }
}
