using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace ScriptRefBase
{
    public class LinkResolver
    {
        private readonly List<LinkResolverPatternSubstitutor> m_PatternList;
        public string CurClass { get; set; }

        public LinkResolver()
        {
            //note: the order of the patterns matters:
            //e.g. REF_WITHCLASS must be matched before REF_NOCLASS
            //BOLDITALIC must be matched before BOLD or ITALICS
            try
            {
                m_PatternList = new List<LinkResolverPatternSubstitutor>
                {
                    //web link <a href>
                    new LinkResolverPatternSubstitutor("WEBLINK", LinkRegexHelper.FormRegexEscapedClosingPattern(@"\<a", @"\<\/a\>"), ResolveHTML),

                    //Wiki link with text
                    new LinkResolverPatternSubstitutor("WIKILINKWITHTEXT", LinkRegexHelper.FormSingleRegexWithSeparator(@"\[\[wiki:", @"\]\]", @"\|"), ResolveWikiLinkWithText),

                    //Wiki link
                    new LinkResolverPatternSubstitutor("WIKILINK", LinkRegexHelper.FormSingleRegex(@"\[\[wiki:", @"\]\]"), ResolveWikiLink),

                    //[[link|text]]
                    new LinkResolverPatternSubstitutor("LINKWITHTEXT", LinkRegexHelper.FormDoubleRegexWithSeparator(@"\[", @"\]", @"\|"), ResolveLinkWithText),

                    //This could be either a single-word namespace with a class, or a single-word namespace + class + function
                    new LinkResolverPatternSubstitutor("THREEDOTS", new Regex(@"\[\[\w+\.\w+\.\w+\]\]"), ResolveThreeDots),

                    //anything between double square brackets that is not a square bracket and not a dot
                    new LinkResolverPatternSubstitutor("CLASS", new Regex(@"\[\[\w+\]\]"), ResolveClass),

                    //anything between double square brackets that is not a square bracket
                    new LinkResolverPatternSubstitutor("CLASS_WITHMEMBER", LinkRegexHelper.FormDoubleRegexWithSeparator(@"\[", @"\]", @"\."),
                        ResolveClassWithMember),

                    new LinkResolverPatternSubstitutor("LINEBREAK1", new Regex(@"\\\\"), ResolveLineBreak),
                    new LinkResolverPatternSubstitutor("LINEBREAK2", new Regex(@"\<br\>"), ResolveLineBreak),
                    new LinkResolverPatternSubstitutor("PARAGRAPHBREAK_WIN", new Regex(@"\r\n\r\n"), ResolveParagraphBreak),
                    new LinkResolverPatternSubstitutor("PARAGRAPHBREAK", new Regex(@"\n\n"), ResolveParagraphBreak),
                    new LinkResolverPatternSubstitutor("PREF_WITHCLASS", new Regex(@"\w+::pref::\w+"),
                        unresolvedLink => ResolverRefExplicitClass(unresolvedLink, true)),
                    new LinkResolverPatternSubstitutor("REF_WITHCLASS", new Regex(@"\w+::ref::\w+"),
                        unresolvedLink => ResolverRefExplicitClass(unresolvedLink, false)),
                    new LinkResolverPatternSubstitutor("REF_NOCLASS", new Regex(@"::ref::\w+"), ResolverRefCurrentClass),
                    new LinkResolverPatternSubstitutor("IMG", new Regex(@"\{img.+\}"), ResolverImage),

                    //recognize /x/ as well as /x.y/
                    new LinkResolverPatternSubstitutor("VARNAME", new Regex(@"\/(\w+(.\w+)?[^/]?)\/[\s,.?!]"), ResolverStyleVarName),

                    new LinkResolverPatternSubstitutor("BOLDITALIC", LinkRegexHelper.FormRegexEscapedClosingPattern(@"__\'\'", @"\'\'__"), ResolverStyleBoldItalic),
                    new LinkResolverPatternSubstitutor("BOLD", LinkRegexHelper.FormRegexDouble("_", "_"), ResolverBold),
                    new LinkResolverPatternSubstitutor("ITALICS", LinkRegexHelper.FormRegexDouble("\'", "\'"), ResolverStyleItalics),
                    new LinkResolverPatternSubstitutor("MONOTYPE", LinkRegexHelper.FormRegexDouble("@", "@"), ResolverStyleMonoType),
                    new LinkResolverPatternSubstitutor("TRUETYPE", LinkRegexHelper.FormRegexEscapedClosingPattern(@"\<tt\>", @"<\/tt\>"), ResolverStyleTrueType)
                };
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error creating regex = {0}", ex);
            }
        }

        public MemDocElementList ParseDescription(string input)
        {
            foreach (var pattern in m_PatternList)
            {
                var resolved = pattern.Resolve(input);
                if (resolved == null)
                    continue;
                var ret = new MemDocElementList();
                var descriptionBefore = ParseDescription(resolved.m_Before);
                var descriptionAfter = ParseDescription(resolved.m_After);
                ret.AddElements(descriptionBefore, resolved.m_ResolvedPattern, descriptionAfter);

                return ret;
            }

            //no special patterns are matched, return a list with one plain text element
            var subsitutedInput = RunSubstitutions(input);

            return MakeSimpleList(subsitutedInput);
        }

        public static MemDocElementList MakeSimpleList(string input)
        {
            return new MemDocElementList(new MemDocElementPlainText(input));
        }

        private static string RunSubstitutions(string input)
        {
            string substituted = input;
            substituted = substituted.Replace("SA:", "See Also:");
            substituted = substituted.Replace("(RO)", "(Read Only)");
            substituted = substituted.Replace(@"\\", "\n");
            substituted = substituted.Replace(@"<br>", "\n");
            return substituted;
        }

        protected virtual IMemDocElement ResolveClass(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart('[').TrimEnd(']');
            return new MemDocElementLink(extractedName, extractedName);
        }

        private IMemDocElement ResolveWikiLink(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart('[').TrimEnd(']');

            var wikiStuff = new MemDocElementLink();
            wikiStuff.InitWikiLink(extractedName);
            return wikiStuff;
        }

        private IMemDocElement ResolveWikiLinkWithText(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart('[').TrimEnd(']');

            var wikiStuff = new MemDocElementLink();
            wikiStuff.InitWikiLink(extractedName);
            return wikiStuff;
        }

        protected virtual IMemDocElement ResolveLinkWithText(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart('[').TrimEnd(']');
            var splitString = extractedName.Split('|');
            return new MemDocElementLink(splitString[0], splitString[1]);
        }

        protected virtual IMemDocElement ResolveClassWithMember(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart('[').TrimEnd(']');
            var splitString = extractedName.Split('.');
            var className = splitString[0];
            var memberName = splitString[1];
            var linkText = GetLinkText(className, memberName);
            var contentText = string.Format("{0}.{1}", className, memberName);
            return new MemDocElementLink(linkText, contentText);
        }

        protected virtual IMemDocElement ResolveThreeDots(string arg)
        {
            var extractedName = arg.TrimStart('[').TrimEnd(']');
            return new MemDocElementLink(extractedName, extractedName);
        }

        protected static string GetLinkText(string className, string nameWithoutClass)
        {
            bool startsWithLower = Char.IsLower(nameWithoutClass[0]);
            char delimiter = startsWithLower ? '-' : '.';
            return string.Format("{0}{1}{2}", className, delimiter, nameWithoutClass);
        }

        protected virtual IMemDocElement ResolverRefCurrentClass(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart().Replace("::ref::", "");
            var linkText = GetLinkText(CurClass, extractedName);

            return new MemDocElementLink(linkText, extractedName);
        }

        private static IMemDocElement ResolverRefExplicitClass(string unresolvedLink, bool isPref)
        {
            string refToken = isPref ? "::pref::" : "::ref::";

            int beforeRef = unresolvedLink.IndexOf(refToken);
            int afterRef = beforeRef + refToken.Length;
            var daClass = unresolvedLink.Substring(0, beforeRef);
            var daName = unresolvedLink.Substring(afterRef);
            var displayName = isPref ? daName : daClass + "." + daName;
            var linkText = GetLinkText(daClass, daName);
            return new MemDocElementLink(linkText, displayName);
        }

        private static IMemDocElement ResolverImage(string unresolvedLink)
        {
            var imgName = unresolvedLink.TrimStart().Replace("{img ", "").Replace("}", "");

            return new MemDocElementImage(imgName);
        }

        private static IMemDocElement ResolverStyleTrueType(string arg)
        {
            var name = arg.Replace("<tt>", ""); //eliminate leading <tt>
            name = name.Replace("</tt>", ""); //eliminate trailing </tt>
            return new MemDocElementStyle(name, StyleType.TrueType);
        }

        private static IMemDocElement ResolverStyleVarName(string arg)
        {
            var simplifiedArg = arg.Trim(' ', '\t', ',', '.', '?');
            var firstTry = (MemDocElementStyle)LinkRegexHelper.ResolverDoubleCharStyle(simplifiedArg, '/', StyleType.Var);

            var ret = new MemDocElementStyle(firstTry.Name.Trim(), StyleType.Var);
            return ret;
        }

        private static IMemDocElement ResolverBold(string arg)
        {
            return LinkRegexHelper.ResolverDoubleCharStyle(arg, '_', StyleType.Bold);
        }

        private static IMemDocElement ResolverStyleItalics(string arg)
        {
            return LinkRegexHelper.ResolverDoubleCharStyle(arg, '\'', StyleType.Italic);
        }

        private static IMemDocElement ResolverStyleMonoType(string arg)
        {
            return LinkRegexHelper.ResolverDoubleCharStyle(arg, '@', StyleType.MonoType);
        }

        private static IMemDocElement ResolverStyleBoldItalic(string arg)
        {
            var name = arg.TrimStart('_').TrimEnd('_');
            name = name.TrimStart('\'').TrimEnd('\'');
            return new MemDocElementStyle(name, StyleType.BoldItalic);
        }

        private static IMemDocElement ResolveHTML(string arg)
        {
            var splitShit = arg.Split(new[] {'>'}, 2, StringSplitOptions.RemoveEmptyEntries);
            var link = splitShit[0].Replace("<a href=\"", "").Replace("\"", "");
            var content = splitShit[1].Replace("</a>", "");
            return new MemDocElementLink(link, content, LinkType.External);
        }

        private static IMemDocElement ResolveLineBreak(string arg)
        {
            return new MemDocElementLineBreak();
        }

        private static IMemDocElement ResolveParagraphBreak(string arg)
        {
            var lb = new MemDocElementLineBreak();
            return new MemDocElementList(lb, lb);
        }
    }
}
