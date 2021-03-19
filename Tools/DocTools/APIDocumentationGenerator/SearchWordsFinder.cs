using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;

namespace APIDocumentationGenerator
{
    public interface ISearchWordsFinder
    {
        IEnumerable<string> GetSearchWordsList();
    }

    internal static class SearchWordsConstants
    {
        private const string commonWords = "a all also an and are as at attached be both by can class for "
            + "from function get if in int into is it just new no none not object of off on one only or print "
            + "ref returns see set that the this to two use used var when wiki will with you";

        public static string[]  m_CommonWordsList = commonWords.Split(' ');
        public static char[] m_Separators = {'\n', '\t', '\r', ' ', '.', ',', ':', '(', ')', '[', ']', '-', '_', '/', '=', '?', '!', '|', '&', ';', '*', '%', '#', '^', '}', '{', '~', '+', '$', '\\', '>', '`', '<', '"'};
    }

    public class SearchWordsXmlDocument : ISearchWordsFinder
    {
        private readonly XmlDocument m_XMLDoc;

        public SearchWordsXmlDocument(XmlDocument xmlDoc)
        {
            m_XMLDoc = xmlDoc;
        }

        public IEnumerable<string> GetSearchWordsList()
        {
            var foundWords = new HashSet<string>();

            var parentClasses = m_XMLDoc.GetElementsByTagName("ParentClass");

            if (parentClasses.Count > 0)
            {
                //there can be only one
                if (parentClasses.Count > 1)
                    Console.WriteLine("There should not be more than one parentclass element");

                var parentClassElement = parentClasses[0];
                SearchWordsPlainText.AddSubWordsFromText(parentClassElement.InnerText, foundWords);
            }

            AddLegalWordsFromXml(m_XMLDoc, "Summary", foundWords);

            AddLegalWordsFromXml(m_XMLDoc, "Description", foundWords);

            var signatureElements = m_XMLDoc.GetElementsByTagName("Declaration");
            foreach (XmlNode sig in signatureElements)
            {
                var nameAttrib = sig.Attributes["name"];
                SearchWordsPlainText.AddSubWordsFromText(nameAttrib.Value, foundWords);
            }
            return foundWords;
        }

        private static void AddLegalWordsFromXml(XmlDocument doc, string tag, ISet<string> foundWords)
        {
            foreach (XmlNode el in doc.GetElementsByTagName(tag))
                foreach (XmlNode child in el.ChildNodes.Cast<XmlNode>().Where(IsLegalElementName))
                    SearchWordsPlainText.AddSubWordsFromText(child.InnerText, foundWords);
        }

        //do not include terms from links and images
        //monotype also excluded, as it typically contains formulas
        private static Func<XmlNode, bool> IsLegalElementName
        {
            get { return child => child.Name != "image" && child.Name != "link" && child.Name != "monotype"; }
        }
    }
}
