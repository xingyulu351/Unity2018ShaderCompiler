using System;
using System.IO;

namespace Bee.TestHelpers
{
    /// <summary>
    /// Test utility to redirect Console.Out/Error to given TextWriter,
    /// until disposal. Modifies global state and is thus not safe for
    /// concurrent tests.
    /// </summary>
    public class TempConsoleRedirect : IDisposable
    {
        private readonly TextWriter _backupStdout;
        private readonly TextWriter _backupStderr;

        public TempConsoleRedirect(TextWriter writer)
        {
            _backupStdout = Console.Out;
            _backupStderr = Console.Error;

            Console.SetOut(writer);
            Console.SetError(writer);
        }

        void IDisposable.Dispose()
        {
            Console.SetOut(_backupStdout);
            Console.SetOut(_backupStderr);
        }
    }
}
