using AssemblyPatcher.Configuration;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace AssemblyPatcher.Events
{
    public class CatchTypePatchingEventArgs : TypedItemTypePatchingEventArgs<ExceptionHandler>
    {
        public CatchTypePatchingEventArgs(MethodDefinition method, ExceptionHandler handler, TypeReference newType, PatchRule patchRule)
            : base(handler, newType, patchRule)
        {
            Method = method;
        }

        public MethodDefinition Method { get; private set; }
    }
}
