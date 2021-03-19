using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Bee.Core;
using Bee.ProgramRunner.CLI;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport.Running.Tests
{
    [TestFixture]
    public class RunnerEnvironmentTests
    {
        private static IEnumerable<RunnerEnvironment> AllRunnerEnvironments()
        {
            // Don't use the Daemon here since we want to unit test the providers directly. (also there is no dependency to the Daemon in this test assembly)
            return DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<RunnerEnvironmentProvider>(RunnerEnvironmentProvider.AssemblyFilter)
                .SelectMany(provider => provider.ProvideInternal());
        }

        private static IEnumerable<Type> AllNonAbstractRunnerEnvironmentProviderTypes()
        {
            return DynamicLoader.GetAllTypesInAssemblies(RunnerEnvironmentProvider.AssemblyFilter).Where(t => !t.IsAbstract && t.IsSubclassOf(typeof(RunnerEnvironmentProvider)));
        }

        private static IEnumerable<RunnerEnvironment> NotOfflineRunnerEnvironments()
        {
            return AllRunnerEnvironments().Where(x => x.Status != RunnerEnvironment.EnvironmentStatus.Offline);
        }

        private static IEnumerable<RunnerEnvironment> FullAccessRunnerEnvironments()
        {
            return AllRunnerEnvironments().Where(x => x.Status == RunnerEnvironment.EnvironmentStatus.FullAccess);
        }

        [Test]
        public void CorrectHostRunnerEnvironmentExists()
        {
            var hostRuntimeEnvironments = AllRunnerEnvironments().Where(x => x.Platform == Platform.HostPlatform).ToArray();
            Assert.IsNotEmpty(hostRuntimeEnvironments);

            Assert.AreEqual(1, hostRuntimeEnvironments.Length);
            Assert.AreEqual(2, hostRuntimeEnvironments[0].SupportedArchitectures.Length);
            Assert.Contains(Architecture.x64, hostRuntimeEnvironments[0].SupportedArchitectures);
            Assert.Contains(Architecture.x86, hostRuntimeEnvironments[0].SupportedArchitectures);
            Assert.IsNull(hostRuntimeEnvironments[0].EnvironmentId); // Host environments have no identifier, since there is only one anyways.
        }

        // It's okay to have zero architectures for offline devices because maybe we still know of the existance of the device but couldn't query its architecture (because it is offline)
        [Test, TestCaseSource(nameof(NotOfflineRunnerEnvironments))]
        public void WithStatusNotOffline_HaveAtLeastOneSupportedArchitecture(RunnerEnvironment environment)
        {
            Assert.IsNotEmpty(environment.SupportedArchitectures);
        }

        [Test, TestCaseSource(nameof(FullAccessRunnerEnvironments))]
        public void WithStatusFullAccess_CanCreateRunner(RunnerEnvironment environment)
        {
            Assert.IsNotNull(environment.InstantiateRunner());
        }

        private static void AssertIsEqualDeepCompare(object obj1, object obj2)
        {
            if (obj1 == null && obj2 == null)
                return;

            Assert.IsNotNull(obj1 == null);
            Assert.IsNotNull(obj2 == null);
            Assert.AreSame(obj1.GetType(), obj2.GetType());

            var type = obj1.GetType();

            if (type.IsPrimitive || typeof(string) == type)
                Assert.AreEqual(obj1, obj2);
            else if (type.IsArray)
            {
                var array1 = (Array)obj1;
                var array2 = (Array)obj2;
                Assert.AreEqual(array1.Length, array2.Length);

                for (var i = 0; i < array1.Length; ++i)
                    AssertIsEqualDeepCompare(array1.GetValue(i), array2.GetValue(i));
            }
            else if (type == typeof(NPath))
                Assert.AreEqual(obj1, obj2);
            else
            {
                type.GetProperties(BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public).ForEach(f => AssertIsEqualDeepCompare(f.GetValue(obj1), f.GetValue(obj2)));
                type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public).ForEach(f => AssertIsEqualDeepCompare(f.GetValue(obj1), f.GetValue(obj2)));
            }
        }

        [Test, TestCaseSource(nameof(AllRunnerEnvironments))]
        public void CanSerializeAndDeserialize(RunnerEnvironment environment)
        {
            var json = environment.SerializeToJson();
            var newEnvironment = RunnerEnvironment.DeserializeFromJson(json);

            AssertIsEqualDeepCompare(environment, newEnvironment);
        }

        [Test, TestCaseSource(nameof(AllRunnerEnvironments))]
        public void CanWriteToStandardFilePath(RunnerEnvironment environment)
        {
            // Kill the Daemon, since touching its files without knowing about them can be dangerous.
            ProgramRunnerDaemon.KillDaemonIfRunning();

            Assert.That(() => environment.RunnerEnvironmentFilePath.WriteAllText(environment.SerializeToJson()), Throws.Nothing);
            Assert.That(environment.RunnerEnvironmentFilePath.FileExists(), Is.True);
        }

        [Test, TestCaseSource(nameof(AllNonAbstractRunnerEnvironmentProviderTypes))]
        public void ProviderHasVersionAttribute(Type type)
        {
            Assert.That(type, Has.Attribute<RunnerEnvironmentProviderVersion>());
        }
    }
}
