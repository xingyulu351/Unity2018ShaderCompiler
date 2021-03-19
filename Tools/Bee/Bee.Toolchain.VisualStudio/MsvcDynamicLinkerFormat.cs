using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.VisualStudio
{
    public abstract class MsvcDynamicLinkerFormat : NativeProgramFormat
    {
        protected MsvcDynamicLinkerFormat(MsvcDynamicLinker linker) : base(linker) {}
    }
}
