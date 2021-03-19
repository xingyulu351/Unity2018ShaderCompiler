using System.Collections.Generic;
using System.Linq;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    class BagOfObjectsFormat : NativeProgramFormat
    {
        public BagOfObjectsFormat(ObjectFileLinker linker) : base(linker)
        {
        }

        public override string Extension { get; } = "fake";

        protected override IEnumerable<NPath> CombineObjectFiles(NativeProgram nativeProgram, NativeProgramConfiguration config,
            IEnumerable<NPath> objectFiles, IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries, NPath linkTarget, NPath artifactsPath)
        {
            AllObjectFiles = objectFiles.ToArray();
            return new[] {linkTarget};
        }

        public NPath[] AllObjectFiles { get; private set; }
    }
}
