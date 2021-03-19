using System;
using System.IO;
using Bee.Core;
using Bee.TestHelpers;
using NUnit.Framework;

namespace Bee.StandaloneDriver.Tests
{
    public class StandaloneBeeDriverTests
    {
        private TopLevelBeeCommand m_DefaultSubcommandBackup;
        private TopLevelBeeCommand[] m_SubcommandsBackup;

        [SetUp]
        public void SetUp()
        {
            // Clear these out for the duration of these argparsing tests to
            // avoid accidentally running real commands.
            m_DefaultSubcommandBackup = StandaloneBeeDriver.DefaultSubcommand;
            m_SubcommandsBackup = StandaloneBeeDriver.Subcommands;
            StandaloneBeeDriver.DefaultSubcommand = null;
            StandaloneBeeDriver.Subcommands = new TopLevelBeeCommand[0];
        }

        [TearDown]
        public void TearDown()
        {
            StandaloneBeeDriver.DefaultSubcommand = m_DefaultSubcommandBackup;
            StandaloneBeeDriver.Subcommands = m_SubcommandsBackup;
        }

        class MySubCommand : TopLevelBeeCommand
        {
            private Action<string[]> Handler { get; }
            public MySubCommand(string name, Action<string[]> handler = null, string abbreviation = null, string description = null)
            {
                Name = name;
                Handler = handler;
                Abbreviation = abbreviation;
                Description = description;
            }

            public override string Name { get; }
            public override string Abbreviation { get; }
            public override string Description { get; }
            public override void Execute(string[] args)
            {
                Handler(args);
            }
        }


        [Test]
        public void Main_WithSubcommand_RunsRequestedSubcommand()
        {
            string[] actualSubcommandArgs = null;
            StandaloneBeeDriver.Subcommands = new[]
            {
                new MySubCommand("testcmd", args => actualSubcommandArgs = args),
            };

            StandaloneBeeDriver.Main(new[] { "testcmd", "do", "--stuff" });

            Assert.AreEqual(new[] { "do", "--stuff" }, actualSubcommandArgs);
        }

        [Test]
        public void Main_WithNoArgs_RunsDefaultSubcommand()
        {
            string[] actualSubcommandArgs = null;
            StandaloneBeeDriver.Subcommands = new[]
            {
                new MySubCommand("testcmd", args => throw new AssertionException("should not run")),
            };
            StandaloneBeeDriver.DefaultSubcommand = new MySubCommand("thedefault", args => actualSubcommandArgs = args);

            StandaloneBeeDriver.Main(new string[0]);

            Assert.AreEqual(new string[0], actualSubcommandArgs);
        }

        [Test]
        public void Main_WithUnknownArgs_RunsDefaultSubcommand()
        {
            string[] actualSubcommandArgs = null;
            StandaloneBeeDriver.Subcommands = new[]
            {
                new MySubCommand("testcmd", args => throw new AssertionException("should not run")),
            };
            StandaloneBeeDriver.DefaultSubcommand = new MySubCommand("thedefault", args => actualSubcommandArgs = args);

            StandaloneBeeDriver.Main(new[] { "nosuchcommand", "--stuff" });

            Assert.AreEqual(new[] { "nosuchcommand", "--stuff" }, actualSubcommandArgs);
        }

        [TestCase("-h")]
        [TestCase("--help")]
        public void Main_WithHelpOption_ShowsHelp(string option)
        {
            StandaloneBeeDriver.Subcommands = new[]
            {
                new MySubCommand("sample", abbreviation: "ex", description: "my example command"),
            };

            StandaloneBeeDriver.DefaultSubcommand = StandaloneBeeDriver.Subcommands[0];
            var output = new StringWriter();

            using (new TempConsoleRedirect(output))
            {
                StandaloneBeeDriver.Main(new[] { option });
            }

            StringAssert.IsMatch(@"^Bee buildsystem\. Powered by Tundra by Andreas Fredriksson\.  Built from: .*

list of commands:

 ex \| sample  my example command

'sample' is the default command\. Use 'bee COMMAND --help' for help on specific commands\.
$".Replace("\n", "\\r?\n"), output.ToString());
        }
    }
}
