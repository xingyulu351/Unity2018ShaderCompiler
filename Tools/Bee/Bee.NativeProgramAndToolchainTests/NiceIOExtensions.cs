using System;
using NiceIO;

namespace Bee.NativeProgramAndToolchainTests
{
    static class NiceIOExtensions
    {
        public static NPath BackDate(this NPath path, TimeSpan? howMuch = null)
        {
            path.SetLastWriteTimeUtc(DateTime.Now - (howMuch ?? TimeSpan.FromMinutes(12345)));
            return path;
        }
    }
}
