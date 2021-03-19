using System;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public enum CodeGen
    {
        Debug,
        Release,
        Master
    }

    public static class CodeGenExtensions
    {
        public static string ToJam(this CodeGen v)
        {
            switch (v)
            {
                case CodeGen.Debug: return "debug";
                case CodeGen.Release: return "release";
                case CodeGen.Master: return "master";
                default: throw new ArgumentOutOfRangeException(nameof(v), v, null);
            }
        }
    }
}
