using System;
using System.Xml;
using System.Xml.Linq;
using UnderlyingModel;
using System.Xml.Xsl;
using APIDocumentationGenerator;
using XMLFormatter;

namespace APIDocumentationGenerator
{
    internal class SearchPopulator
    {
        private readonly XslCompiledTransform m_XsltTransform;

        internal SearchIndexRepresentation SearchIndex { get; private set; }

        internal SearchPopulator()
        {
            m_XsltTransform = XsltUtils.InitXslt(XsltUtils.XslType.SearchResults);
            SearchIndex = new SearchIndexRepresentation();
        }

        internal void AppendPotentialSearchResult(MemberItem memberItem, Func<string, string, string, XElement> getXMLOutputFromDoc)
        {
            var simplified = memberItem.DocModel.Summary.Replace("\n", @"\\");
            simplified = simplified.Replace("\r", "");
            simplified = simplified.Replace("\"", "");
            simplified = simplified.Replace("\\", "");

            var xmlSummary = getXMLOutputFromDoc(memberItem.GetNames().ClassName, simplified, "summary");
            var htmlSummary = XmlUtils4.XmlToHtml(xmlSummary, m_XsltTransform);
            var oneResult = new SearchResultElement
            {
                HTMLSummary = htmlSummary,
                Title = memberItem.DisplayName,
                Url = memberItem.HtmlName
            };
            SearchIndex.AppendSearchResult(oneResult);
        }

        internal void SearchIndexFromXmlContent(XElement xmlContent, int index)
        {
            var xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(xmlContent.ToString());

            var search = new SearchWordsXmlDocument(xmlDoc);
            var searchWords = search.GetSearchWordsList();
            foreach (var word in searchWords)
                SearchIndex.AddPageIndexToTerm(word, index);
        }
    }
}
