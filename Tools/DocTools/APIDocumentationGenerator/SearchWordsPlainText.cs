using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using UnderlyingModel;

namespace APIDocumentationGenerator
{
    public static class SearchWordsPlainText
    {
        public static void AddSubWordsFromText(string text, ISet<string> foundSubWords)
        {
            string[] fullWords;
            if (DocGenSettings.Instance.Language == LanguageUtil.ELanguage.ja)
            {
                // In Japanese, words can't be simply split by spaces or punctuations.
                // We use TinySegmenter here.
                TinySegmenter segmenter = new TinySegmenter();
                fullWords = segmenter.Segment(text).ToArray();
            }
            else
            {
                //full words split by punctuation (can be compound ones like FindAtIndex)
                fullWords = text.Split(SearchWordsConstants.m_Separators, StringSplitOptions.RemoveEmptyEntries);
            }

            foreach (var word in fullWords)
                AddSubwordsFromWord(word, foundSubWords);
        }

        public static void AddSubwordsFromWord(string originalWord, ISet<string> foundWords)
        {
            var simplified = ReduceWord(originalWord);
            if (simplified.Length == 0)
            {
                return;
            }
            if (SearchWordsConstants.m_CommonWordsList.Contains(simplified))
            {
                return;
            }
            //only allow strings that have at least some letters in them, skip pure numbers and punctuation
            if (simplified.All(c => !char.IsLetter(c)))
            {
                return;
            }
            foundWords.Add(simplified);
        }

        public static bool IsNumber(string lcWord)
        {
            int num;
            return int.TryParse(lcWord, out num);
        }

        public static string ReduceWord(string word)
        {
            string simplified = word.ToLower().Trim();
            //this punctuation can be in the middle of a word, but ignore it at the ends
            return simplified.Trim(new[] {',', '@', ':', '\"', '\'', '<'});
        }
    }
}
