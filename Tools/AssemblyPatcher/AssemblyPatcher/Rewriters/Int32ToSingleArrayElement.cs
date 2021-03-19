using System;
using AssemblyPatcher.Configuration;
using Mono.Cecil.Cil;

namespace AssemblyPatcher.Rewriters
{
    class Int32ToSingleArrayElement : IILRewriter
    {
        public void Rewrite(ILProcessor ilProcessor, Instruction inst, PatchRule rule)
        {
            throw new NotImplementedException();
        }

        public string Name
        {
            get { return GetType().Name; }
        }
    }
}
