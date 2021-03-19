using System;
using Mono.Cecil;

namespace Unity.Serialization.Weaver
{
    sealed class SerializeMethodEmitter : SerializeMethodEmitterBase
    {
        protected override string SerializeMethodName { get { return "Unity_Serialize"; } }

        protected override TypeDefinition SerializedStateWriterInterface
        {
            get { return _serializationBridgeProvider.SerializedStateWriterInterface; }
        }

        public static MethodDefinition SerializeMethodDefinitionFor(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider)
        {
            return new SerializeMethodEmitter(typeDef, serializationBridgeProvider).MethodDefinition;
        }

        private SerializeMethodEmitter(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider) :
            base(typeDef, serializationBridgeProvider)
        {
            CreateMethodDef(SerializeMethodName);
        }

        protected override void CallWriterMethod(string methodName, string fieldName)
        {
            Callvirt(SerializedStateWriterInterface, methodName);
        }

        protected override void WriteSequenceLength(string fieldName, Action emitSequenceLength)
        {
            LoadStateInstance(SerializedStateWriterInterface);
            emitSequenceLength();
            Callvirt(SerializedStateWriterInterface, WriteMethodNameFor(Import(typeof(int))));
        }

        protected override void FinishWritingSequence()
        {
        }
    }
}
