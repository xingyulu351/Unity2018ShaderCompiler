using System.Collections.Generic;
using System.Linq;
using UnderlyingModel;

namespace ScriptRefBase
{
    internal class RecognizedClassSubstitutor
    {
        private readonly IMemberGetter m_Getter;
        private readonly bool m_ResolveLinkWithoutDoc;

        public RecognizedClassSubstitutor(IMemberGetter getter, bool resolveLinkWithoutDoc)
        {
            m_Getter = getter;
            m_ResolveLinkWithoutDoc = resolveLinkWithoutDoc;
        }

        public LinkResolvedPattern Resolve(string input)
        {
            var splitString = SplitWithPunctuation(input);
            for (int wordIndex = 0; wordIndex < splitString.Length; wordIndex++)
            {
                var wordEntry = splitString[wordIndex];
                var elem = m_Getter.GetClosestNonObsoleteMember(wordEntry);
                if (elem == null || elem.IsUndoc)
                    continue;
                if (!m_ResolveLinkWithoutDoc && !elem.AnyHaveDoc)
                    continue;

                var beforeRange = splitString.ToList().GetRange(0, wordIndex);
                var afterRange = splitString.ToList().GetRange(wordIndex + 1, splitString.Length - wordIndex - 1);
                var before = string.Join("", beforeRange);
                var after = string.Join("", afterRange);
                return new LinkResolvedPattern
                {
                    m_Before = before,
                    m_ResolvedPattern = new MemDocElementLink(elem.HtmlName, wordEntry),
                    m_After = after
                };
            }
            return null;
        }

        static bool IsWordChar(char c)
        {
            return char.IsLetterOrDigit(c) || c == '_';
        }

        static bool IsDotFollowedByWordChar(int i, string input)
        {
            char c = input[i];
            return c == '.' && i < input.Length - 1 && IsWordChar(input[i + 1]);
        }

        internal static string[] SplitWithPunctuation(string input)
        {
            var stringList = new List<string>();
            int current = 0;
            while (current < input.Length)
            {
                int lookahead = current + 1;
                if (IsWordChar(input[current]) || IsDotFollowedByWordChar(current, input))
                {
                    while (lookahead < input.Length && (IsWordChar(input[lookahead]) || IsDotFollowedByWordChar(lookahead, input)))
                        lookahead++;
                }
                else
                {
                    while (lookahead < input.Length && !IsWordChar(input[lookahead]))
                        lookahead++;
                }
                stringList.Add(input.Substring(current, lookahead - current));
                current = lookahead;
            }
            return stringList.ToArray();
        }
    }
}
