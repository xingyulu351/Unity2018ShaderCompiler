using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Bee.Core.Tests;
using Bee.TundraBackend.Tests;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.NativeProgramAndToolchainTests.IntegrationTests
{
    public class ToolchainAndBackendTestBase
    {
        private static IEnumerable<ToolChain> ToolchainsThatCanBuild => _toolchainsThatCanBuild.Value;
        private static readonly Lazy<ToolChain[]> _toolchainsThatCanBuild = new Lazy<ToolChain[]>(() =>
            ToolchainsForTestsDiscovery.All.Where(t => t.CanBuild).ToArray());

        private static IEnumerable<ToolChain> ToolchainsThatCanBuildAndRun => _toolchainsThatCanBuildAndRun.Value;
        private static readonly Lazy<ToolChain[]> _toolchainsThatCanBuildAndRun = new Lazy<ToolChain[]>(() =>
            ToolchainsThatCanBuild.Where(t => t.CanRun).ToArray());

        protected ToolChain Toolchain { get; set; }
        protected NPath TestRoot { get; set; }
        protected BeeAPITestBackend<ToolChain>  TestBackend { get; set; }

        private static BeeAPITestBackend<ToolChain>[] FindAllBackends()
        {
            return BeeTestApiBackendsLocator.AllBeeApiTestBackends<ToolChain>()

                //Let's stop running allbackends tests on standalone bee. it is too identical to the tundra backend that it brings
                //little value. We test the proper functioning of standalone bee in the backend tests, and that is sufficient coverage
                .Where(b => b.Name != "StandaloneBee")
                .ToArray();
        }

        protected static IEnumerable RunnableToolchains_FastestBackend()
        {
            return TestCaseDataFor(ToolchainsThatCanBuildAndRun, new BeeAPITestBackend_Tundra<ToolChain>().AsEnumerable());
        }

        protected static IEnumerable AllBackends()
        {
            return FindAllBackends().Select(testBackend => new TestCaseData(testBackend).SetName($"{{m}}__{testBackend.Name}")).Cast<object>();
        }

        protected static IEnumerable RunnableToolchains_AllBackends()
        {
            return TestCaseDataFor(ToolchainsThatCanBuildAndRun, FindAllBackends());
        }

        protected static IEnumerable AllToolchains_FastestBackend()
        {
            var backends = new[] {new BeeAPITestBackend_Tundra<ToolChain>()};

            return TestCaseDataFor(ToolchainsThatCanBuild, backends);
        }

        protected static IEnumerable AllToolchains_AllBackends()
        {
            var allToolchains = ToolchainsThatCanBuild;
            var backends = FindAllBackends();

            return TestCaseDataFor(allToolchains, backends);
        }

        protected static IEnumerable SingleToolchain_AllBackends()
        {
            var allToolchains = ToolchainsThatCanBuildAndRun.OrderByDescending(t => t.SpeedScore).First().AsEnumerable();
            var backends = FindAllBackends();

            return TestCaseDataFor(allToolchains, backends);
        }

        private static IEnumerable TestCaseDataFor(IEnumerable<ToolChain> allToolchains, IEnumerable<BeeAPITestBackend<ToolChain>> backends)
        {
            foreach (var toolchain in allToolchains)
                foreach (var backend in backends)
                    yield return TestCaseDataFor(toolchain, backend);
        }

        private static TestCaseData TestCaseDataFor(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            return new TestCaseData(toolchain, testBackend).SetName(
                $"{{m}}_{toolchain.ActionName}_{testBackend.Name}");
        }

        protected void ManualFixtureSetup(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            testBackend.Start();
            Toolchain = toolchain;
            TestBackend = testBackend;
            TestRoot = testBackend.TestRoot;
        }
    }
}
