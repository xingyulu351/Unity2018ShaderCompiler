using System;
using JamSharp.JamState;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.Common;
using Unity.BuildSystem.JamStateToTundraConverter;

namespace Unity.BuildSystem.JamStateToBeeConverter.Tests
{
    [TestFixture]
    public class ConverterTests
    {
        [Test]
        public void AskForNonExistingNodeThrows()
        {
            var exception = Assert.Throws<Exception>(() => new Converter().Convert(new JamState(), new[] {"nonexisting"}, "mydag.json", new JamBackend()));
            StringAssert.Contains("Unknown target: nonexisting", exception.Message);
        }
    }
}
