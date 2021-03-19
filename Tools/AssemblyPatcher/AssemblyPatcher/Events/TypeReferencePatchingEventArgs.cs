using AssemblyPatcher.Configuration;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace AssemblyPatcher.Events
{
    public class TypeReferencePatchingEventEventArgs : MemberReferencePatchingEventArgs
    {
        public TypeReferencePatchingEventEventArgs(MemberReference patchedItem, MemberReference item, Instruction instruction, PatchRule patchRule = null) : base(patchedItem, item, instruction, patchRule)
        {
        }

        public TypeReferencePatchingEventEventArgs(MemberReference patchedItem, MemberReference item, PatchRule patchRule) : base(patchedItem, item, null, patchRule)
        {
        }

        public TypeReference TypeReference
        {
            get { return (TypeReference)Item; }
        }
    }
}
