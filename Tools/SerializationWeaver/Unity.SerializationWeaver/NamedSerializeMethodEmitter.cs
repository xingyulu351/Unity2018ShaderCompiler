using System;
using Mono.Cecil;
using Unity.SerializationLogic;
using Mono.Cecil.Cil;

namespace Unity.Serialization.Weaver
{
    sealed class NamedSerializeMethodEmitter : SerializeMethodEmitterBase
    {
        private LocalVariable fieldNamesVariable;

        protected override string SerializeMethodName { get { return "Unity_NamedSerialize"; } }

        protected override TypeDefinition SerializedStateWriterInterface
        {
            get { return _serializationBridgeProvider.SerializedNamedStateWriterInterface; }
        }

        public static MethodDefinition SerializeMethodDefinitionFor(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider)
        {
            return new NamedSerializeMethodEmitter(typeDef, serializationBridgeProvider).MethodDefinition;
        }

        private NamedSerializeMethodEmitter(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider) :
            base(typeDef, serializationBridgeProvider)
        {
            CreateMethodDef(SerializeMethodName);
        }

        protected override void CallWriterMethod(string methodName, string fieldName)
        {
            EmitCString(fieldName);
            Callvirt(SerializedStateWriterInterface, methodName);
        }

        protected override void CallSerializeMethodFor(string fieldName, TypeReference typeReference)
        {
            LoadStateInstance(SerializedStateWriterInterface);
            EmitCString(fieldName);
            Callvirt(SerializedStateWriterInterface, "BeginMetaGroup");

            base.CallSerializeMethodFor(fieldName, typeReference);

            LoadStateInstance(SerializedStateWriterInterface);
            Callvirt(SerializedStateWriterInterface, "EndMetaGroup");
        }

        protected override void WriteSequenceLength(string fieldName, Action emitSequenceLength)
        {
            LoadStateInstance(SerializedStateWriterInterface);
            EmitCString(fieldName);
            emitSequenceLength();
            Callvirt(SerializedStateWriterInterface, "BeginSequenceGroup");
        }

        protected override void FinishWritingSequence()
        {
            LoadStateInstance(SerializedStateWriterInterface);
            Callvirt(SerializedStateWriterInterface, "EndMetaGroup");
        }

        private void EmitCString(string fieldName)
        {
            if (fieldName != null)
            {
                if (fieldNamesVariable == null)
                {
                    fieldNamesVariable = DefineLocal(new PinnedType(new ByReferenceType(Module.TypeSystem.Byte)));
                    CStringStore.StoreStoragePointerIntoLocalVariable(Processor, fieldNamesVariable, Module);
                }

                fieldNamesVariable.EmitLoad();
                Conv_I();

                var stringOffset = CStringStore.GetStringOffset(Module, fieldName);

                if (stringOffset != 0)
                {
                    Ldc_I4(stringOffset);
                    Add();
                }
            }
            else
            {
                Ldc_I4(0);
                Conv_I();
            }
        }
    }
}
