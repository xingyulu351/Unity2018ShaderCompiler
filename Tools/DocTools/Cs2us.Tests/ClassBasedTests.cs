using System.Collections.Generic;

namespace Cs2us.Tests
{
    public abstract class ClassBasedTests : BaseConversionTest
    {
        protected override string Convert(string input, bool explicitlyTyped = false, bool usePragmaStrict = false)
        {
            return Cs2UsUtils.ConvertClass(input, explicitlyTyped, usePragmaStrict);
        }

        protected IEnumerable<TestScenario> TestScenarios()
        {
            yield break;
        }
    }
}
