using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Bee.Why
{
    internal class Explanation
    {
        public BuiltNodeInfo NodeBeingExplained { get; }
        public DependencyChain DependencyChain { get; }

        public RebuildReason[] RebuildReasons { get; }

        public Explanation(BuiltNodeInfo nodeBeingExplained, DependencyChain dependencyChain)
        {
            NodeBeingExplained = nodeBeingExplained ?? throw new ArgumentNullException(nameof(nodeBeingExplained));
            DependencyChain = dependencyChain ?? throw new ArgumentNullException(nameof(dependencyChain));
            RebuildReasons = nodeBeingExplained.RebuildReasons.ToArray();
        }

        private struct Color : IDisposable
        {
            public Color(ConsoleColor color)
            {
                Console.ForegroundColor = color;
            }

            public void Dispose()
            {
                Console.ResetColor();
            }
        }

        private void FormatTokenListToConsole(string prefix, int width, TokenDiffer.Token[] tokens)
        {
            int spaceRemaining = width;
            Console.Write(prefix);
            foreach (var token in tokens)
            {
                // If the token is too long for this line, and we did already put some content on this line,
                // start a new line.
                if (token.Length > spaceRemaining && spaceRemaining < width)
                {
                    Console.WriteLine();
                    Console.Write(prefix);
                    spaceRemaining = width;
                }

                // If this isn't the first token we've written this line, write a space
                if (spaceRemaining < width)
                {
                    spaceRemaining--;
                    Console.Write(' ');
                }

                // If it's still too long, split it across multiple lines
                if (token.Length > spaceRemaining)
                {
                    int startIndex = 0;
                    while (true)
                    {
                        int lineLength = Math.Min(spaceRemaining, token.Length - startIndex);
                        using (new Color(token.Color))
                            Console.Write(token.Value.Substring(startIndex, lineLength));
                        startIndex += lineLength;
                        spaceRemaining -= lineLength;

                        if (startIndex >= token.Length)
                            break;

                        Console.WriteLine();
                        Console.Write(prefix);
                        spaceRemaining = width;
                    }
                }
                else
                {
                    spaceRemaining -= token.Length;
                    using (new Color(token.Color))
                        Console.Write(token.Value);
                }
            }

            Console.WriteLine();
        }

        private void FormatTokenDiffsToConsole(string oldValue, string newValue, string title, string prefix, int width)
        {
            TokenDiffer.Token[] oldTokens;
            TokenDiffer.Token[] newTokens;

            try
            {
                TokenDiffer.Diff(oldValue, newValue, out oldTokens, out newTokens);
            }
            catch (InvalidDataException)
            {
                Console.Write($"{prefix} The content appears to be the same, but ");
                using (new Color(ConsoleColor.Red))
                    Console.WriteLine("there are whitespace differences.");
                return;
            }

            Console.WriteLine($"{prefix} Old {title}:");
            FormatTokenListToConsole(prefix + "  ", width, oldTokens);

            Console.WriteLine(prefix);

            Console.WriteLine($"{prefix} New {title}:");
            FormatTokenListToConsole(prefix + "  ", width, newTokens);
        }

        public void FormatToConsole()
        {
            Console.WriteLine();
            Console.Write("This is why ");
            using (new Color(ConsoleColor.Blue))
                Console.Write(NodeBeingExplained.Annotation);
            Console.WriteLine(" was built:");
            Console.WriteLine();

            var chain = DependencyChain.Nodes.ToArray();
            var prefix = "";
            for (int i = 0; i < chain.Length; ++i)
            {
                Console.Write(prefix.ToArray());

                if (chain[i].Item1.OriginalDagIndex == NodeBeingExplained.OriginalDagIndex)
                    Console.ForegroundColor = ConsoleColor.Blue;
                Console.Write($" {chain[i].Item1.Annotation}");
                Console.ResetColor();

                if (i == 0)
                    Console.Write(", requested as a final build target,");
                else
                    Console.Write(", which");

                if (i < chain.Length - 1)
                {
                    if (chain[i].Item2 == DependencyChain.Kind.Uses)
                        Console.Write(" takes as an input the output of:");
                    else if (chain[i].Item2 == DependencyChain.Kind.Depends)
                        Console.Write(" has a declared dependency on:");
                }
                else
                    Console.Write(" was built because:");

                Console.WriteLine();

                if (prefix.Length == 0)
                    prefix = " |-";
                else
                    prefix = "   " + prefix;
            }

            prefix = prefix.TrimEnd('-');
            Console.WriteLine(prefix);

            var alreadyExplainedNodes = new HashSet<int>();
            alreadyExplainedNodes.Add(NodeBeingExplained.OriginalDagIndex);
            FormatRebuildReasonsToConsole(prefix, RebuildReasons, alreadyExplainedNodes);

            Console.WriteLine();
        }

        private void FormatRebuildReasonsToConsole(string prefix, IList<RebuildReason> rebuildReasons, HashSet<int> alreadyExplainedNodes)
        {
            // Sort the reasons into a more helpful sequence
            rebuildReasons = rebuildReasons.OrderBy(r =>
            {
                if (r is InputChangedReason)
                    return 1;
                if (r is ActionChangedReason)
                    return 2;
                if (r is ResponseFileChangedReason)
                    return 3;
                if (r is DependencyChanged)
                    return 4;
                return 5;
            }).ToList();

            // Do the InputChanged reasons first as they're the most common and we want tighter spacing
            foreach (var inputChanged in rebuildReasons.OfType<InputChangedReason>())
            {
                Console.Write($"{prefix}- {inputChanged.File} ");
                using (new Color(ConsoleColor.Red))
                    Console.WriteLine($"changed ({inputChanged.Method})");
            }

            if (rebuildReasons.Any(r => r is InputChangedReason) && !rebuildReasons.All(r => r is InputChangedReason))
                Console.WriteLine(prefix);

            for (int reasonIndex = 0; reasonIndex < rebuildReasons.Count; ++reasonIndex)
            {
                var reason = rebuildReasons[reasonIndex];

                if (reason is InputChangedReason)
                    continue;

                bool isLastReason = (reasonIndex == rebuildReasons.Count - 1);
                bool shouldWriteNewLineAfterReason = !isLastReason && (rebuildReasons[reasonIndex + 1].GetType() != reason.GetType());

                if (reason is ActionChangedReason actionChanged)
                {
                    Console.Write($"{prefix}");
                    using (new Color(ConsoleColor.Red))
                        Console.WriteLine(" The action for the node changed.");

                    FormatTokenDiffsToConsole(actionChanged.OldAction, actionChanged.NewAction, "action", prefix, 160);
                }
                else if (reason is ResponseFileChangedReason responseFileChanged)
                {
                    Console.Write(prefix);
                    using (new Color(ConsoleColor.Red))
                        Console.WriteLine(" The response file for the node changed.");

                    FormatTokenDiffsToConsole(responseFileChanged.OldContent,
                        responseFileChanged.NewContent, "response file content", prefix, 160);
                }
                else if (reason is DependencyChanged dependencyChanged)
                {
                    Console.Write($"{prefix}- {dependencyChanged.DependencyInfo.Annotation} ");
                    using (new Color(ConsoleColor.Yellow))
                        Console.Write(dependencyChanged.DependencyInfo.RebuildReasons.OfType<NotBuiltBeforeReason>().Any() ? "is an input and was built" : "is an input and was rebuilt");

                    if (!alreadyExplainedNodes.Add(dependencyChanged.DependencyInfo.OriginalDagIndex))
                    {
                        Console.WriteLine($", as explained above.");
                    }
                    else
                    {
                        Console.WriteLine();
                        var newPrefix = (isLastReason ? prefix.Substring(0, prefix.Length - 1) + " " : prefix) + "  |";
                        Console.WriteLine(newPrefix);
                        FormatRebuildReasonsToConsole(newPrefix, dependencyChanged.DependencyInfo.RebuildReasons, alreadyExplainedNodes);

                        if (!isLastReason)
                            shouldWriteNewLineAfterReason = true;
                    }
                }
                else if (reason is InputListChangedReason inputListChanged)
                {
                    Console.WriteLine($"{prefix} The {inputListChanged.DependencyType} inputs to the node changed.");

                    if (inputListChanged.FilesAdded.Any())
                    {
                        Console.WriteLine($"{prefix} Files added:");
                        foreach (var file in inputListChanged.FilesAdded)
                        {
                            Console.Write(prefix);
                            using (new Color(ConsoleColor.Green))
                                Console.WriteLine("   " + file);
                        }
                    }

                    if (inputListChanged.FilesRemoved.Any())
                    {
                        Console.WriteLine($"{prefix} Files removed:");
                        foreach (var file in inputListChanged.FilesRemoved)
                        {
                            Console.Write(prefix);
                            using (new Color(ConsoleColor.Red))
                                Console.WriteLine("   " + file);
                        }
                    }
                }
                else if (reason is NotBuiltBeforeReason)
                {
                    Console.WriteLine($"{prefix} The node hadn't been built before.");
                }
                else if (reason is PreviousBuildFailedReason)
                {
                    Console.WriteLine($"{prefix} The node failed to build successfully on the previous attempt.");
                }
                else if (reason is OutputsMissingReason outputsMissing)
                {
                    Console.WriteLine($"{prefix} Some of the node's outputs were missing.");
                    if (outputsMissing.OutputsMissing.Any())
                    {
                        Console.WriteLine($"{prefix} Files missing:");
                        foreach (var file in outputsMissing.OutputsMissing)
                        {
                            Console.Write(prefix);
                            using (new Color(ConsoleColor.Red))
                                Console.WriteLine("   " + file);
                        }
                    }
                }
                else if (reason is NoGoodReason)
                {
                    Console.WriteLine($"{prefix} Not entirely sure why this got rebuilt!");
                    Console.WriteLine($"{prefix} Probably the structure of the build graph changed?");
                }
                else
                {
                    Console.WriteLine(
                        $"{prefix} Formatting has not been implemented for reasons of type {reason.GetType().Name}");
                }

                if (shouldWriteNewLineAfterReason)
                    Console.WriteLine(prefix);
            }
        }
    }
}
