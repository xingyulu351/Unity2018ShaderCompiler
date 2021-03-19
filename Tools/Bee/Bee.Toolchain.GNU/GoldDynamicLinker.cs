using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.GNU
{
    public class GoldDynamicLinker : LdDynamicLinker
    {
        protected override string LdLinkerName { get; } = "gold";
        public GoldDynamicLinker(ToolChain toolchain, bool runThroughCompierFrontend) : base(toolchain, runThroughCompierFrontend) {}
    }
}
