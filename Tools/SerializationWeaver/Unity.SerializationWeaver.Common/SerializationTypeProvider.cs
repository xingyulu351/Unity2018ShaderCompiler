using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace Unity.SerializationWeaver.Common
{
    public class SerializationTypeProvider
    {
        private readonly AssemblyDefinition _unityEngineAssembly;

        public SerializationTypeProvider(AssemblyDefinition unityEngineAssembly)
        {
            _unityEngineAssembly = unityEngineAssembly;
        }

        public TypeDefinition UnitySerializableInterface
        {
            get { return GetTypeNamed("UnityEngine.IUnitySerializable"); }
        }

        public TypeDefinition UnityAssetsReferenceHolderInterface
        {
            get { return GetTypeNamed("UnityEngine.IUnityAssetsReferenceHolder"); }
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

        private TypeDefinition GetTypeNamed(string typeName)
        {
            return Types.First(t => t.FullName == typeName);
        }

        private IEnumerable<TypeDefinition> Types
        {
            get { return _unityEngineAssembly.MainModule.Types; }
        }
    }
}
