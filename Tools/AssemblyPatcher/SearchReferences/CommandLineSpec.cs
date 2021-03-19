using CommandLine;
using CommandLine.Text;

namespace SearchReferences
{
    class CommandLineSpec
    {
        [Option('c', "check", HelpText = "Checks if all referenced types/member exist", MutuallyExclusiveSet = "cmd")]
        public bool CheckReferences { get; set; }

        [Option('s', "search", HelpText = "Searchs strings in types/members.", MutuallyExclusiveSet = "cmd")]
        public string Search { get; set; }

        [Option('a', "assembly", HelpText = "Assembly to be checked.", Required = true)]
        public string Assembly { get; set; }

        [Option('f', "framework-folder", HelpText = "Folder storing framework assemblies.")]
        public string TargetFrameworkFolder { get; set; }

        [Option('r', "aditional-assemblies", HelpText = "Folder storing aditional assemblies to be used for type resolution.")]
        public string AditionalAssembliesFolder { get; set; }

        [Option('d', "assembly-dependency", HelpText = "", MutuallyExclusiveSet = "cmd")]
        public bool AssemblyDependency { get; set; }

        [Option('v', "verbose", DefaultValue = 0, HelpText = "Print extra information.")]
        public int Verbose { get; set; }

        [HelpOption]
        public string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        [ParserState]
        public IParserState LastParserState { get; set; }
    }
}
