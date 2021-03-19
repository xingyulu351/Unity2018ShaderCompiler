using System;

namespace AssemblyPatcher
{
    public class PatchingException : Exception
    {
        public PatchingException(string msg) : base(msg)
        {
        }
    }
}
