using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Xsl;
using APIDocumentationGenerator;
using XMLFormatter;

namespace MemSyntax
{
    //creates a JS index based on the SearchIndexRepresentation
    public class SearchIndexJsonOutput
    {
        private readonly SearchIndexRepresentation m_Repr;

        public SearchIndexJsonOutput(SearchIndexRepresentation repr)
        {
            m_Repr = repr;
        }

        public void WriteSearchIndexTo(string jsFileName)
        {
            var writer = new StreamWriter(jsFileName);
            writer.WriteLine("{");

            writer.Write("\"pages\": \n[");

            int j = 0;
            foreach (var searchResult in m_Repr.m_PotentialSearchResults)
            {
                var formatSearchResult = FormatPages(searchResult);
                writer.Write(formatSearchResult);
                if (j < m_Repr.m_PotentialSearchResults.Count - 1)
                    writer.Write(",\n");
                j++;
            }
            writer.WriteLine("],");

            writer.Write("\"info\": \n[");

            int i = 0;
            foreach (var searchResult in m_Repr.m_PotentialSearchResults)
            {
                var formatSearchResult = FormatSearchResult(searchResult, i);
                formatSearchResult = EscapeStringForJSON(ReplaceNonEscapeBackSlashes(formatSearchResult));

                writer.Write(formatSearchResult);
                if (i < m_Repr.m_PotentialSearchResults.Count - 1)
                    writer.Write(",\n");
                i++;
            }
            writer.WriteLine("],");
            writer.Write("\"common\": {");
            var commonWordsJSOutput = string.Join(",", SearchWordsConstants.m_CommonWordsList.Select(CommonWordColumnOne));
            writer.Write(commonWordsJSOutput);

            writer.WriteLine("},");
            writer.Write("\"searchIndex\": \n{");
            var mapEntries = m_Repr.m_NameToPageIndices.OrderBy(m => m.Key).Select(FormatSearchIndexEntry);
            string pageIndicesAsString = string.Join(",\n", mapEntries);

            writer.Write(pageIndicesAsString);
            writer.WriteLine("}}");
            writer.Close();
        }

        internal static string EscapeStringForJSON(string input)
        {
            string output = input.Replace("\t", "\\t");
            return output;
        }

        //replace isolated backslashes
        //(which are not already part of an escape sequence like \\ or \")
        internal static string ReplaceNonEscapeBackSlashes(string formatSearchResult)
        {
            var len = formatSearchResult.Length;
            var indexOfLastFoundBackslash = len - 1;
            while (indexOfLastFoundBackslash > 0)
            {
                var indexOf = formatSearchResult.LastIndexOf('\\', indexOfLastFoundBackslash);
                if (0 <= indexOf && indexOf < len)
                {
                    bool isPrevCharBackSlash = indexOf > 0 && formatSearchResult[indexOf - 1] == '\\';
                    if (isPrevCharBackSlash)
                        indexOf--;
                    bool isNextCharQuote = indexOf < len - 1 && formatSearchResult[indexOf + 1] == '\"';
                    if (!isPrevCharBackSlash && !isNextCharQuote)
                        formatSearchResult = formatSearchResult.Insert(indexOf, "\\");
                }
                indexOfLastFoundBackslash = indexOf - 1;
            }
            return formatSearchResult;
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
