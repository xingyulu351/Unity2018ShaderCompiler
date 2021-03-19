using System;

namespace AssemblyPatcher
{
    public class PatchRuleException : Exception
    {
        public PatchRuleException(string ruleAsText, int line)
        {
            LineNumber = line;
            RuleText = ruleAsText;
        }

        protected string RuleText { get; private set; }
        public int LineNumber { get; private set; }
    }
}
