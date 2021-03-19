using System.Collections.Generic;
using System.Xml.Linq;

namespace APIDocumentationGenerator
{
    public struct SearchResultElement
    {
        public string HTMLSummary;
        public string Url;
        public string Title;
    }

    public class SearchIndexRepresentation
    {
        public readonly List<SearchResultElement> m_PotentialSearchResults;
        public readonly Dictionary<string, List<int>> m_NameToPageIndices;
        public SearchIndexRepresentation()
        {
            m_PotentialSearchResults = new List<SearchResultElement>();
            m_NameToPageIndices = new Dictionary<string, List<int>>();
        }

        public void AppendSearchResult(SearchResultElement el)
        {
            m_PotentialSearchResults.Add(el);
        }

        public void AddPageIndexToTerm(string term, int index)
        {
            if (!m_NameToPageIndices.ContainsKey(term))
                m_NameToPageIndices[term] = new List<int>(index);
            if (!m_NameToPageIndices[term].Contains(index))
                m_NameToPageIndices[term].Add(index);
        }
    }
}
