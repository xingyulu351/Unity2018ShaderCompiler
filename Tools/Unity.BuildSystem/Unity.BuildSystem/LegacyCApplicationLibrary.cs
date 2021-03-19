using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem
{
    public sealed class LegacyCApplicationLibrary : PrecompiledLibrary
    {
        public override bool ToolchainSpecific => false;

        public override bool System { get; }
        public override bool Static { get; }
        public override bool Dynamic { get; }

        public LegacyCApplicationLibrary(NPath path)
        {
            // leave out .lib since win applies this for sysem libs too
            if (path.Depth == 1 && !path.HasExtension("a", "bc", "la"))
            {
                System = true;
                _Name = path.FileName;
                return;
            }
            _Path = path;
            Static = Path.HasExtension("a", "bc", "la", "lib");
            Dynamic = Path.HasExtension("dll", "dylib", "so") || Path.FileName.Contains(".so.");
        }
    }
}
