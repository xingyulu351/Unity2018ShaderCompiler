using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building
{
    public abstract class ToolChainForHostProvider
    {
        public abstract ToolChain Provide();
    }
}
