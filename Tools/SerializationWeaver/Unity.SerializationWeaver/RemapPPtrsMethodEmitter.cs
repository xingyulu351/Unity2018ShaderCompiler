using System;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Unity.SerializationLogic;

namespace Unity.Serialization.Weaver
{
    public class RemapPPtrsMethodEmitter : MethodEmitterBase
    {
        private const string RemapPPtrsTypeName = "Unity_RemapPPtrs";
        private const string GetNewInstanceToReplaceOldInstanceMethodName = "GetNewInstanceToReplaceOldInstance";
        private const string CanExecuteSerializationCallbacks = "CanExecuteSerializationCallbacks";

        public static MethodDefinition RemapPPtrsMethodDefinitionFor(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider)
        {
            return new RemapPPtrsMethodEmitter(typeDef, serializationBridgeProvider).MethodDefinition;
        }

        private RemapPPtrsMethodEmitter(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider) :
            base(typeDef, serializationBridgeProvider)
        {
            CreateMethodDef(RemapPPtrsTypeName);
        }

        protected override ParameterDefinition[] MethodDefParameters()
        {
            return new[] { ParamDef("depth", typeof(int)), ParamDef("cloningObject", typeof(bool)) };
        }

        private TypeDefinition PPtrRemapperInterface
        {
            get { return _serializationBridgeProvider.PPtrRemapperInterface; }
        }

        protected override bool ShouldProcess(FieldDefinition fieldDefinition)
        {
            return UnitySerializationLogic.ShouldFieldBePPtrRemapped(fieldDefinition, TypeResolver);
        }

        protected override void EmitInstructionsFor(FieldReference field)
        {
            if (UnitySerializationLogic.IsSupportedCollection(TypeOf(field)))
                EmitWithDepthCheck(EmitRemappingForCollectionField, field);
            else if (!UnitySerializationLogic.IsFixedBuffer(field.Resolve()))
                EmitRemappingForNonCollectionField(field, EmitLoadField);
        }

        private void EmitRemappingForNonCollectionField(FieldReference field, Action<FieldReference> fieldLoader)
        {
            var fieldType = TypeOf(field);
            if (UnitySerializationLogic.ShouldImplementIDeserializable(fieldType) && !IsUnityEngineObject(fieldType))
                EmitWithDepthCheck(EmitRemappingForIDeserializable, field, fieldLoader);
            else
                EmitRemappingForSimpleField(field, fieldLoader);
        }

        private void EmitRemappingForIDeserializable(FieldReference fieldDef, Action<FieldReference> fieldLoader)
        {
            var fieldType = TypeOf(fieldDef);
            if (IsStruct(fieldType))
                InvokeRemapPPtrsOn(fieldDef, fieldLoader);
            else
                EmitWithNotNullCheckOnField(fieldDef, _ => InvokeRemapPPtrsOn(_, fieldLoader));
        }

        private void InvokeRemapPPtrsOn(FieldReference fieldDef, Action<FieldReference> fieldLoader)
        {
            fieldLoader(fieldDef);
            CallRemapPPtrsMethodFor(TypeOf(fieldDef));
        }

        private void EmitRemappingForSimpleField(FieldReference fieldDef, Action<FieldReference> fieldLoader)
        {
            if (IsUnityEngineObject(TypeOf(fieldDef)))
            {
                var endLbl = DefineLabel();

                fieldLoader(fieldDef);
                Brfalse_S(endLbl);

                EmitRemappingForSimpleFieldUnchecked(fieldDef, fieldLoader);

                MarkLabel(endLbl);
            }
            else
                EmitRemappingForSimpleFieldUnchecked(fieldDef, fieldLoader);
        }

        private void EmitRemappingForSimpleFieldUnchecked(FieldReference fieldDef, Action<FieldReference> fieldLoader)
        {
            Ldarg_0();
            LoadStateInstance(_serializationBridgeProvider.PPtrRemapperInterface);
            fieldLoader(fieldDef);
            Callvirt(PPtrRemapperInterface, GetNewInstanceToReplaceOldInstanceMethodName);
            Isinst(TypeOf(fieldDef));
            EmitStoreField(fieldDef);
        }

        private MethodReference RemapPPtrsMethodRefFor(TypeReference typeReference)
        {
            var method = new MethodReference(RemapPPtrsTypeName, Import(typeof(void)), TypeReferenceFor(typeReference))
            {
                HasThis = true
            };
            method.Parameters.Add(new ParameterDefinition("depth", ParameterAttributes.None, Import(typeof(int))));
            method.Parameters.Add(new ParameterDefinition("cloningObject", ParameterAttributes.None, Import(typeof(bool))));

            return method;
        }

        private void EmitRemappingForCollectionField(FieldReference fieldDef)
        {
            EmitWithNotNullCheckOnField(fieldDef, EmitInlinedRemapPPtrsInvocationForCollection);
        }

        private void EmitInlinedRemapPPtrsInvocationForCollection(FieldReference fieldDef)
        {
            EmitLoopOnFieldElements(fieldDef, EmitRemapPPtrsInvocationLoopBody);
        }

        private void EmitRemapPPtrsInvocationLoopBody(FieldReference fieldDef, TypeReference elementType)
        {
            if (UnitySerializationLogic.ShouldImplementIDeserializable(elementType) && !IsUnityEngineObject(elementType))
            {
                if (TypeOf(fieldDef).IsArray)
                {
                    var endLbl = DefineLabel();
                    {
                        EmitLoadFieldArrayItemInLoop(fieldDef);
                        Brfalse_S(endLbl);

                        EmitLoadFieldArrayItemInLoop(fieldDef);
                        CallRemapPPtrsMethodFor(elementType);
                    }

                    MarkLabel(endLbl);
                }
                else
                {
                    var endLbl = DefineLabel();
                    {
                        EmitLoadField(fieldDef);
                        EmitLoadItemIndex();
                        Callvirt(GetItemMethodRefFor(TypeOf(fieldDef)));

                        var tmp = DefineLocal(elementType);
                        tmp.EmitStore();

                        if (IsStruct(elementType))
                        {
                            Processor.Emit(OpCodes.Ldloca, tmp.Index);
                        }
                        else
                        {
                            tmp.EmitLoad();
                            Brfalse_S(endLbl);
                            tmp.EmitLoad();
                        }

                        CallRemapPPtrsMethodFor(elementType);

                        if (IsStruct(elementType))
                        {
                            // if it is a struct, then RemapPPtrs is called on a temporary struct that was got from collection
                            // this temp struct is modified and now should be put to collection replacing the old one (case 838737)
                            EmitLoadField(fieldDef);
                            EmitLoadItemIndex();
                            Processor.Emit(OpCodes.Ldloc, tmp.Index);
                            Callvirt(SetItemMethodRefFor(TypeOf(fieldDef)));
                        }
                    }

                    MarkLabel(endLbl);
                }
            }
            else
            {
                if (TypeOf(fieldDef).IsArray)
                    EmitRemapPPtrsArrayItemInvocation(fieldDef, elementType);
                else
                    EmitRemapPPtrsListItemInvocation(fieldDef, elementType);
            }
        }

        private void CallRemapPPtrsMethodFor(TypeReference typeReference)
        {
            Ldarg_1(); // depth
            Ldc_I4(1);
            Add();
            Ldarg_2(); // cloningObject
            CallMethodOn(RemapPPtrsMethodRefFor(typeReference), typeReference);
        }

        private void EmitRemapPPtrsArrayItemInvocation(FieldReference fieldDef, TypeReference elementType)
        {
            EmitLoadField(fieldDef);
            EmitLoadItemIndex();
            LoadStateInstance(PPtrRemapperInterface);
            EmitLoadFieldArrayItemInLoop(fieldDef);
            Callvirt(PPtrRemapperInterface, GetNewInstanceToReplaceOldInstanceMethodName);
            Isinst(elementType);
            Stelem_Ref();
        }

        private void EmitRemapPPtrsListItemInvocation(FieldReference fieldDef, TypeReference elementType)
        {
            EmitLoadField(fieldDef);
            EmitLoadItemIndex();

            LoadStateInstance(PPtrRemapperInterface);

            EmitLoadField(fieldDef);
            EmitLoadItemIndex();
            Callvirt(GetItemMethodRefFor(TypeOf(fieldDef)));

            Callvirt(PPtrRemapperInterface, GetNewInstanceToReplaceOldInstanceMethodName);
            Isinst(elementType);

            Callvirt(SetItemMethodRefFor(TypeOf(fieldDef)));
        }

        private void InjectSerializationCallbackMethod(string methodName, bool cloningObjectCheck)
        {
            string interfaceName = "UnityEngine.ISerializationCallbackReceiver";
            if (IfTypeImplementsInterface(interfaceName) == false)
                return;

            var endLbl = DefineLabel();
            {
                LoadStateInstance(_serializationBridgeProvider.PPtrRemapperInterface);
                Callvirt(PPtrRemapperInterface, CanExecuteSerializationCallbacks);
                Brfalse_S(endLbl);
                if (cloningObjectCheck)
                {
                    Ldarg_2();
                    Brtrue_S(endLbl);
                }
                InvokeMethodIfTypeImplementsInterface(interfaceName, methodName);
            }
            MarkLabel(endLbl);
        }

        protected override void InjectBeforeSerialize()
        {
            InjectSerializationCallbackMethod("OnBeforeSerialize", true);
        }

        protected override void InjectAfterDeserialize()
        {
            InjectSerializationCallbackMethod("OnAfterDeserialize", false);
        }
    }
}
