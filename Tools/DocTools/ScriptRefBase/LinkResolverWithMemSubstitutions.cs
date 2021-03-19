using UnderlyingModel;

namespace ScriptRefBase
{
    public class LinkResolverWithMemSubstitutions : LinkResolver
    {
        public IMemberGetter MemberGetter { get; private set; }
        public bool ResolveLinkWithoutDoc { private get; set; }

        public LinkResolverWithMemSubstitutions(IMemberGetter getter)
        {
            MemberGetter = getter;
            ResolveLinkWithoutDoc = false; //don't create links to pages without docs (except for testing)
        }

        public MemDocElementList ParseDescriptionWithMemSubstitutions(string input)
        {
            var list = ParseDescription(input);
            var newList = new MemDocElementList();
            foreach (var elem in list.m_Elements)
            {
                var textElem = elem as MemDocElementPlainText;
                if (textElem != null)
                {
                    var resolve = SubstituteMemberItems(textElem.Text);
                    if (resolve != null)
                    {
                        newList.m_Elements.AddRange(resolve.m_Elements);
                        continue;
                    }
                }
                newList.m_Elements.Add(elem);
            }
            return newList;
        }

        private MemDocElementList SubstituteMemberItems(string input)
        {
            var memSubstitutor = new RecognizedClassSubstitutor(MemberGetter, ResolveLinkWithoutDoc);
            var resolved = memSubstitutor.Resolve(input);
            if (resolved != null)
            {
                var ret = new MemDocElementList();

                var descriptionAfter = SubstituteMemberItems(resolved.m_After);

                //note that since we're not using regex here and the parsing of patterns is strictly forward,
                //the "before" part doesn't need to be parsed
                ret.AddElements(MakeSimpleList(resolved.m_Before), resolved.m_ResolvedPattern, descriptionAfter);
                return ret;
            }
            return MakeSimpleList(input);
        }

        protected override IMemDocElement ResolveClass(string unresolvedLink)
        {
            return ResolveClassLink(unresolvedLink);
        }

        protected override IMemDocElement ResolveClassWithMember(string unresolvedLink)
        {
            return ResolveClassLink(unresolvedLink);
        }

        private IMemDocElement ResolveClassLink(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart('[').TrimEnd(']');
            return GetElementLinkFromExtractedName(extractedName);
        }

        protected override IMemDocElement ResolveLinkWithText(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart('[').TrimEnd(']');
            var splitString = extractedName.Split('|');
            var linkText = splitString[0];
            var contentText = splitString[1];
            var item = MemberGetter.GetClosestNonObsoleteMember(linkText);
            if (item != null)
            {
                var resolvedLinkText = item.HtmlName;
                return new MemDocElementLink(resolvedLinkText, contentText);
            }
            return new MemDocElementLink(linkText, contentText);
        }

        protected override IMemDocElement ResolverRefCurrentClass(string unresolvedLink)
        {
            var extractedName = unresolvedLink.TrimStart().Replace("::ref::", "");
            var curClassItem = MemberGetter.GetClosestNonObsoleteMember(CurClass);
            if (curClassItem != null)
            {
                var linkText = GetLinkText(curClassItem.HtmlName, extractedName);
                return new MemDocElementLink(linkText, extractedName);
            }
            return new MemDocElementPlainText(extractedName);
        }

        protected override IMemDocElement ResolveThreeDots(string arg)
        {
            var extractedName = arg.TrimStart('[').TrimEnd(']');
            return GetElementLinkFromExtractedName(extractedName);
        }

        private IMemDocElement GetElementLinkFromExtractedName(string extractedName)
        {
            var item = MemberGetter.GetClosestNonObsoleteMember(extractedName);
            if (item != null)
            {
                var linkText = item.HtmlName;
                var contentText = item.DisplayName;
                return new MemDocElementLink(linkText, contentText);
            }
            return new MemDocElementPlainText(extractedName);
        }
    }
}
