using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace CompareCompilerCommandLines
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
                args = new[] { "jam", "old"};

            var one = args[0];
            var two = args[1];

            var text1 = File.ReadAllText(one);
            var text2 = File.ReadAllText(two);

            text1 = MakeVisualStudioCmdLineLookLikeJams(text1);
            text2 = MakeVisualStudioCmdLineLookLikeJams(text2);

            var split1 = text1.Split(' ');
            var split2 = text2.Split(' ');

            Console.WriteLine("Duplicates in jam:");
            PrintDupes(split1);

            Console.WriteLine("Duplicates in xcode:");
            PrintDupes(split2);

            Console.WriteLine("Items in Jambuild not in xcodebuild:");
            FindElementsNotInCollection(split1, split2);
            Console.WriteLine("\nItems in XCodebuild not in jambuild:");
            FindElementsNotInCollection(split2, split1);

            Console.ReadKey();
        }

        private static string MakeVisualStudioCmdLineLookLikeJams(string text1)
        {
            var regex = new Regex("/I ");
            text1 = regex.Replace(text1, "/I");
            var regex2 = new Regex("/D ");
            text1 = regex2.Replace(text1, "/D");

            var regex3 = new Regex("/D\"(?<define>.*?)\"");
            text1 = regex3.Replace(text1, m =>
            {
                return "/D" + m.Groups["define"].ToString();
            });


            return text1;
        }

        private static void PrintDupes(IEnumerable<string> split1)
        {
            foreach (var dupe in split1.Where(s => split1.Count(s2 => s2 == s) > 1))
            {
                Console.WriteLine(dupe);
            }
        }

        private static void FindElementsNotInCollection(IEnumerable<string> elements, IEnumerable<string> collection)
        {
            foreach (var line1 in elements)
            {
                if (collection.Contains(line1)) continue;
                if (line1.StartsWith("-F")) continue;
                if (line1.StartsWith("-I")) continue;
                if (line1.StartsWith("/I")) continue;
                if (line1.Contains("../../")) continue;
                if (line1.EndsWith(".o")) continue;
                if (line1.EndsWith("gcc-4.0")) continue;
                Console.WriteLine("Found: " + line1);
            }
        }
    }
}
