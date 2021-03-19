using System;
using System.IO;
using InternalCallRegistration.Tests.Fixtures.UnityFaux;
using NUnit.Framework;
using Unity.InternalCallRegistrationWriter.Runner;

namespace Unity.InternalCallRegistration.Tests
{
    [TestFixture]
    public class RunnerTests
    {
        [Test]
        public void CanOutputRegistrationsForUnityAssembly()
        {
            var location = UnityFauxFixtureLocation.Location();
            RunInternalCallWriter(location, GetTempFile());
        }

        [Test]
        [Ignore("for some reason")]
        public void RunForLocalDebug()
        {
            const string location = @"E:\unity2\build\WebGLSupport\Managed\UnityEngine.dll";
            RunInternalCallWriter(location, GetTempFile());
        }

        private static void RunInternalCallWriter(string location, string tempFile)
        {
            var args = new[]
            {
                string.Format("-assembly={0}", location),
                string.Format("-output={0}", tempFile),
            };

            DefaultRunner.ParseOptionsAndRun(args);
            Assert.IsTrue(File.Exists(tempFile));
            Console.Write(File.ReadAllText(tempFile));
        }

        private static string GetTempFile()
        {
            return string.Format("{0}.txt", Path.GetTempFileName());
        }
    }
}
