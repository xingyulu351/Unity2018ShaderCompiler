using NiceIO;

namespace Bee.NativeProgramSupport.Core
{
    public struct InvocationResult
    {
        public NPath Result;
        public NPath Stdout;
        public NPath Stderr;

        public static InvocationResult For(NPath program, NPath environmentDirectory = null)
        {
            return new InvocationResult
            {
                Result = (environmentDirectory ?? program.Parent).Combine(program.FileName).ChangeExtension("result"),
                Stdout = (environmentDirectory ?? program.Parent).Combine(program.FileName).ChangeExtension("out"),
                Stderr = (environmentDirectory ?? program.Parent).Combine(program.FileName).ChangeExtension("err"),
            };
        }
    }
}
