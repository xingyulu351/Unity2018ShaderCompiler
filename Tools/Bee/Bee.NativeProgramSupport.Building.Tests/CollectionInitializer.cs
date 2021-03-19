using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class CollectionInitializer
    {
        [Test]
        public void CommandToBuild_Lambda()
        {
            var np = new NativeProgram("program")
            {
                CommandToBuild = {c => $"mycommand_{c.CodeGen}"}
            };
            Assert.AreEqual("mycommand_Debug", np.CommandToBuild.For(new NativeProgramConfiguration(CodeGen.Debug, null, false)));
        }

        [Test]
        public void CommandToBuild_Value()
        {
            var np = new NativeProgram("program")
            {
                CommandToBuild = {"mycommand"}
            };
            Assert.AreEqual("mycommand", np.CommandToBuild.For(new NativeProgramConfiguration(CodeGen.Debug, null, false)));
        }
    }
}
