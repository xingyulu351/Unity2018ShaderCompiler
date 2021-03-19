using NiceIO;

namespace Bee.Core
{
    public abstract class RunnableProgram
    {
        public abstract string InvocationString { get; }
        public abstract NPath Path { get; }
    }
}
