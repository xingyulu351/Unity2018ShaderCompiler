using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using AssemblyPatcher.Extensions;

namespace AssemblyPatcher.Configuration
{
    class Parser
    {
        internal IEnumerable<PatchRule> Parse(string configFilePath)
        {
            using (var content = new FileStream(configFilePath, FileMode.Open, FileAccess.Read))
            {
                return Parse(content);
            }
        }

        internal IEnumerable<PatchRule> Parse(Stream configContents)
        {
            using (var reader = new StreamReader(configContents))
            {
                var rules = new List<PatchRule>();
                rules.AddRange(reader.Lines().Select(ParsePatchRule).Where(rule => rule != null));

                return rules;
            }
        }

        private PatchRule ParsePatchRule(string ruleAsText, int line)
        {
            if (string.IsNullOrWhiteSpace(ruleAsText)) return null;

            var match = ConfigLineMatcher.Match(ruleAsText);
            if (!match.Success)
            {
                throw new PatchRuleException(ruleAsText, line + 1);
            }

            if (match.Groups["full_line_comment"].Success)
                return null;

            var patchRule = new PatchRule(ValueOfNull(match, "source"), ValueOfNull(match, "target"));
            if (match.Groups["target_assembly"].Success)
            {
                patchRule.DeclaringAssembly = AppendDllIfNoExtension(match.Groups["target_assembly"].Value.Trim());
            }

            if (match.Groups["applicable_with"].Success)
            {
                patchRule.ApplicableWith = match.Groups["applicable_with"].Value.Trim();
            }

            if (patchRule.Operation == OperationKind.RemoveBody)
                return patchRule;

            patchRule.Operation = (match.Groups["target"].Success || match.Groups["target_assembly"].Success)
                ? OperationKind.Replace
                : OperationKind.Throw;
            return patchRule;
        }

        private static string AppendDllIfNoExtension(string declaringAssembly)
        {
            var an = new AssemblyName(declaringAssembly);
            if (an.Version != null) return declaringAssembly; // Assume 'declaringAssembly' is an AssemblyName instead of the file location of the assembly

            return declaringAssembly.EndsWith(".dll") || declaringAssembly.EndsWith(".exe")
                ? declaringAssembly
                : declaringAssembly + ".dll";
        }

        private static string ValueOfNull(Match match, string groupname)
        {
            return match.Groups[groupname].Success ?  match.Groups[groupname].Value.Trim() : null;
        }

        private static Regex ConfigLineMatcher = new Regex(@"
                                (
                                    (
                                        (\|\s*(?<applicable_with>[^\|]{1,})\s*\|)?
                                        (\[\s* (?<source_assembly>[^\]]*)\s*\])? \s* ( (?<source>.{1,})? )\s* -> \s*

                                        (\[\s*(?<target_assembly>[^\]]*)\s*\])?\s*( (?<target>[^\#]{1,})?)?

                                        \s*(?<comment>\#.{1,})?
                                    ) |

                                    \s*(?<full_line_comment>\#.{1,})
                                )", RegexOptions.Compiled | RegexOptions.ExplicitCapture | RegexOptions.IgnorePatternWhitespace);
    }
}
