using System;
using AssemblyPatcher.Configuration;
using Mono.Cecil;

namespace AssemblyPatcher.Events
{
    public class TypedItemTypePatchingEventArgs<T> : EventArgs
    {
        public T Item { get; private set; }
        public TypeReference NewType { get; private set; }
        public PatchRule PatchRule { get; private set; }

        public TypedItemTypePatchingEventArgs(T item, TypeReference newType, PatchRule patchRule)
        {
            Item = item;
            NewType = newType;
            PatchRule = patchRule;
        }
    }
}
