using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace Unity.Serialization.Weaver
{
    public class SerializationBridgeProvider
    {
        public AssemblyDefinition UnityEngineAssembly { get; private set; }

        public SerializationBridgeProvider(AssemblyDefinition serializationInferfaceProvider)
        {
            UnityEngineAssembly = serializationInferfaceProvider;
        }

        public TypeDefinition UnitySerializableInterface
        {
            get { return GetTypeNamed("UnityEngine.IUnitySerializable"); }
        }

        public TypeDefinition PPtrRemapperInterface
        {
            get { return GetTypeNamed("UnityEngine.IPPtrRemapper"); }
        }

        public TypeDefinition SerializedStateWriterInterface
        {
            get { return GetTypeNamed("UnityEngine.ISerializedStateWriter"); }
        }

        public TypeDefinition SerializedStateReaderInterface
        {
            get { return GetTypeNamed("UnityEngine.ISerializedStateReader"); }
        }

        public TypeDefinition SerializedNamedStateWriterInterface
        {
            get { return GetTypeNamed("UnityEngine.ISerializedNamedStateWriter"); }
        }

        public TypeDefinition SerializedNamedStateReaderInterface
        {
            get { return GetTypeNamed("UnityEngine.ISerializedNamedStateReader"); }
        }

        private TypeDefinition GetTypeNamed(string typeName)
        {
            return Types.First(t => t.FullName == typeName);
        }

        private IEnumerable<TypeDefinition> Types
        {
            get { return UnityEngineAssembly.MainModule.Types; }
        }
    }
}
