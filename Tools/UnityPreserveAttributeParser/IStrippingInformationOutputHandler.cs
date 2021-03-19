using System;
using Mono.Cecil;

namespace UnityPreserveAttributeParser
{
    public interface IStrippingInformationOutputHandler
    {
        void ProcessAssembly(AssemblyDefinition assembly);
        void ProcessType(TypeDefinition type, PreserveState state);
        void ProcessMethod(MethodDefinition method, PreserveState state);
        void EndType();
        void EndModule();
        void EndAssembly();
        void End();
        void ProcessModule(string key);
    }
}
