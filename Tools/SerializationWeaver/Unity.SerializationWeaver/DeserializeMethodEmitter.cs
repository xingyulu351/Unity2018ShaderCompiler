using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Mono.Cecil;

namespace Unity.Serialization.Weaver
{
    sealed class DeserializeMethodEmitter : DeserializeMethodEmitterBase
    {
        protected override string DeserializeMethodName { get { return "Unity_Deserialize"; } }

        protected override TypeDefinition SerializedStateReaderInterface
        {
            get { return _serializationBridgeProvider.SerializedStateReaderInterface; }
        }

        public static MethodDefinition DeserializeMethodDefinitionFor(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider)
        {
            return new DeserializeMethodEmitter(typeDef, serializationBridgeProvider).MethodDefinition;
        }

        private DeserializeMethodEmitter(TypeDefinition typeDef, SerializationBridgeProvider serializationBridgeProvider) :
            base(typeDef, serializationBridgeProvider)
        {
            CreateMethodDef(DeserializeMethodName);
        }

        protected override void CallReaderMethod(string methodName, string fieldName)
        {
            Callvirt(SerializedStateReaderInterface, methodName);
        }

        protected override void ReadSequenceLength(string fieldName)
        {
            LoadStateInstance(SerializedStateReaderInterface);
            Callvirt(SerializedStateReaderInterface, ReadMethodNameFor(Import(typeof(int))));
        }

        protected override void FinishReadingSequence()
        {
        }
    }
}
