using AssemblyPatcher.Configuration;
using Mono.Cecil;

namespace AssemblyPatcher.Events
{
    public class ParameterTypePatchingEventArgs : TypedItemTypePatchingEventArgs<ParameterDefinition>
    {
        public ParameterTypePatchingEventArgs(MethodDefinition method, ParameterDefinition parameter, TypeReference newType, PatchRule patchRule) : base(parameter, newType, patchRule)
        {
            Method = method;
        }

        public MethodDefinition Method { get; private set; }
    }
}
