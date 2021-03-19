using System;

namespace Bee.Stevedore.Program
{
    /// <summary>
    /// A "humane" exception is an exception with a message that can
    /// readily be show to the user as-is, without further context.
    /// </summary>
    public class HumaneException : Exception
    {
        public HumaneException(string message, Exception inner = null) : base(message, inner) {}
    }
}
