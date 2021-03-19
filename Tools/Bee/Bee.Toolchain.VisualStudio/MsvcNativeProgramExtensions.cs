using Bee.NativeProgramSupport.Building;
using Bee.Toolchain.VisualStudio;
using Unity.BuildSystem.NativeProgramSupport;

public static class MsvcNativeProgramExtensions
{
    public static ICustomizationFor<MsvcCompilerSettings> CompilerSettingsForMsvc(this NativeProgram nativeProgram)
    {
        return CLikeCompiler.TypedCompilerCustomizationsFor<MsvcCompilerSettings>(nativeProgram);
    }

    public static ICustomizationFor<MsvcDynamicLinker> DynamicLinkerSettingsForMsvc(this NativeProgram nativeProgram)
    {
        return ObjectFileLinker.TypedLinkerCustomizationsFor<MsvcDynamicLinker>(nativeProgram);
    }
}
