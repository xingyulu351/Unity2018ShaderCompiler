using NDesk.Options;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace usw.Configuration
{
    public class ConversionOptions
    {
        public static ConversionOptions Parse(IEnumerable<string> args)
        {
            args = ParseResponseFiles(args);

            var result = new ConversionOptions();
            var options = new OptionSet
            {
                {"assembly:", a => { result.Assemblies.Add(a); result.additionalAssemblyPaths.Add(Path.GetDirectoryName(a)); } },
                {"verbose", v => result.Verbose = true },
                {"pdb", v => result.OutSymbolsFormat = Weaver.OutSymbolsFormat.Pdb },
                {"mdb", v => result.OutSymbolsFormat = Weaver.OutSymbolsFormat.Mdb },
                {"additionalAssemblyPath:", v => result.additionalAssemblyPaths.Add(v) },
                {"unity-engine:", v => result.UnityEngineDLLPath = v },
                {"unity-networking:", v => result.UnityEngineNetworkingDllPath = v },
                { "lock=", "Path to project.lock.json file.", l => result.ProjectLockFile = l },
            };
            var extra = options.Parse(args);
            if (extra.Count != 1)
                throw new ArgumentException("Usage:\n\tuws -assembly=[assembly] -assembly=[assembly2] [pdb|mdb] --unity-engine=<path to UnityEngine.dll> <output directory>");

            var lastArgumentIndex = extra.Count - 1;
            result.OutputDir = extra[lastArgumentIndex];
            return result;
        }

        private static IEnumerable<string> ParseResponseFiles(IEnumerable<string> args)
        {
            var newArgs = new List<string>();

            foreach (var arg in args)
            {
                if (arg.Length > 0 && arg[0] == '@')
                {
                    var path = arg.Substring(1);

                    try
                    {
                        var responseArguments = File.ReadAllLines(path, Encoding.UTF8);

                        foreach (var line in responseArguments)
                        {
                            if (!string.IsNullOrEmpty(line))
                                newArgs.Add(line);
                        }
                    }
                    catch (Exception e)
                    {
                        throw new Exception("Failed to read response file \"" + path + "\": " + e.ToString());
                    }
                }
                else
                {
                    newArgs.Add(arg);
                }
            }

            return newArgs;
        }

        public bool Verbose;
        public string OutputDir;
        public List<string> Assemblies = new List<string>();
        public string UnityEngineDLLPath;
        public string UnityEngineNetworkingDllPath;
        public string ProjectLockFile;
        public Weaver.OutSymbolsFormat OutSymbolsFormat = Weaver.OutSymbolsFormat.None;
        public HashSet<string> additionalAssemblyPaths = new HashSet<string>();
    }
}
