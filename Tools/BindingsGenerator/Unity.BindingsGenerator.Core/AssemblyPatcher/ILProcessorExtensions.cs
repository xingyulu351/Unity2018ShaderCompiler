using System.Collections.Generic;
using Mono.Cecil.Cil;

namespace Unity.BindingsGenerator.Core.AssemblyPatcher
{
    public static class ILProcessorExtensions
    {
        public static void AppendRange(this ILProcessor processor, IEnumerable<Instruction> instructions)
        {
            foreach (var instruction in instructions)
            {
                processor.Append(instruction);
            }
        }
    }
}
