namespace Bee.Core
{
    public abstract class TopLevelBeeCommand
    {
        public abstract string Name { get; }
        public abstract string Abbreviation { get; }
        public abstract string Description { get; }

        public abstract void Execute(string[] args);
    }
}
