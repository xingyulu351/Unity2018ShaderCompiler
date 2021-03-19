using AssemblyPatcher.Configuration;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace AssemblyPatcher.Events
{
    public class LocalVariableTypePatchingEventArgs : TypedItemTypePatchingEventArgs<VariableDefinition>
    {
        public LocalVariableTypePatchingEventArgs(MethodDefinition declaringMethod, VariableDefinition localVar, TypeReference newType, PatchRule patchRule) : base(localVar, newType, patchRule)
        {
            DeclaringMethod = declaringMethod;
        }

        public MethodDefinition DeclaringMethod { get; private set; }
    }
}
