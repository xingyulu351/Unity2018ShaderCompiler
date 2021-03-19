using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.Lumin;
using Unity.BuildSystem.NativeProgramSupport;

public static class LuminNativeProgramExtensions
{
    public static ICustomizationFor<LuminCompilerSettings> CompilerSettingsForLumin(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<LuminCompilerSettings>(nativeProgram, c => c.ToolChain.Platform is LuminPlatform);
    }

    public static ICustomizationFor<LuminDynamicLinker> DynamicLinkerSettingsForLumin(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<LuminDynamicLinker>(nativeProgram, c => c.ToolChain.Platform is LuminPlatform);
    }
}
