using Bee.Core;
using Bee.Stevedore.Program;

namespace Bee.Stevedore
{
    class SteveTopLevelBeeCommand : TopLevelBeeCommand
    {
        public override string Name => "steve";
        public override string Abbreviation => "s";
        public override string Description => "invoke the Stevedore artifact manager";
        public override void Execute(string[] args) => StevedoreProgram.SteveMain(args);
    }
}
