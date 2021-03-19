using System.Linq;
using Bee.Core;
using Moq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    public class NativeProgramTestsBase
    {
        protected NativeProgram NativeProgram;

        [SetUp]
        public virtual void SetUp()
        {
            Backend.Current = new Mock<IBackend>().Object;
            NativeProgram = new NativeProgram("testProgram");
        }

        protected static ObjectFileProductionInstructions[] ObjectFileProductionInstructionses(NativeProgram a)
        {
            var toolchain = new MockToolchain();
            var format = new MockFormat(toolchain);
            var config = new NativeProgramConfiguration(CodeGen.Debug, toolchain, true);
            return a.ProduceObjectFileProductionInstructions(null, config, format, "artifacts", toolchain.ObjectFileProducers, out NPath[] additionalObjectFiles).ToArray();
        }
    }
}
