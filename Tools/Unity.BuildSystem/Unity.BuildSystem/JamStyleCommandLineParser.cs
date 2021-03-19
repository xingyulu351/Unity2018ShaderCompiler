using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JamSharp.Runtime;

namespace Unity.BuildSystem
{
    class JamStyleCommandLineParser
    {
        private List<string> _targetsToBuild = new List<string>();
        public IEnumerable<string> TargetsToBuild => _targetsToBuild;

        private Dictionary<string, string> _variables = new Dictionary<string, string>();
        public IReadOnlyDictionary<string, string> Variables => _variables;

        public JamStyleCommandLineParser(params string[] args) : this((IEnumerable<string>)args)
        {
        }

        public JamStyleCommandLineParser(IEnumerable<string> args)
        {
            // Skip the first arg, which is the buildprogram path
            foreach (var arg in args)
            {
                if (arg.StartsWith("-s"))
                {
                    var parts = arg.Substring(2).Split('=');
                    if (parts.Length < 2)
                        continue;

                    _variables[parts[0]] = String.Join("=", parts.Skip(1));
                    continue;
                }

                if (!arg.StartsWith("-"))
                {
                    if (arg.Contains('='))
                    {
                        // This appears to be a shorthand for -s
                        var parts = arg.Split('=');
                        if (parts.Length < 2)
                            continue;
                        _variables[parts[0]] = String.Join("=", parts.Skip(1));
                        continue;
                    }

                    _targetsToBuild.Add(arg);
                }
            }

            if (!_targetsToBuild.Any())
                _targetsToBuild.Add("all");
        }

        public void VerifyParsedCommandLineAgainstJamCore(GlobalVariables globalVariables)
        {
            var jamTargetsToBuild = globalVariables["JAM_COMMAND_LINE_TARGETS"].Elements;

            var sb = new StringBuilder();

            var targetsMissingFromJamCore = TargetsToBuild.Except(jamTargetsToBuild).ToArray();
            var targetsMissingFromParser = jamTargetsToBuild.Except(TargetsToBuild).ToArray();

            if (targetsMissingFromJamCore.Any())
                sb.AppendLine($"The parser located targets that JamCore did not: {string.Join(",", targetsMissingFromJamCore)}");
            if (targetsMissingFromParser.Any())
                sb.AppendLine($"JamCore located targets that the parser did not: {string.Join(",", targetsMissingFromParser)}");

            var knownGlobals = globalVariables.Storage.AllGlobalVariables;
            foreach (var kvp in Variables)
            {
                if (!knownGlobals.Contains(kvp.Key))
                    sb.AppendLine($"The parser extracted variable that JamCore did not: {kvp.Key}");
                else if (globalVariables[kvp.Key] != kvp.Value)
                    sb.AppendLine($"The parser extracted variable {kvp.Key} with value {kvp.Value}, but JamCore gave it value {globalVariables[kvp.Key]}");
            }

            var errors = sb.ToString();
            if (!string.IsNullOrEmpty(errors))
                throw new Exception("JamStyleCommandLineParser results did not match JamCore! " + errors);
        }
    }
}
