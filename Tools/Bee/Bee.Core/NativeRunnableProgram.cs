using NiceIO;

namespace Bee.Core
{
    public class NativeRunnableProgram : RunnableProgram
    {
        public NativeRunnableProgram(NPath path)
        {
            Path = path;
        }

        public override NPath Path { get; }
        public override string InvocationString => Path.InQuotes(SlashMode.Native);
    }
}
