using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.LLVM;
using Unity.BuildSystem.NativeProgramSupport;

public static class ClangNativeProgramExtensions
{
    public static ICustomizationFor<ClangCompilerSettings> CompilerSettingsForClang(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<ClangCompilerSettings>(nativeProgram);
    }
}
