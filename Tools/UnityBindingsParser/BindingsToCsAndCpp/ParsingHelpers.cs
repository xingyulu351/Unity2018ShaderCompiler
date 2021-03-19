using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using Unity.CommonTools;
#if TEST
using NUnit.Framework;
#endif

namespace BindingsToCsAndCpp
{
#if TEST
    [TestFixture]
#endif
    internal class ParsingHelpers
    {
        public static readonly List<string> Structs = new List<string>();
        public static readonly List<string> Enums = new List<string>();

        static ParsingHelpers()
        {
            RetTypeMap.Add("ScriptingObjectOfType", "ICallType_Object_Return");
            RetTypeMap.Add("ICallScriptingObjectOfType", "ICallType_Object_Return");
            RetTypeMap.Add("RefCounted", "ICallType_Object_Return");
            RetTypeMap.Add("ScriptingObjectWithIntPtrField", "ICallType_Object_Return");
            RetTypeMap.Add("ReadOnlyScriptingObjectOfType", "ICallType_Object_Return");
            RetTypeMap.Add("ICallString", "ICallType_String_Return");
            RetTypeMap.Add("ScriptingSystemTypeObjectPtr", "ICallType_SystemTypeObject_Return");

            KnownStructs = MapBuilder.BuildFrom<string[]>(File.ReadAllLines(Path.Combine(Workspace.BasePath, "Runtime", "Export", "common_structs")), MapBuilder.MapType.Struct);
            Structs.AddRange(KnownStructs);
        }

#if TEST
        [TestCase("public Class : Stuff", "Class")]
        [Test]
        public void TestGetClassFrom(string content, string expected)
        {
            var res = GetClassFrom(content);
            Assert.AreEqual(res, expected);
        }

        [TestCase("public Class", "Class")]
        [Test]
        public void TestGetClassFrom2(string content, string expected)
        {
            var res = GetClassFrom(content);
            Assert.AreEqual(res, expected);
        }

        [TestCase("public void MyMethod() {yeah}", "public void MyMethod()", "yeah")]
        public void TestGetBodyFromCustomContent(string content, string expectedsignature, string expectedbody)
        {
            var result = SplitSignatureAndBodyFromCustom(content);
            Assert.AreEqual(expectedbody, result.Item2);
            Assert.AreEqual(expectedsignature, result.Item1);
        }

#endif

        public static string GetClassFrom(string content)
        {
            return GetLastWord(content, ":");
        }

        static string GetLastWord(string content, string end)
        {
            return GetLastWord(content, content.IndexOf(end));
        }

        public static string GetLastWord(string content, int end = -1)
        {
            if (end < 0)
                end = content.Length;
            var partial = content.Substring(0, end).Trim();
            var words = partial.Split(' ');
            return words[words.Length - 1];
        }

        public static Tuple<string, string, string, string> SplitSignatureAndBodyFromCustom(string content)
        {
            var newline = "\n";
            if (content.Contains("\r\n"))
                newline = "\r\n";

            var startSig = content.IndexOf("(");
            var endHeader = content.Substring(0, startSig).LastIndexOf('\n');
            var header = string.Empty;
            if (endHeader > 0)
                header = content.Substring(0, endHeader);
            else
                endHeader = 0;

            string signature = null;

            signature = content.Substring(endHeader, content.IndexOf(")") + 1);

            var body = string.Empty;

            var block = GetBlock(content, endHeader);
            body = block.Item1;

            var footer = string.Empty;
            if (block.Item3 < content.Length - 1)
            {
                footer = content.Substring(block.Item3 + 1);
                footer =
                    footer.Split("\r\n", StringSplitOptions.RemoveEmptyEntries).Where(x => x.Trim().StartsWith("#")).Join(newline);
            }
            return Tuple.Create(signature, body, header, footer);
        }

        public static string ExtractString(string content, string begin, string stop)
        {
            var start = content.IndexOf(begin) + 1;
            var end = content.LastIndexOf(stop);

            var ret = content.Substring(start, end - start);
            return ret;
        }

        public static string GetBody(string content)
        {
            return ExtractString(content, "{", "}");
        }

        public static Tuple<string, int, int> GetBlock(string content, int start = 0)
        {
            if (start < 0 || start > content.Length || !content.Substring(start).Contains('{'))
                return Tuple.Create((string)null, -1, -1);
            var level = 0;
            var blockStart = -1;
            var blockEnd = -1;

            var lines = content.Substring(start).Split('\n');
            foreach (var line in lines)
            {
                if (line.TrimStart().StartsWith("//"))
                {
                    start += line.Length + 1;
                    continue;
                }
                if (line.Contains('{'))
                    break;
            }

            if (start < 0 || start > content.Length || !content.Substring(start).Contains('{'))
                return Tuple.Create((string)null, -1, -1);

            for (var i = start; i < content.Length; i++)
            {
                var c = content[i];
                if (c == '{')
                {
                    if (level == 0)
                    {
                        if (blockStart < 0)
                            blockStart = i;
                    }
                    level++;
                }
                else if (c == '}')
                {
                    level--;
                    if (level < 0)
                        throw new UnbalancedBracesException(content.Substring(start));
                    if (level == 0)
                    {
                        if (blockStart > 0)
                        {
                            blockEnd = i;
                            break;
                        }
                    }
                }
            }
            if (level != 0)
                throw new UnbalancedBracesException(content.Substring(start));
            return Tuple.Create(content.Substring(blockStart + 1, blockEnd - blockStart - 1), blockStart, blockEnd + 1);
        }

        public static List<string> SplitBlocks(string content, int start)
        {
            var blocks = new List<string>();
            do
            {
                var block = GetBlock(content, start);
                if (block.Item2 >= 0)
                    blocks.Add(content.Substring(start, block.Item3 - start));
                start = block.Item3 + 1;
            }
            while (start > 0);
            return blocks;
        }

        public static Tuple<string, string, string> SplitSignatureGetterAndSetterFromCustomProp(string content)
        {
            content = content.Clean();

            var getterPosition = -1;

            var getterBlock = GetBlock(content);
            if (getterBlock.Item1 == null)
                throw new ArgumentException("No getter body in CUSTOMPROP block", content);

            var getter = getterBlock.Item1;
            getterPosition = getterBlock.Item2;
            var setterBlock = GetBlock(content, getterBlock.Item3);
            var setter = setterBlock.Item1;

            var signature = content.Substring(0, getterPosition);
            return Tuple.Create(signature.Trim(), getter.Trim(), setter);
        }

        public static bool IsStruct(string typeName)
        {
            return Structs.Contains(typeName);
        }

        public static Tuple<string, string, string> ParseAutoProp(string codeContent)
        {
            codeContent = codeContent.Trim();
            var parts = codeContent.Split(' ', StringSplitOptions.RemoveEmptyEntries);

            var name = parts[1];
            var cgetter = parts[2];
            var csetter = parts.Length > 3 ? parts[3] : null;

            var nameStart = codeContent.IndexOf(name) + name.Length;

            return Tuple.Create(codeContent.Substring(0, nameStart), cgetter, csetter);
        }

        public static Tuple<string, string, string> ParseEnum(string content, string defaultVisibility = "public")
        {
            string visibility, type;
            var name = visibility = type = null;
            var idx = content.IndexOf(':');
            if (idx > 0)
            {
                type = content.Substring(idx + 1).Trim();
                content = content.Substring(0, idx);
            }
            var parts = content.Split(' ', StringSplitOptions.RemoveEmptyEntries);
            switch (parts.Length)
            {
                case 1:
                    name = parts[0];
                    break;
                case 2:
                    visibility = parts[0];
                    name = parts[1];
                    break;
            }

            if (visibility == null)
                visibility = defaultVisibility;
            return Tuple.Create(name, visibility, type);
        }

        public readonly static string[] KnownStructs;

        public static Dictionary<string, string> RetTypeMap = new Dictionary<string, string>();

        public static string NormalizeSignatureSpacing(string content)
        {
            return string.Join(" ", content.Split(new[] {" "}, StringSplitOptions.RemoveEmptyEntries));
        }

        public static string GenerateObsolete(string obsoleteKind, string obsolete)
        {
            var str = string.Format("[{0} (\"{1}\"", "System.Obsolete", obsolete);
            if (obsoleteKind == "error")
                str += ", true";
            str += ")]";
            return str;
        }

        public static string RemoveLineComment(string line)
        {
            if (line.TrimStart().IndexOf("//") == 0)
                return null;

            var str = line;
            var idx = str.IndexOf("//");
            while (idx > 0)
            {
                var count = 0;
                for (var i = 0; i < idx; i++)
                {
                    if (line[i] == '"' && (i == 0 || (line[i - 1] != '\\' && line[i - 1] != '\'')))
                        count++;
                }

                if ((count % 2) == 1)
                {
                    if (idx + 2 >= str.Length)
                        idx = -1;
                    else
                        idx = str.IndexOf("//", idx + 2);
                }
                else
                    break;
            }

            // yeah, don't remove url parts...
            if (idx > 0 && str[idx - 1] != ':')
                str = str.Substring(0, idx);
            return str;
        }

        public static string RemoveBlockComment(string line, int padding = 0)
        {
            var idx = line.IndexOf("/*");
            while (idx >= 0)
            {
                var end = line.IndexOf("*/", idx);
                if (end > idx)
                {
                    line = line.Substring(0, idx) + "".PadRight(padding) + line.Substring(end + 2);
                    idx = line.IndexOf("/*");
                }
                else
                    idx = -1;
            }
            return line;
        }

        public static string RemoveComments(string[] lines)
        {
            var snippet = lines.Select(x => RemoveLineComment(x)).Where(l => l != null).Join("\n");
            return RemoveBlockComment(snippet, 1);
        }

        /// <summary>
        /// Removes // and /**/ comments from a single line
        /// </summary>
        /// <param name="line"></param>
        /// <returns></returns>
        public static string RemoveCommentsFromLine(string line)
        {
            return RemoveLineComment(RemoveBlockComment(line));
        }

        public static string RecreateAutoProp(string code)
        {
            var prop = ParseAutoProp(code);
            var content = new IndentedWriter();
            content.AppendLine(prop.Item1);
            content.AppendLine("{");
            content.AppendLine(string.Format("return self->{0} ();", prop.Item2));
            content.AppendLine("}");
            if (string.IsNullOrEmpty(prop.Item3))
                return content.ToString();
            content.AppendLine("{");
            content.AppendLine(string.Format("self->{0} (value);", prop.Item3));
            content.AppendLine("}");
            return content.ToString();
        }

        public static List<Tuple<string, bool, bool>> ParseRawCS(string code)
        {
            var pos = 0;

            var blocks = new List<Tuple<string, bool, bool>>();
            var lines = code.Split('\n');

            for (var i = 0; i < lines.Length; i++, pos++)
            {
                var l = lines[i];
                if (string.IsNullOrWhiteSpace(l))
                {
                    blocks.Add(Tuple.Create(l, false, true));
                    pos += l.Length;
                    continue;
                }

                var line = l.Trim();

                if (line.StartsWith("#") || line.EndsWith(";") || line.EndsWith("]"))
                {
                    blocks.Add(Tuple.Create(l, true, false));
                    pos += l.Length;
                    continue;
                }

                Tuple<string, int, int> block = null;
                try
                {
                    block = GetBlock(code, pos);
                }
                catch (UnbalancedBracesException)
                {
                    // just add all the lines to a block, no point parsing any further
                    for (; i < lines.Length; i++)
                    {
                        blocks.Add(Tuple.Create(lines[i], false, false));
                    }
                    break;
                }
                catch (Exception)
                {
                    Debugger.Break();
                }

                if (block.Item2 < 0)
                {
                    blocks.Add(Tuple.Create(l, true, false));
                    pos += l.Length;
                    continue;
                }

                var iw = new IndentedWriter();
                iw.Append(l);
                //blocks.Add (Tuple.Create(code.Substring(pos, block.Item3 - pos), true, false));
                pos += l.Length + 1;
                while (pos < block.Item3)
                {
                    l = lines[++i];
                    iw.AppendLine();
                    iw.Append(l);
                    pos += l.Length + 1;
                }
                blocks.Add(Tuple.Create(iw.Output, true, false));
                pos--;
            }
            return blocks;
        }
    }

    public static class EnumerableExtensions
    {
        public static IEnumerable<T> Prepend<T>(this IEnumerable<T> items, T prepended)
        {
            return new[] { prepended }.Concat(items);
        }

        public static string CommaSeparatedList(this IEnumerable<string> items)
        {
            return string.Join(", ", items.ToArray());
        }

        public static string Join(this IEnumerable<string> items, string separator)
        {
            return string.Join(separator, items.ToArray());
        }
    }

    public static class StringExtensions
    {
        public static string Clean(this string str)
        {
            str = Clean(str, "\r\n", "\n");
            var lines = str.Split(new[] {"\n"}, StringSplitOptions.RemoveEmptyEntries);
            var sb = new IndentedWriter();
            foreach (var line in lines)
            {
                if (string.IsNullOrWhiteSpace(line))
                    continue;
                sb.AppendLine(line.Trim());
            }
            str = sb.ToString().Trim();
            str = Clean(str, "\r\n", "\n");
            return str;
        }

        public static string ToStringOrEmpty(this string str, bool shouldBeEmpty)
        {
            return shouldBeEmpty ? string.Empty : str;
        }

        public static string[] Split(this string str, string separator, StringSplitOptions opts)
        {
            return str.Split(new[] {separator}, opts);
        }

        public static string[] Split(this string str, char separator, StringSplitOptions opts)
        {
            return str.Split(new[] { separator.ToString() }, opts);
        }

        public static string Clean(this string str, string s, string s1)
        {
            while (str.IndexOf(s, StringComparison.InvariantCulture) >= 0)
                str = str.Replace(s, s1);
            return str;
        }
    }

    public static class StringBuilderExtensions
    {
        public static void AppendLineFormat(this StringBuilder sb, string format, params object[] args)
        {
            sb.AppendFormat(format, args);
            sb.Append("\n");
        }

        public static void AppendLineUnix(this StringBuilder sb)
        {
            sb.Append("\n");
        }

        public static void AppendLineUnix(this StringBuilder sb, string value)
        {
            sb.Append(value).Append("\n");
        }

        public static StringBuilder NormalizeUnix(this StringBuilder sb)
        {
            return sb.Replace("\r\n", "\n");
        }
    }
}
