using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.Emscripten;
using Unity.BuildSystem.NativeProgramSupport;

public static class EmscriptenNativeProgramExtensions
{
    public static ICustomizationFor<EmscriptenCompilerSettings> CompilerSettingsForEmscripten(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<EmscriptenCompilerSettings>(nativeProgram);
    }

    public static ICustomizationFor<EmscriptenDynamicLinker> DynamicLinkerSettingsForEmscripten(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<EmscriptenDynamicLinker>(nativeProgram, c => c.ToolChain.Platform is WebGLPlatform);
    }
}
