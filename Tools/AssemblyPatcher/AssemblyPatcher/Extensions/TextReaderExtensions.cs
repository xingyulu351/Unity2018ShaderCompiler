using System.Collections.Generic;
using System.IO;

namespace AssemblyPatcher.Extensions
{
    internal static class TextReaderExtensions
    {
        public static IEnumerable<string> Lines(this TextReader self)
        {
            var line = self.ReadLine();
            while (line != null)
            {
                yield return line;
                line = self.ReadLine();
            }
        }
    }
}
