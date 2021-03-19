using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.Why;

namespace Bee.StandaloneDriver
{
    // Invoked through reflection in Bee.StandaloneDriver
    internal class WhyTopLevelBeeCommand : TopLevelBeeCommand
    {
        public override string Name => "why";
        public override string Abbreviation => "w";
        public override string Description => "explain why the given target was built";
        public override void Execute(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("Why what?");
                Environment.Exit(1);
            }

            if (args.Length > 1)
            {
                Console.WriteLine("Only explaining one target at a time is supported.");
                Environment.Exit(2);
            }

            var tundraBackend = new TundraBackend.TundraBackend(Configuration.RootArtifactsPath);
            var explainer = new TundraExplainer(tundraBackend);

            var searchString = args.Single();

            Explanation explanation;
            try
            {
                explanation = explainer.Explain(searchString);
            }
            catch (KeyNotFoundException)
            {
                Console.WriteLine($"Couldn't figure out which node you meant by \"{searchString}\".");
                Environment.Exit(3);
                return;
            }

            explanation.FormatToConsole();
        }
    }
}
