using System.Linq;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.ProgramRunner.CLI.Tests
{
    [TestFixture]
    public class RunnerEnvironmentFindTests
    {
        [Test]
        public void Successful_ForHostEnvironmentByPlatform()
        {
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments(Platform.HostPlatform).Count());
        }

        [Test]
        public void Successful_ForHostEnvironmentByPlatformAndArchitecture()
        {
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments(Platform.HostPlatform, new[] { Architecture.x64 }).Count());
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments(Platform.HostPlatform, new[] { Architecture.x86 }).Count());
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments(Platform.HostPlatform, new Architecture[] { Architecture.x86, Architecture.x64 }).Count());
        }

        [Test]
        public void ByStringSuccessful_ForHostEnvironmentByPlatform()
        {
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.Name}").Count());
        }

        [Test]
        public void ByStringSuccessful_ForHostEnvironmentByPlatform_WithDisplayName()
        {
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.DisplayName}").Count());
        }

        [Test]
        public void ByStringSuccessful_ForHostEnvironmentByPlatformAndArchitecture()
        {
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.Name}@{Architecture.x64.Name}").Count());
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.Name}@{Architecture.x86.Name}").Count());
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.Name}@{Architecture.x64.Name},{Architecture.x86.Name}").Count());
        }

        [Test]
        public void ByStringSuccessful_ForHostEnvironmentByPlatformAndArchitecture_WithDisplayName()
        {
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.DisplayName}@{Architecture.x64.DisplayName}").Count());
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.DisplayName}@{Architecture.x86.DisplayName}").Count());
            Assert.AreEqual(1, ProgramRunnerDaemon.FindRunnerEnvironments($"{Platform.HostPlatform.DisplayName}@{Architecture.x64.DisplayName},{Architecture.x86.DisplayName}").Count());
        }
    }
}
