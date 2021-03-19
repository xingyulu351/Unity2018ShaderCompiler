// This is supposed to represent a "best practice" example of creating
// the default Bee downloadable Emscripten toolchain.

using System.Linq;
using Bee.NativeProgramSupport.Building.FluentSyntaxHelpers;
using Bee.Toolchain.Emscripten;
using Unity.BuildSystem.NativeProgramSupport;

class HelloWorldEmscripten
{
    static void Main()
    {
        var np = new NativeProgram("HelloWorld");
        np.Sources.Add("src");

        var toolchain = ToolChain.Store.Emscripten().Sdk_1_38_3().AsmJS();

        var nativeProgramConfiguration = new NativeProgramConfiguration(CodeGen.Debug, toolchain, lump: true);
        np.SetupSpecificConfiguration(nativeProgramConfiguration, toolchain.ExecutableFormat, targetDirectory: "build");
    }
}
