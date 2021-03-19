using System;
using AssemblyPatcher.Configuration;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace AssemblyPatcher.Events
{
    public class MemberReferencePatchingEventArgs : EventArgs
    {
        public MemberReference PatchedItem { get; private set; }
        public MemberReference Item { get; private set; }
        public PatchRule PatchRule { get; private set; }
        public Instruction Instruction { get; private set; }

        public MemberReferencePatchingEventArgs(MemberReference patchedItem, MemberReference item, PatchRule patchRule = null) : this(patchedItem, item, null, patchRule)
        {
        }

        public MemberReferencePatchingEventArgs(MemberReference patchedItem, MemberReference item, Instruction instruction = null, PatchRule patchRule = null)
        {
            PatchedItem = patchedItem;
            Item = item;
            PatchRule = patchRule;
            Instruction = instruction;
        }
    }
}
