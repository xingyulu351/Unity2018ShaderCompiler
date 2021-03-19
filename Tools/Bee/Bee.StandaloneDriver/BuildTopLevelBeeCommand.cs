using Bee.Core;

namespace Bee.StandaloneDriver
{
    class BuildTopLevelBeeCommand : TopLevelBeeCommand
    {
        public override string Name => "build";
        public override string Abbreviation => "b";
        public override string Description => "build the given targets";
        public override void Execute(string[] args) => StandaloneBeeDriver.BuildMain(args);
    }
}
