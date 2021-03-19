using System.Collections.Generic;
using System.IO;

namespace AssemblyPatcher.Tests
{
    public class AssemblyPatcherTestSpec
    {
        public AssemblyPatcherTestSpec(string assemblyPath, Stream configContent, string unpatchedExpectation, string patchedExpectation)
        {
            AssemblyPath = assemblyPath;
            Config = configContent;
            UnpatchedExpectation = unpatchedExpectation;
            PatchedExpectation = patchedExpectation;
        }

        public string AssemblyPath
        {
            get; private set;
        }

        public Stream Config
        {
            get; private set;
        }

        public string UnpatchedExpectation { get; set; }

        public string PatchedExpectation { get; set; }
    }
}
