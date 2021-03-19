using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.Android;
using Bee.Toolchain.GNU;
using Unity.BuildSystem.NativeProgramSupport;

public static class AndroidNativeProgramExtensions
{
    public static ICustomizationFor<AndroidNdkCompilerSettings> CompilerSettingsForAndroid(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<AndroidNdkCompilerSettings>(nativeProgram);
    }

    public static ICustomizationFor<AndroidDynamicLinker> DynamicLinkerSettingsForAndroid(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<AndroidDynamicLinker>(nativeProgram, c => c.ToolChain.Platform is AndroidPlatform);
    }
}
