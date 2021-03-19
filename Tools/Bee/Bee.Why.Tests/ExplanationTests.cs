using System;
using System.Collections.Generic;
using System.IO;
using Bee.Core.Tests;
using NUnit.Framework;

namespace Bee.Why.Tests
{
    [TestFixture]
    public class ExplanationTests
    {
        private Explanation MakeSingleNodeExplanation(string nodeName, params RebuildReason[] reasons)
        {
            var node = new BuiltNodeInfo(nodeName, 0, reasons);
            return new Explanation(node, new DependencyChain(new[] {new Tuple<BuiltNodeInfo, DependencyChain.Kind>(node, DependencyChain.Kind.None)}));
        }

        private string GetStdOutFrom(Action action)
        {
            using (var writer = new StringWriter())
            {
                var oldOut = Console.Out;
                try
                {
                    Console.SetOut(writer);
                    action();
                    return writer.ToString();
                }
                finally
                {
                    Console.SetOut(oldOut);
                }
            }
        }

        [Test]
        public void NotRebuiltBefore()
        {
            var explanation = MakeSingleNodeExplanation("TestNode", new NotBuiltBeforeReason());
            var output = GetStdOutFrom(() => explanation.FormatToConsole());
            Assert.That(output, new MultilineStringMatchesSpecConstraint(
                "",
                "This is why TestNode was built:",
                "",
                " TestNode, requested as a final build target, was built because:",
                " |",
                " | The node hadn't been built before."
            ));
        }

        [Test]
        public void SeveralInputsChanged()
        {
            var explanation = MakeSingleNodeExplanation("TestNode", new InputHashChangedReason("main.cs"), new InputTimeStampChangedReason("main.txt"));
            var output = GetStdOutFrom(() => explanation.FormatToConsole());
            Assert.That(output, new MultilineStringMatchesSpecConstraint(
                "",
                "This is why TestNode was built:",
                "",
                " TestNode, requested as a final build target, was built because:",
                " |",
                " |- main.cs changed (timestamp and sha1)",
                " |- main.txt changed (timestamp)"
            ));
        }

        [Test]
        public void ActionChanged()
        {
            var explanation = MakeSingleNodeExplanation("TestNode", new ActionChangedReason("the thing it did first", "the thing it does now"));
            var output = GetStdOutFrom(() => explanation.FormatToConsole());
            Assert.That(output, new MultilineStringMatchesSpecConstraint(
                "",
                "This is why TestNode was built:",
                "",
                " TestNode, requested as a final build target, was built because:",
                " |",
                " | The action for the node changed.",
                " | Old action:",
                " |  the thing it did first",
                " |",
                " | New action:",
                " |  the thing it does now"
            ));
        }

        [Test]
        public void ResponseFileChanged()
        {
            var explanation = MakeSingleNodeExplanation("TestNode", new ResponseFileChangedReason("the thing it did first", "the thing it does now"));
            var output = GetStdOutFrom(() => explanation.FormatToConsole());
            Assert.That(output, new MultilineStringMatchesSpecConstraint(
                "",
                "This is why TestNode was built:",
                "",
                " TestNode, requested as a final build target, was built because:",
                " |",
                " | The response file for the node changed.",
                " | Old response file content:",
                " |  the thing it did first",
                " |",
                " | New response file content:",
                " |  the thing it does now"
            ));
        }

        [Test]
        public void DependencyChanged()
        {
            var childNode = new BuiltNodeInfo("ChildNode", 0, new RebuildReason[] {new ResponseFileChangedReason("old", "new")});
            var parentNode = new BuiltNodeInfo("ParentNode", 1,
                new RebuildReason[] {new DependencyChanged(childNode, new InputHashChangedReason("ChildNode"))});

            var explanation = new Explanation(parentNode, new DependencyChain(new[] { new Tuple<BuiltNodeInfo, DependencyChain.Kind>(parentNode, DependencyChain.Kind.None) }));

            var output = GetStdOutFrom(() => explanation.FormatToConsole());

            Assert.That(output, new MultilineStringMatchesSpecConstraint(
                "",
                "This is why ParentNode was built:",
                "",
                " ParentNode, requested as a final build target, was built because:",
                " |",
                " |- ChildNode is an input and was rebuilt",
                "    |",
                "    | The response file for the node changed.",
                "    | Old response file content:",
                "    |  old",
                "    |",
                "    | New response file content:",
                "    |  new"
            ));
        }

        [Test]
        public void VeryLongTokensArePutOnNewLinesAndSplit()
        {
            var explanation = MakeSingleNodeExplanation("TestNode", new ActionChangedReason("Token1", "Token1 VeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongToken"));
            var output = GetStdOutFrom(() => explanation.FormatToConsole());
            Assert.That(output, new MultilineStringMatchesSpecConstraint(
                "",
                "This is why TestNode was built:",
                "",
                " TestNode, requested as a final build target, was built because:",
                " |",
                " | The action for the node changed.",
                " | Old action:",
                " |  Token1",
                " |",
                " | New action:",
                " |  Token1",
                " |  VeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVery",
                " |  VeryVeryLongToken"
            ));
        }
    }
}
