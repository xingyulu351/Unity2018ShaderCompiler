using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.Xcode;
using Unity.BuildSystem.NativeProgramSupport;

public static class IOSNativeProgramExtensions
{
    public static ICustomizationFor<XcodeClangCompilerSettings> CompilerSettingsForIos(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<XcodeClangCompilerSettings>(nativeProgram,
            c => c.ToolChain.Platform is IosPlatform);
    }

    public static ICustomizationFor<XcodeClangCompilerSettings> CompilerSettingsForTvos(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<XcodeClangCompilerSettings>(nativeProgram,
            c => c.ToolChain.Platform is TvosPlatform);
    }

    public static ICustomizationFor<XcodeClangCompilerSettings> CompilerSettingsForIosOrTvos(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<XcodeClangCompilerSettings>(nativeProgram,
            c => c.ToolChain.Platform is IosPlatform || c.ToolChain.Platform is TvosPlatform);
    }

    public static ICustomizationFor<XcodeDynamicLinker> DynamicLinkerSettingsForIos(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<XcodeDynamicLinker>(nativeProgram, c => c.ToolChain.Platform is IosPlatform);
    }

    public static ICustomizationFor<XcodeDynamicLinker> DynamicLinkerSettingsForIosOrTvos(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<XcodeDynamicLinker>(nativeProgram, c => c.ToolChain.Platform is IosPlatform || c.ToolChain.Platform is TvosPlatform);
    }
}
