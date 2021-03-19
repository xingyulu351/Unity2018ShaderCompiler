using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.Xcode;
using Unity.BuildSystem.NativeProgramSupport;

public static class MacNativeProgramExtensions
{
    public static ICustomizationFor<XcodeClangCompilerSettings> CompilerSettingsForMac(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<XcodeClangCompilerSettings>(nativeProgram, c => c.ToolChain.Platform is MacOSXPlatform);
    }

    public static ICustomizationFor<XcodeDynamicLinker> DynamicLinkerSettingsForMac(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<XcodeDynamicLinker>(nativeProgram, c => c.ToolChain.Platform is MacOSXPlatform);
    }
}
