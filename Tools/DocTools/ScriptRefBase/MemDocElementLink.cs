namespace ScriptRefBase
{
    public class MemDocElementLink : IMemDocElement
    {
        public string Content { get; private set; }
        public LinkType LinkType { get; private set; }
        public string Link { get; private set; }

        public MemDocElementLink()
        {
        }

        public MemDocElementLink(string link, string content,
                                 LinkType lt = LinkType.ScriptRef)
        {
            Content = content;
            LinkType = lt;
            Link = link;
        }

        public void InitWikiLink(string originalText)
        {
            var textAfterWiki = originalText.Replace("wiki:", "");

            LinkType = LinkType.Wiki;
            string remainderText = textAfterWiki;
            if (remainderText.StartsWith("man:"))
            {
                remainderText = remainderText.Replace("man:", "");
            }
            else if (remainderText.StartsWith("comp:"))
            {
                remainderText = remainderText.Replace("comp:", "");
            }

            if (remainderText.Contains("|"))
            {
                var split = remainderText.Split('|');
                Link = split[0].Replace(" ", "");
                Content = split[1];
            }
            else
            {
                Link = remainderText.Replace(" ", "");
                Content = remainderText;
            }
        }

        public override string ToString()
        {
            var linkType = "";
            if (LinkType == LinkType.Wiki)
                linkType = "(Wiki)";
            else if (LinkType == LinkType.External)
                linkType = "(Ext)";
            return string.Format("{0}:{1}{2}", Link, Content, linkType);
        }

        public string PlainText
        {
            get { return Content; }
        }
    }
}
