using System.IO;
using NUnit.Framework;

namespace Cs2us.Tests
{
    public class BaseConversionTest
    {
        //private const bool compileOutput = false;

        protected void AssertConversion(string input, string expected,  bool explicitlyTyped, bool usePragmaStrict = false, bool compileOutput = false)
        {
            var actualUsOutput = Convert(input, explicitlyTyped, usePragmaStrict);
            if (usePragmaStrict)
                expected = "#pragma strict\n" + expected;
            Assert.AreEqual(expected.TrimStart().CollapseWhitespace(), actualUsOutput.CollapseWhitespace());
            if (compileOutput)
                AssertUnityScriptCompilation(actualUsOutput);
        }

        protected virtual string Convert(string input, bool explicitlyTypedVariables = false, bool usePragmaStrict = false)
        {
            return Cs2UsUtils.ConvertStuff(input, explicitlyTypedVariables, usePragmaStrict);
        }

        //compiles UnityScript
        public static void AssertUnityScriptCompilation(string scriptContent)
        {
            var scriptPath = Path.GetTempFileName();
            File.Delete(scriptPath);
            File.WriteAllText(scriptPath, scriptContent);
            var usCompilerOutput = UnityScriptUtils.Compilation.CompileUnityScript(scriptPath);
            Assert.IsNull(usCompilerOutput, "expecting null (no errors)");
        }
    }
}
