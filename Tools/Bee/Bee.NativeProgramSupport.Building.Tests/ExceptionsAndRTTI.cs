using System.Linq;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class ExceptionsAndRTTI : NativeProgramTestsBase
    {
        [SetUp]
        public override void SetUp()
        {
            base.SetUp();
            NativeProgram.Sources.Add("myfile.cpp");
        }

        [Test]
        public void RTTI_False()
        {
            NativeProgram.RTTI.Set(false);
            Assert.IsFalse(OnlyCompilationUnit.CompilerSettings.RTTI);
        }

        [Test]
        public void RTTI_True()
        {
            NativeProgram.RTTI.Set(true);
            Assert.IsTrue(OnlyCompilationUnit.CompilerSettings.RTTI);
        }

        [Test]
        public void Exceptions_False()
        {
            NativeProgram.Exceptions.Set(false);
            Assert.IsFalse(OnlyCompilationUnit.CompilerSettings.Exceptions);
        }

        [Test]
        public void Exceptions_True()
        {
            NativeProgram.Exceptions.Set(true);
            Assert.IsTrue(OnlyCompilationUnit.CompilerSettings.Exceptions);
        }

        private CLikeCompilationUnitInstruction OnlyCompilationUnit => ObjectFileProductionInstructionses(NativeProgram).OfType<CLikeCompilationUnitInstruction>().Single();
    }
}
