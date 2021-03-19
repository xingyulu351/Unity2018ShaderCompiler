using System;
using System.IO;
using System.Text;

namespace AssemblyPatcher.Tests.Extensions
{
    public static class StringExtensions
    {
        public static Stream AsStream(this string self)
        {
            if (self == null)
                throw new NullReferenceException("self");

            return new MemoryStream(Encoding.ASCII.GetBytes(self));
        }
    }
}
