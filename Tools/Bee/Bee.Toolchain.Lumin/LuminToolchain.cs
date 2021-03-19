using Bee.Toolchain.GNU;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Lumin
{
    public class LuminToolchain : ToolChain
    {
        public override NativeProgramFormat DynamicLibraryFormat => new LuminDynamicLibraryFormat(this);
        public override NativeProgramFormat ExecutableFormat => new LuminExecutableFormat(this);
        public override NativeProgramFormat StaticLibraryFormat => new GNUStaticLibraryFormat(this);
        public override CLikeCompiler CppCompiler => new LuminCompiler(this);

        public LuminToolchain(LuminSdk sdk) :
            base(sdk, sdk.Architecture)
        {
        }
    }
}
