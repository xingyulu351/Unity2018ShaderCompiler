using System.Text.RegularExpressions;
using Bee.Core;
using Bee.Core.Tests;
using Bee.ToolchainTestInfrastructure;
using Bee.Toolchain.Emscripten;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Emscripten.Tests
{
    [TestFixture]
    class EmscriptenToolChainTests : SingleToolchainTests
    {
        protected override ToolChain ProduceToolchainToTest() => new EmscriptenToolchain(EmscriptenSdk.Locator.LatestSdk, new AsmJsArchitecture());

        static string StripNewlines(string s)
        {
            return s.Replace("\n", "").Replace("\r", "");
        }

        [Test]
        public void PreJsLibIsPrepended()
        {
            MainCpp.WriteAllText("#include <stdio.h>\n int main() { printf(\"hello\"); return 0; }");

            var myprejscontents = "Module.HELLOWORLD = 1;";
            var jsfile = TestRoot.Combine("jsfile.js");
            jsfile.WriteAllText(myprejscontents);

            Build(toolchain => SetupNativeProgram(toolchain, np => np.PrebuiltLibraries.Add(new PreJsLibrary(jsfile))));

            var output = StripNewlines(TestRoot.Combine("program.js").ReadAllText());
            // check for the token, and make sure it comes before moduleOverrides (--pre-js)
            StringAssert.IsMatch(@"HELLOWORLD.*moduleOverrides", output);
        }

        [Test]
        public void PostJsLibIsAppended()
        {
            MainCpp.WriteAllText("#include <stdio.h>\n int main() { printf(\"hello\"); return 0; }");

            var myprejscontents = "Module.HELLOWORLD = 1;";
            var jsfile = TestRoot.Combine("jsfile.js");
            jsfile.WriteAllText(myprejscontents);

            Build(toolchain => SetupNativeProgram(toolchain, np => np.PrebuiltLibraries.Add(new PostJsLibrary(jsfile))));

            var output = StripNewlines(TestRoot.Combine("program.js").ReadAllText());
            // check for the token, and make sure it comes after shouldRunNow mention (--post-js)
            StringAssert.IsMatch(@"shouldRunNow.*HELLOWORLD", output);
        }

        [Test]
        public void JavascriptLibraryIsIntegrated()
        {
            MainCpp.WriteAllText("#include <stdio.h>\n extern \"C\" int testCall(); int main() { return testCall(); }");

            var jsfile = TestRoot.Combine("jslib.js");
            jsfile.WriteAllText(@"mergeInto(LibraryManager.library, { testCall: function() { return 1234567; } });");

            Build(toolchain => SetupNativeProgram(toolchain, np => np.PrebuiltLibraries.Add(new JavascriptLibrary(jsfile))));

            var output = StripNewlines(TestRoot.Combine("program.js").ReadAllText());
            // check that the library was merged and that our call was defined as global function properly
            StringAssert.IsMatch(@"function _testCall.*1234567", output);
        }
    }
}
