using System.IO;
using ICSharpCode.NRefactory.CSharp;

namespace Cs2us.Tests
{
    public abstract class ResourceBasedTests : ClassBasedTests
    {
        protected void AssertConversionFromResource(string testName)
        {
            var scenario = TestEspectFromResource(testName);
            AssertConversion(scenario.Input, scenario.Expected, scenario.ExplicitlyTypedVariables);
        }

        private TestScenario TestEspectFromResource(string testName)
        {
            var fullPath = Path.Combine("TestScenarios", TestType, testName + ".cs");
            var contents = File.ReadAllText(fullPath);
            var parser = new CSharpParser();
            var syntaxTree = parser.Parse(contents);
            var extractor = new ExpectationExtractor();
            syntaxTree.AcceptVisitor(extractor);
            return new TestScenario(testName, contents, extractor.Content, true);
        }

        protected abstract string TestType
        {
            get;
        }
    }

    internal class ExpectationExtractor : DepthFirstAstVisitor
    {
        public override void VisitComment(Comment comment)
        {
            if (comment.CommentType == CommentType.MultiLine)
                Content = comment.Content.Trim('\r', '\n');
        }

        public string Content { get; private set; }
    }
}
