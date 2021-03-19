using CommandLine;
using CommandLine.Text;

namespace AssemblyPatcher
{
    internal class CommandLineSpec
    {
        [Option('a', "assembly", HelpText = "Assembly to be patched", Required = true)]
        public string Assembly { get; set; }

        [Option('o', "output", HelpText = "Output assembly path (defaults to assembly.patched.dll) (if relative, will be rooted at the same folder as the original).")]
        public string Output { get; set; }

        [Option('c', "config", Required = true, HelpText = "Config file with fully qualified name of members/types to be patched.")]
        public string ConfigFile { get; set; }

        [Option('k', "key-file", HelpText = "Path to the file containing the key to resign a strong named asssembly.", DefaultValue = null)]
        public string KeyFilePath { get; set; }

        [Option('s', "search-path", HelpText = "Replacement assemblies search path.")]
        public string SearchPath { get; set; }

        [Option('f', "log-file", HelpText = "Log output file path (adctive to --log-config).")]
        public string LogFile { get; set; }

        [Option('l', "log-level", HelpText = "Log level (if not specified the one in the configuration file is used).")]
        public string LogLevel { get; set; }

        [Option("log-config", HelpText = "Log (log4net) configuration file path.")]
        public string LogConfig { get; set; }

        [Option('p', "platform", HelpText = "Target platform (if not specified, General is assumed).", DefaultValue = "General", Required = false)]
        public string Platform { get; set; }

        [HelpOption]
        public string GetUsage()
        {
            return HelpText.AutoBuild(this, current => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        [ParserState]
        public IParserState LastParserState { get; set; }
    }
}
