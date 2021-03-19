namespace Cs2us.Tests
{
    public struct TestScenario
    {
        public string Input;
        public string Expected;
        public readonly string Description;
        public bool ExplicitlyTypedVariables;
        public readonly bool PragmaStrict;

        public TestScenario(string description, string input, string expected, bool expl = false, bool pragmastrict = false)
        {
            Input = input;
            Expected = expected;
            Description = description;
            ExplicitlyTypedVariables = expl;
            PragmaStrict = pragmastrict;
        }

        public override string ToString()
        {
            return Description;
        }
    }
}
