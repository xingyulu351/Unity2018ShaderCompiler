using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.VisualStudio;
using Unity.BuildSystem.NativeProgramSupport;

public static class WindowsNativeProgramExtensions
{
    public static ICustomizationFor<MsvcDynamicLinker> DynamicLinkerSettingsForWindows(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<MsvcDynamicLinker>(nativeProgram, c => c.Platform is WindowsPlatform);
    }
}
