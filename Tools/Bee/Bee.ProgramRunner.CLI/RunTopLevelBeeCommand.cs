using Bee.Core;

namespace Bee.ProgramRunner.CLI
{
    class RunTopLevelBeeCommand : TopLevelBeeCommand
    {
        public override string Name => "run";
        public override string Abbreviation => "r";
        public override string Description => "invoke the ProgramRunner tool";
        public override void Execute(string[] args)
        {
            ProgramRunnerCLI.RunnerMain(args);
        }
    }
}
