using System;
using System.Text.RegularExpressions;

namespace ScriptRefBase
{
    internal class LinkResolverPatternSubstitutor
    {
        private readonly Regex m_Regex;
        private readonly Func<string, IMemDocElement> m_LinkSubstitutor;
        private readonly string m_Name; //used for debugging and readability

        public LinkResolverPatternSubstitutor(string name, Regex regex, Func<string, IMemDocElement> linkSubstitutor)
        {
            m_Name = name;
            m_LinkSubstitutor = linkSubstitutor;
            m_Regex = regex;
        }

        public LinkResolvedPattern Resolve(string input)
        {
            var firstMatch = m_Regex.Match(input);
            if (!firstMatch.Success)
                return null;
            var resolved = m_LinkSubstitutor(firstMatch.ToString());
            string[] splitPattern;
            if (m_Name == "VARNAME")
            {
                var splitRegex = new Regex(@"\/(\w+(?:.\w+)?)\/");
                splitPattern = splitRegex.Split(input, 2);
                return new LinkResolvedPattern
                {
                    m_Before = splitPattern[0],
                    m_ResolvedPattern = resolved,
                    m_After = splitPattern[2]
                };
            }
            splitPattern = m_Regex.Split(input, 2);
            return new LinkResolvedPattern
            {
                m_Before = splitPattern[0],
                m_ResolvedPattern = resolved,
                m_After = splitPattern[1]
            };
        }

        public override string ToString()
        {
            return m_Name;
        }
    }
}
