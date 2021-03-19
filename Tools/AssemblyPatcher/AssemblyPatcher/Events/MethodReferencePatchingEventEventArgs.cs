using AssemblyPatcher.Configuration;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace AssemblyPatcher.Events
{
    public class MethodReferencePatchingEventEventArgs : MemberReferencePatchingEventArgs
    {
        public MethodReferencePatchingEventEventArgs(MemberReference patchedItem, MemberReference item, Instruction instruction, PatchRule patchRule) : base(patchedItem, item, instruction, patchRule)
        {
        }

        public MethodReference TypeReference
        {
            get { return (MethodReference)Item; }
        }
    }
}
