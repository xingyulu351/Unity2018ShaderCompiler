using NUnit.Framework;
using Unity.InternalCallRegistrationWriter.Runner;

namespace Unity.InternalCallRegistration.Tests
{
    [TestFixture]
    public class OptionTests
    {
        [Test]
        public void InputAssemblyOptionIsParsed()
        {
            var args = new string[1];
            const string assembly1Name = "assembly1.dll";
            const string assembly2Name = "assembly2.dll";
            args[0] = string.Format("-assembly={0};{1}", assembly1Name, assembly2Name);
            var options = InternalCallOptions.ParseOptionsFrom(args);
            Assert.AreEqual(options.InputAssemblies[0], assembly1Name);
            Assert.AreEqual(options.InputAssemblies[1], assembly2Name);
        }

        [Test]
        public void OutputCppIsParsed()
        {
            var args = new string[2];
            const string outputName = "ICallRegistration.cpp";
            const string assemblyName = "assembly.dll";
            args[0] = string.Format("-output={0}", outputName);
            args[1] = string.Format("-assembly={0}", assemblyName);
            var options = InternalCallOptions.ParseOptionsFrom(args);
            Assert.AreEqual(options.OutputFile, outputName);
        }

        [Test]
        public void DefaultOutputFileWorks()
        {
            var args = new string[1];
            const string assemblyName = "assembly.dll";
            args[0] = string.Format("-assembly={0}", assemblyName);
            var options = InternalCallOptions.ParseOptionsFrom(args);
            Assert.AreEqual(options.OutputFile, "RegisterIcalls.cpp");
        }
    }
}
