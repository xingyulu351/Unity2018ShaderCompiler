using System.Collections.Generic;
using NUnit.Framework;
using Unity.BuildSystem.CSharpSupport;

namespace Bee.CSharpSupport.Tests
{
    [TestFixture]
    public class CSharpProgramTests
    {
        [Test]
        public void TransitiveReferences()
        {
            var d2 = new CSharpProgram() {Path = "mydependency2.dll"};
            var d1 = new CSharpProgram() {Path = "mydepepndency1.dll", References = { d2}};
            var program = new CSharpProgram() { Path = "program.exe", References = { d1}};

            var transitiveReferences = CSharpProgram.AllTransitiveReferencesFor(new CSharpProgramConfiguration(CSharpCodeGen.Debug, null), new[] {program});

            CollectionAssert.AreEquivalent(new[] { d1, d2}, transitiveReferences);
        }
    }
}
