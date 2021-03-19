using Mono.Cecil;

namespace Unity.Serialization.Weaver
{
    sealed class NamedDeserializeMethodEmitter : DeserializeMethodEmitterBase
    {
        private LocalVariable fieldNamesVariable;

        protected override string DeserializeMethodName { get { return "Unity_NamedDeserialize"; } }

        protected override TypeDefinition SerializedStateReaderInterface
        {
            get { return _serializationBridgeProvider.SerializedNamedStateReaderInterface; }
        }

        public static MethodDefinition DeserializeMethodDefinitionFor(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider)
        {
            return new NamedDeserializeMethodEmitter(typeDef, serializationBridgeProvider).MethodDefinition;
        }

        private NamedDeserializeMethodEmitter(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider) :
            base(typeDef, serializationBridgeProvider)
        {
            CreateMethodDef(DeserializeMethodName);
        }

        protected override void CallReaderMethod(string methodName, string fieldName)
        {
            EmitCString(fieldName);
            Callvirt(SerializedStateReaderInterface, methodName);
        }

        protected override void CallDeserializeMethodFor(string fieldName, TypeReference typeReference)
        {
            LoadStateInstance(SerializedStateReaderInterface);
            EmitCString(fieldName);
            Callvirt(SerializedStateReaderInterface, "BeginMetaGroup");

            base.CallDeserializeMethodFor(fieldName, typeReference);

            LoadStateInstance(SerializedStateReaderInterface);
            Callvirt(SerializedStateReaderInterface, "EndMetaGroup");
        }

        protected override void ReadSequenceLength(string fieldName)
        {
            LoadStateInstance(SerializedStateReaderInterface);
            EmitCString(fieldName);
            Callvirt(SerializedStateReaderInterface, "BeginSequenceGroup");
        }

        protected override void FinishReadingSequence()
        {
            LoadStateInstance(SerializedStateReaderInterface);
            Callvirt(SerializedStateReaderInterface, "EndMetaGroup");
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
