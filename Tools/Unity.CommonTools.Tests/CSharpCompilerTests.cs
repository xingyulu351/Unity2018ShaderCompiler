using System.IO;
using NUnit.Framework;

namespace Unity.CommonTools.Test
{
    [TestFixture]
    public class CSharpCompilerTests
    {
        [Test]
        public void CanCompileSimpleDLL()
        {
            var tmpdir = Paths.CreateTempDirectory();
            var dll = Paths.Combine(tmpdir, "test.dll");
            CSharpCompiler.CompileAssemblyFromSource(dll, "public class Hallo {}");
            Assert.IsTrue(File.Exists(dll));
        }
    }
}
