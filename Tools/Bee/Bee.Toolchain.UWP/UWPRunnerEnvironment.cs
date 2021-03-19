using System.Linq;
using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.UWP
{
    public class UWPRunnerEnvironment : RunnerEnvironment
    {
        public UWPRunnerEnvironment(Architecture[] supportedArchitectures) : base(new UniversalWindowsPlatform(), supportedArchitectures, supportedArchitectures.Select(x => x.DisplayName).SeparateWith("."))
        {}

        public override Runner InstantiateRunner(string runnerArgs = null) => new UWPRunner(this);
    }
}
