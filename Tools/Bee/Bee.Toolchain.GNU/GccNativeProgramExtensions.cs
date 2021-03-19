using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.GNU;
using Unity.BuildSystem.NativeProgramSupport;

public static class GccNativeProgramExtensions
{
    public static ICustomizationFor<GccCompilerSettings> CompilerSettingsForGcc(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<GccCompilerSettings>(nativeProgram);
    }
}
