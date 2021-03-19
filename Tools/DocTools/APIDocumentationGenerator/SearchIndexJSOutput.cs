using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Xsl;
using APIDocumentationGenerator;
using XMLFormatter;

namespace MemSyntax
{
    //creates a JS index based on the SearchIndexRepresentation
    public class SearchIndexJSOutput
    {
        private readonly SearchIndexRepresentation m_Repr;

        public SearchIndexJSOutput(SearchIndexRepresentation repr)
        {
            m_Repr = repr;
        }

        public void WriteSearchIndexTo(string jsFileName)
        {
            var writer = new StreamWriter(jsFileName);
            writer.WriteLine("//\n// Search index for Unity script documentation\n//");

            writer.Write("var pages = \n[");

            int j = 0;
            foreach (var searchResult in m_Repr.m_PotentialSearchResults)
            {
                var formatSearchResult = FormatPages(searchResult);
                writer.Write(formatSearchResult);
                if (j < m_Repr.m_PotentialSearchResults.Count - 1)
                    writer.Write(",");
                j++;
            }
            writer.WriteLine("];");

            writer.Write("var info = \n[");

            int i = 0;
            foreach (var searchResult in m_Repr.m_PotentialSearchResults)
            {
                var formatSearchResult = FormatSearchResult(searchResult, i);
                writer.Write(formatSearchResult);
                if (i < m_Repr.m_PotentialSearchResults.Count - 1)
                    writer.Write(",");
                i++;
            }
            writer.WriteLine("];");
            writer.Write("var common = \n{");
            var commonWordsJSOutput = string.Join(",", SearchWordsConstants.m_CommonWordsList.Select(CommonWordColumnOne));
            writer.Write(commonWordsJSOutput);

            writer.WriteLine("};");
            writer.Write("var searchIndex = \n{");
            var mapEntries = m_Repr.m_NameToPageIndices.OrderBy(m => m.Key).Select(FormatSearchIndexEntry);
            string pageIndicesAsString = string.Join(",", mapEntries);

            writer.Write(pageIndicesAsString);
            writer.WriteLine("};");
            writer.Close();
        }

        private string FormatSearchIndexEntry(KeyValuePair<string, List<int>> keyValuePair)
        {
            string intListAsString = string.Join(",", keyValuePair.Value);
            return string.Format("\"{0}\":[{1}]", keyValuePair.Key, intListAsString);
        }

        private string FormatPages(SearchResultElement result)
        {
            return string.Format("[\"{0}\", \"{1}\"]", result.Url, result.Title);
        }

        private string FormatSearchResult(SearchResultElement result, int i)
        {
            var htmlSummary = result.HTMLSummary;
            return string.Format("[\"{0}\",{1}]", htmlSummary, i);
        }

        private string CommonWordColumnOne(string common)
        {
            return string.Format("\"{0}\":1", common);
        }
    }
}
