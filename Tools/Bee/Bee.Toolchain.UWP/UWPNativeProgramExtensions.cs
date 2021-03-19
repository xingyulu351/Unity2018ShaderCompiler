using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.VisualStudio;
using Unity.BuildSystem.NativeProgramSupport;

public static class UWPNativeProgramExtensions
{
    public static ICustomizationFor<MsvcDynamicLinker> DynamicLinkerSettingsForUWP(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<MsvcDynamicLinker>(nativeProgram, c => c.Platform is UniversalWindowsPlatform);
    }
}
