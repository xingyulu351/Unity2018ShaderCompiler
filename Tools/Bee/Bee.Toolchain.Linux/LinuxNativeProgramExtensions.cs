using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.GNU;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Linux
{
    public static class LinuxNativeProgramExtensions
    {
        public static ICustomizationFor<LdDynamicLinker> DynamicLinkerSettingsForLinux(this NativeProgram nativeProgram)
        {
            return ObjectFileLinker.TypedLinkerCustomizationsFor<LdDynamicLinker>(nativeProgram, c => c.ToolChain.Platform is LinuxPlatform);
        }
    }
}
