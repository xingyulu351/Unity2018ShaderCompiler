using System.Collections.Generic;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class TestableToolchainProvider
    {
        public abstract IEnumerable<ToolChain> Provide();
    }
}
