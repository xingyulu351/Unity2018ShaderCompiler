using System;
using System.IO;
using NiceIO;

namespace Bee.TestHelpers
{
    public class TempChangeDirectory : IDisposable
    {
        readonly NPath m_PreviousDirectory;

        public TempChangeDirectory(NPath directory)
        {
            m_PreviousDirectory = NPath.CurrentDirectory;
            Directory.SetCurrentDirectory(directory.ToString(SlashMode.Native));
        }

        void IDisposable.Dispose()
        {
            Directory.SetCurrentDirectory(m_PreviousDirectory.ToString(SlashMode.Native));
        }
    }
}
