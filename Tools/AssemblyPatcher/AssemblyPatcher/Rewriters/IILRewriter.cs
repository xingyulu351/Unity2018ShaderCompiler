using AssemblyPatcher.Configuration;
using Mono.Cecil.Cil;

namespace AssemblyPatcher.Rewriters
{
    public interface IILRewriter
    {
        void Rewrite(ILProcessor ilProcessor, Instruction inst, PatchRule rule);
        string Name { get; }
    }
}
